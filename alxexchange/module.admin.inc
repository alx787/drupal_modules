<?php

// форма с кнопками
function alxexchange_form($form_state, &$form_submit) {

	$form['notice_msg'] = array(
		'#type' => 'item',
		'#title' => 'Все процедуры модуля выполняются через drupal_bootstrap',
		//'#markup' => '',
	);


	$form['total_count'] = array(
		'#type' => 'item',
		'#title' => 'Количество позиций в таблице обмена',
		'#markup' => alxexchange_getValForms('total'),
	);

	$form['load_count'] = array(
		'#type' => 'item',
		'#title' => 'Количество загруженный позиций',
		'#markup' => alxexchange_getValForms('load'),
	);



	$form['submit_catalog'] = array(
		'#type' => 'submit',
		'#name' => 'catalog',
		'#value' => 'Загрузить каталог товаров',
		'#disabled' => TRUE,
	);

	$form['submit_structure'] = array(
		'#type' => 'submit',
		'#name' => 'structure',
		'#value' => 'Загрузить структруру каталога',
		'#disabled' => TRUE,
	);

	$form['submit_orders'] = array(
		'#type' => 'submit',
		'#name' => 'orders',
		'#value' => 'Выгрузить заказы',
		'#disabled' => TRUE,
	);

	return $form;
}


// вычисление значений полей на форме
function alxexchange_getValForms($pos_val) {
	$count = -1;

	if ($pos_val == 'total') {
		$count = db_query("SELECT COUNT(*) FROM {catalog} n")->fetchField();
	}

	if ($pos_val == 'load') {
		$count = db_query("SELECT COUNT(*) FROM {catalog} n WHERE n.published = 1")->fetchField();
	}

	return $count;
}


// обработчик нажатых кнопок
function alxexchange_form_submit($form, &$form_state) {

	switch ($form_state['clicked_button']['#name']) {
		case "catalog":
			drupal_set_message('Каталог');
			module_load_include('inc', 'alxexchange', 'module.catalog');
			alxexchange_init_priceload();
			break;
		case "structure":
			drupal_set_message('Структура каталога');
			module_load_include('inc', 'alxexchange', 'module.struct');
			alxexchange_init_structload();
			break;
		case "orders":
			drupal_set_message('Заказы');
			module_load_include('inc', 'alxexchange', 'module.orders');
			alxexchange_init_ordersunload();
			break;
	}

}

