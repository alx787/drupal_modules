<?php

function alxexchange_init_priceload() {

	///////////////////////////////////
	// отключаем модуль pathauto
	///////////////////////////////////
	$modules = array('pathauto');
	module_disable($modules);	


	alxexchange_set_catalog_published_off();

	$mas = alxexchange_load_catalog_in_massiv();

	//dpm(count($mas));

	$operations = array();
	foreach ($mas as $record) {
		$operations[] = array('alxexchange_process_catalog', array($record));
	}


	// для отладки
	//$operations[] = array('process_product', array($strings[3]));

	$batch = array(
		'file' => drupal_get_path('module', 'alxexchange') . '/module.catalog.inc',
		// Массив операций и их параметров
		'operations' => $operations,
		// Функция, которая будет выполнена после окончания всех операций
		'finished' => 'alxexchange_batch_processing_finished_catalog',
		// Заголовок страницы с прогресс баром.
		// Опционально, по умолчанию t('Processing')
		'title' => 'Загрузка прайс листа в каталог',
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

// получить массив каталога из базы данных
function alxexchange_load_catalog_in_massiv() {

	$massiv_catalog = array();

	$sql = 'SELECT catalog.id,
				catalog.groupid, 
				catalog.date, 
				catalog.name, 
				catalog.nomerizd, 
				catalog.cover,
				catalog.agelimit,
				catalog.izdat,
				catalog.godizdan,
				catalog.pages,
				catalog.author,
				catalog.isbn,
				catalog.annot,
				catalog.content,
				catalog.price,
				catalog.cnt,
				catalog.published,
				catalog.supplcode,
				ifnull(catstruct.levname, \'\') as gruppa,
				ifnull(categ.levname, \'\') as category
			FROM {catalog} catalog
			left join {catstruct} catstruct
				on catalog.groupid = catstruct.levelid
			left join (select levelid, levname from catstruct where length(levelid) = 2) categ
				on left(catalog.groupid, 2) = categ.levelid
			where catalog.published = 0
			limit 10';
			//limit 5000';

//			where catalog.id = :catid

	//$args = array(':published' => 0);
	//$args = array(':catid' => '00081490');
	$args = array();
	$result = db_query($sql, $args);

	$i = 0;
	while ($record = $result->fetchAssoc()) {

		$i++;
		$record['i'] = $i;

		$massiv_catalog[] = $record;
	}
	return $massiv_catalog;

	// return $result->fetchAllAssoc('id');
}


// убрать публикацию с нод
function alxexchange_set_catalog_published_off() {
	// снимаем признак публикации с товаров
	db_query("UPDATE {node} SET status = :status WHERE type = :type", array(':status' => 0, ':type' => 'product_display'));
}

// убрать публикацию с нод, с которых убрана публикация в таблице catalog
function alxexchange_set_status_off() {
	$result = db_query("select id from {catalog} where published = 0")->fetchAll();

	$mas_id = array();
	foreach ($result as $oneRes) {
		$mas_id[] = $oneRes->id;
	}


	$query = new EntityFieldQuery();

	$query->entityCondition('entity_type', 'node')
		->entityCondition('bundle', 'product_display')
		->propertyCondition('status', 1)
		->fieldCondition('field_codnom', 'value', $mas_id, 'IN');
//		->addMetaData('account', user_load(1));

	$result = $query->execute();	

	// echo count($result) . "\n";

	if (isset($result['node'])) {
		$nids = array_keys($result['node']);
		$nodes = entity_load('node', $nids);
	}

	foreach ($nodes as $node) {
		$node->status = 0;
		node_save($node);

		//echo $node->status . "\n";
		//print_r($node);
	}


}

function alxexchange_process_catalog($oneProduct, &$context) {

	//watchdog('alx_notice',  $oneProduct['i'] . ' ' . $oneProduct['name']);

	//return TRUE;

	////////////////////////////////////////
	// запрос через апи
	////////////////////////////////////////
	$result = db_query('select product_id, sku from {commerce_product} where sku = :sku', array('sku' => $oneProduct['id']));

	if ($record = $result->fetch()) {
		// найдена существующая позиция product
		$cp = commerce_product_load($record->product_id);
		//dpm('найдена');
	} else {
		// создание новой позиции product
		$cp = commerce_product_new('product');
		//dpm('новая');
	}


	$cp->is_new           = FALSE;
	//$cp->revision_id      = NULL;
	$cp->uid              = 1;
	$cp->status           = TRUE;
	$cp->created          = $cp->changed = time();
	$cp->sku              = $oneProduct['id'];
	$cp->title            = $oneProduct['name'];
	$cp->language         = LANGUAGE_NONE;
	$cp->commerce_price   = array(LANGUAGE_NONE => array( 0 => array('amount' => $oneProduct['price'] * 100, 'currency_code' => 'RUB', )));

	// дополнительное кастомное поле
	$cp->field_idcode[LANGUAGE_NONE][0]['value'] = $oneProduct['id'];

	// сохранение
	$new_cp_id = commerce_product_save($cp);


	// ищем ноду которая соответствует товару
	$result = db_query('select entity_id from {field_data_field_product} where field_product_product_id = :product_id and bundle = :bundle',
	              array('product_id' => $cp->product_id, 'bundle' => 'product_display'));


	if ($record = $result->fetch()) {
		// редактирование существующей позиции product_display
		$node = node_load($record->entity_id);
	} else {
		// создание новой позиции product_display
		$node = new stdClass();
		$node->type = 'product_display';
		node_object_prepare($node);
		$node->field_product[LANGUAGE_NONE][]['product_id'] = $cp->product_id;
	}



	$node->title = $oneProduct['name'];
	//$node->body['und'][0]['value'] = 'описание товара';

	$node->uid = 1;
	$node->language = LANGUAGE_NONE;
	$node->promote = 0;
	$node->status = 1; // опубликовано

	// $node->field_product[LANGUAGE_NONE][]['product_id'] = $cp->product_id;

	//$node->field_godizdan[LANGUAGE_NONE][0]['value'] = '2016';

	// категория товара
	if (!empty($oneProduct['category'])) {
		$node->field_category['und'][0]['tid'] = alxexchange_get_taxonomy_tid($oneProduct['category'], 'category');
	}

	// издательство
	if (!empty($oneProduct['izdat'])) {
		$node->field_izdatelstvo['und'][0]['tid'] = alxexchange_get_taxonomy_tid($oneProduct['izdat'], 'izdatelstvo');
	}

	// автор
	if (!empty($oneProduct['author'])) {
		$node->field_author['und'][0]['tid'] = alxexchange_get_taxonomy_tid($oneProduct['author'], 'author');
	}

	// возраст
	if (!empty($oneProduct['agelimit'])) {
		$node->field_agelimit['und'][0]['tid'] = alxexchange_get_taxonomy_tid($oneProduct['agelimit'], 'agelimit');
	}

	// год издания
	if (!empty($oneProduct['godizdan'])) {
		$node->field_godizdan['und'][0]['value'] = $oneProduct['godizdan'];
	}


	// группа товара
	if (!empty($oneProduct['gruppa'])) {
		$node->field_group['und'][0]['tid'] = alxexchange_get_taxonomy_tid($oneProduct['gruppa'], 'productgroups');
	}

	// isbn
	if (!empty($oneProduct['isbn'])) {
		$node->field_isbn['und'][0]['value'] = $oneProduct['isbn'];
	}

	// 
	if (!empty($oneProduct['pereplet'])) {
		$node->field_cover['und'][0]['value'] = $oneProduct['cover'];
	}

	// 
	if (!empty($oneProduct['pages'])) {
		$node->field_pages['und'][0]['value'] = $oneProduct['pages'];
	}

	if (!empty($oneProduct['nomerizd'])) {
		$node->field_nomerizd['und'][0]['value'] = $oneProduct['nomerizd'];
	}

	if (!empty($oneProduct['id'])) {
		$node->field_codnom['und'][0]['value'] = $oneProduct['id'];
	}




	node_save($node);

	// установим признак 'опубликовано'
	//$oneProduct['id']
	db_update('catalog')->fields(array('published' => 1))->condition('id', $oneProduct['id'])->execute();


	//watchdog('catalog', $oneProduct['id']);

	// Эта информация будет доступна в mymodule_batch_finished
	$context['results'][] = $oneProduct['name'];
	// Сообщение выводимое под прогресс баром после окончания текущей операции
	//$context['message'] = 'Обновлена дата у материала <em>' . $node->title . '</em>';
	$context['message'] = 'Загрузка ' . $oneProduct['id'] . ' '. $oneProduct['name'];

}


function alxexchange_batch_processing_finished_catalog($success, $results, $operations) {
	///////////////////////////////////
	// включаем модуль pathauto
	$modules = array('pathauto');
	module_enable($modules);
	///////////////////////////////////

	if ($success) {
	    //drupal_set_message('Загружено ' . count($results) . ' товаров:' . theme('item_list', array('items' => $results)));
	    drupal_set_message('Загружено ' . count($results) . ' товаров');
	} else {
		drupal_set_message('Завершено с ошибками.', 'error');
	}

}


// поиск термина таксономии и создание его если термин не найден
// $term_name - название термина
// $vocabulary - название словаря
function alxexchange_get_taxonomy_tid($term_name, $vocabulary_machine_name) {
	$tid = '';

	$arr_terms = taxonomy_get_term_by_name($term_name, $vocabulary_machine_name);
	if (!empty($arr_terms)) {
		$arr_terms = array_values($arr_terms);
		$tid = $arr_terms[0]->tid;
	} else {
		$vocabulary = taxonomy_vocabulary_machine_name_load($vocabulary_machine_name);
		$term = (object)array(
			'vid' => $vocabulary->vid,
			'name' => $term_name,
			// родитель
			//'parent' => $tid,      
		);
		taxonomy_term_save($term);
		$tid = $term->tid;
	};

	return $tid;
}

