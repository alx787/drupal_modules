<?php

function alxexchange_init_ordersunload() {

	$mas = alxexchange_load_orders_in_massiv();

	//// для отладки
	// foreach ($mas as $record) {
	// 	$context = array();
	// 	alxexchange_process_order($record, $context);
	// }

	// return TRUE;

	$operations = array();
	foreach ($mas as $record) {
		$operations[] = array('alxexchange_process_order', array($record));
	}

	// для отладки
	//$operations[] = array('process_product', array($strings[3]));

	$batch = array(
		'file' => drupal_get_path('module', 'alxexchange') . '/module.orders.inc',
		// Массив операций и их параметров
		'operations' => $operations,
		// Функция, которая будет выполнена после окончания всех операций
		'finished' => 'alxexchange_batch_processing_finished_orders',
		// Заголовок страницы с прогресс баром.
		// Опционально, по умолчанию t('Processing')
		'title' => 'Выгрузка заказов',
		// Сообщение, показываемое при инициализации.
		// Опционально, по умолчанию t('Initializing.')
		'init_message' => 'Подготовка данных',
		// Сообщение, показывемое при выполнении операций.
		// Опционально, по умолчанию t('Completed @current of @total.')
		'progress_message' => 'Выполнено @current из @total.',
		// Сообщение показываемое при ошибке выполнения операции.
		// Опционально, по умолчанию t('An error has occurred.')
		'error_message' => 'Произошла ошибка.',
	);


	batch_set($batch);

}


// собираем созданные заказы
function alxexchange_load_orders_in_massiv() {
	$massiv_orders = array();
	$massiv_orders = commerce_order_load_multiple(array(), array('status' => 'pending'), TRUE);
	return $massiv_orders;
}

// обработка заказов - пакетная процедура
function alxexchange_process_order($oneOrder, &$context) {

	//////////////////////////////////////////////
	// выгрузка шапки заказа
	//////////////////////////////////////////////

	$datetime = date("Y.m.d H:i:s", $oneOrder->created);
	$user = $oneOrder->uid;

	$body = 'номер заказа на сайте: ' . $oneOrder->order_number;
	// адрес - добавим в боди если заведен адрес
	if (isset($user_profile->field_address['und'][0]['safe_value'])) {
		$body = $body . '\n' . 'указан адрес:' . '\n' . $user_profile->field_address['und'][0]['safe_value'];
	}


	$status = 'created';
	$oper = 0;
	$notinprice = 0;

	$field_values = field_get_items('commerce_order', $oneOrder, 'commerce_customer_billing');
	$user_profile_id = $field_values[0]['profile_id'];
	$user_profile = commerce_customer_profile_load($user_profile_id);

	$clientname = '';

	if (isset($user_profile->field_customer_name['und'][0]['safe_value'])) {
		$clientname = $user_profile->field_customer_name['und'][0]['safe_value'];
	}

	$clientphone = '';

	if (isset($user_profile->field_phone['und'][0]['safe_value'])) {
		$clientphone = $user_profile->field_phone['und'][0]['safe_value'];
	}

	$comment = '';

	if (isset($user_profile->field_comment['und'][0]['safe_value'])) {
		$comment = $user_profile->field_comment['und'][0]['safe_value'];
	}



	$sql = db_insert('orders');
	$sql->fields(array(
					'datetime' 		=> $datetime, 
					'user' 			=> $user, 
					'body' 			=> $body, 
					'status' 		=> $status, 
					'oper' 			=> $oper, 
					'notinprice' 	=> $notinprice, 
					'clientname' 	=> $clientname, 
					'clientphone' 	=> $clientphone, 
					'comment' 		=> $comment,
						));

	$last_insert_id = $sql->execute();
	if (!$last_insert_id) {
		watchdog('warning', 'cant unload order ' . $order->order_id);
	}


	//watchdog('alx_notice', $last_insert_id);

	//////////////////////////////////////////////
	// выгрузка таблицы заказа
	//////////////////////////////////////////////

	// // обработка товаров заказа
	// хотел изначально сделать так но потом через апи
	// // foreach ($order->commerce_line_items['und'] as $lineItem) {
	// // 	dpm($lineItem);
	// // }

	/////////////////////////////////////////////////////////////////
	// этот код спизжен весь целиком с какого то сайта
	// все сделано через ебучее апи коммерца
	// я понимаю с трудом, но оно работает
	/////////////////////////////////////////////////////////////////
	// $order_lines = field_get_items('commerce_order', $order, 'commerce_line_items');
	// // Line item IDs
	// $line_item_ids = array();
	// 	foreach ($order_lines as $order_line) {
	// 	$line_item_ids[] = $order_line['line_item_id'];
	// }

	// // Load line item details
	// $line_items = commerce_line_item_load_multiple($line_item_ids);

	// // Get the product IDs
	// $product_ids = array();
	// foreach ($line_items as $line_item) {
	// 	$tmp = field_get_items('commerce_line_item', $line_item, 'commerce_product');
	// 	$product_ids[] = $tmp[0]['product_id'];
	// }

	// // Load all to get product details
	// $products = commerce_product_load_multiple($product_ids);			
	/////////////////////////////////////////////////////////////////
	// конец
	/////////////////////////////////////////////////////////////////

	$sql_val = array();

	$order_lines = field_get_items('commerce_order', $oneOrder, 'commerce_line_items');
	foreach ($order_lines as $order_line) {

		$line_item = commerce_line_item_load($order_line['line_item_id']);

		$tmp = field_get_items('commerce_line_item', $line_item, 'commerce_product');
	 	$product_id = $tmp[0]['product_id'];

		$product = commerce_product_load($product_id);


		$orderid = $last_insert_id;
		$oper = 0;
		$itemid = $product->sku;
		$item = $product->title;
		$price = $line_item->commerce_unit_price['und'][0]['amount'];
		$cnt = $line_item->quantity;
		$comment = '';
		$status = 'CREATED';

		$author = '';
		$izdat = '';
		$isbn = '';
		$pereplet = '';
		$godizdan = '';


		// здесь мы будем искать нашу ноду product_display к которой привязана commerce_product

		$query = new EntityFieldQuery();

		$result = $query
			->entityCondition('entity_type', 'node', '=')
			->entityCondition('bundle', 'product_display', '=')
			->fieldCondition('field_product', 'product_id', $product->product_id, '=')
			->range(0, 1);

		if ($result = $query->execute()) {

			$arr_keys = array_keys($result['node']);
			$node_id = array_shift($arr_keys);

			$node = node_load($node_id);

			// автор
			if (!empty($node->field_author)) {
				$items = field_get_items('node', $node, 'field_author');
				$output = field_view_value('node', $node, 'field_author', $items[0]);
				$author = $output['#title'];
			}

			// издательство
			if (!empty($node->field_izdatelstvo)) {
				// $term = taxonomy_term_load($node->field_izdatelstvo['und'][0]['tid']);
				// $termname = $term->name;
				// dpm($termname);

				$items = field_get_items('node', $node, 'field_izdatelstvo');
				$output = field_view_value('node', $node, 'field_izdatelstvo', $items[0]);
				$izdat = $output['#title'];
			}

			// isbn
			if (!empty($node->field_isbn)) {
				$items = field_get_items('node', $node, 'field_isbn');
				$output = field_view_value('node', $node, 'field_isbn', $items[0]);
				$isbn = $output['#markup'];
			}

			// переплет - обложка
			if (!empty($node->field_cover)) {
				$items = field_get_items('node', $node, 'field_cover');
				$output = field_view_value('node', $node, 'field_cover', $items[0]);
				$pereplet = trim($output['#markup']);

				if (strlen($pereplet) > 10) {
					$pereplet = mb_substr($pereplet, 0, 10);
				}

			}

			// год издания
			if (!empty($node->field_godizdan)) {
				$items = field_get_items('node', $node, 'field_godizdan');
				$output = field_view_value('node', $node, 'field_godizdan', $items[0]);
				$godizdan = $output['#markup'];
			}
		}
			

		$sql_val_row = array(
								'orderid' => $orderid,
								'oper' => $oper,
								'itemid' => $itemid,
								'item' => $item,
								'price' => $price,
								'cnt' => $cnt,
								'comment' => $comment,
								'author' => $author,
								'izdat' => $izdat,
								'isbn' => $isbn,
								'pereplet' => $pereplet,
								'godizdan' => $godizdan,
								'status' => $status,
						);

		$sql_val[] = $sql_val_row;

	}


	$sql = db_insert('orderitems');
	$sql->fields(array(
					'orderid',
					'oper',
					'itemid',
					'item',
					'price',
					'cnt',
					'comment',
					'author',
					'izdat',
					'isbn',
					'pereplet',
					'godizdan',
					'status',
				));

	foreach ($sql_val as $record) {
  		$sql->values($record);
	}
	$sql->execute();

	// меняем статус заказа на завершено
	commerce_order_status_update($oneOrder, 'completed', FALSE, FALSE);


	$context['results'][] = $orderid;
	// Сообщение выводимое под прогресс баром после окончания текущей операции
	//$context['message'] = 'Обновлена дата у материала <em>' . $node->title . '</em>';
	$context['message'] = 'Выгрузка заказов ' . $clientname;

}


function alxexchange_batch_processing_finished_orders($success, $results, $operations) {
	if ($success) {
	    drupal_set_message('Выгружено ' . count($results) . ' товаров');
	} else {
		drupal_set_message('Завершено с ошибками.', 'error');
	}
}
