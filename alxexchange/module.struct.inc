<?php

// получение массива групп с родительскими элементами из базы
function alxexchange_load_groups_in_massiv() {

	$massiv_grupp = array();

		// получаем записи из базы
	$sql = 'SELECT levelid,
					parcode, 
					levcode, 
					levorder, 
					levname, 
					gruppa
			FROM {catstruct}';

	$args = array();
	$result = db_query($sql, $args);

	while ($record = $result->fetch()) {
		// выделяем код группы
		$group_code = '';
	    // код строки в массив
    	$stringCodes = explode('-' , $record->levelid);

		if (count($stringCodes) > 1) {
		    $group_code = array_slice($stringCodes, 0, count($stringCodes) - 1);
		    $group_code = implode('-', $group_code);
		}

		$massiv_grupp[] = array('code' => $record->levelid, 'parentcode' => $group_code, 'name' => $record->levname);
	}

	// расставим в термины таксономии имена родительских групп
	$i = 0; $j = count($massiv_grupp) - 1;

	for ($i = 0; $i <= $j; $i++) {
		if (!isset($massiv_grupp[$i]['parentname'])) {
			$massiv_grupp[$i]['parentname'] = alxexchange_searchGrupp($massiv_grupp[$i], $massiv_grupp);
		}

	}
	return $massiv_grupp;
}


// для расстановки имен родительских групп
function alxexchange_searchGrupp($one, &$arr) {
  // эта группа родительская
  if (empty($one['parentcode'])) {
    return '';
  }

  // ищем имя родительской группы по коду
  $find_parent_name = '';
  $i = 0; $j = count($arr) - 1;
  for($i = 0; $i <= $j; $i++) {
    if ($one['parentcode'] == $arr[$i]['code']) {

      $find_parent_name = $arr[$i]['name'];

      if (!isset($arr[$i]['parentcode'])) {
        $arr[$i]['parentcode'] = searchGrupp($arr[$i]['code'], $arr);
      }
      break;
    }
  }

  // чтобы уменьшить количество вызовов функции сразу же расставим
  // родительскую группу у всех элементов входящих в эту группу
  if (!empty($find_parent_name)) {
    $i = 0; $j = count($arr) - 1;
    for($i = 0; $i <= $j; $i++) {
      if (!isset($arr[$i]['parentname'])) {
        if ($one['parentcode'] == $arr[$i]['parentcode']) {
          $arr[$i]['parentname'] = $find_parent_name;
        }
      }
    }
  }

  return $find_parent_name;
}


///////////////////////////////////////////////////////////
// основная процедура
///////////////////////////////////////////////////////////
// запись структуры в таксономию друпала
// поиск термина таксономии и создание его если термин не найден
// $term_name - название термина
// $vocabulary - название словаря
function alxexchange_get_taxonomy_tid_struct($one, $arr, $vocabulary) {
//function get_taxonomy_tid_struct($term_name, $term_parent_name, &$arr, $vocabulary) {
	$tid = '';

	// сначала проверим есть ли такой термин
	$arr_terms = taxonomy_get_term_by_name($one['name'], $vocabulary->machine_name);
  
	if (!empty($arr_terms)) {
		// термин найден
		$arr_terms = array_values($arr_terms);
		//$tid = $arr_terms[0]->tid;
		$term = $arr_terms[0];
	} else {
		// термин не найден
		$term = new stdClass();
		$term->name = $one['name'];
		$term->vid = $vocabulary->vid;
	};

	// родитель
	//'parent' => $tid,
	if (isset($term->parent)) {
		unset($term->parent);
	}
  
	if (!empty($one['parentname'])) {
		// находим в массиве элемент с родительским элементом
		$i = 0; $j = count($arr) - 1;
		for ($i = 0; $i <= $j; $i++) {
			if ($arr[$i]['name'] == $one['parentname']) {
				// рекурсивный вызов процедуры
				$parent_tid = alxexchange_get_taxonomy_tid_struct($arr[$i], $arr, $vocabulary, NULL);
				//
				$term->parent = $parent_tid;
				break;
			}
		}
	}
  
	taxonomy_term_save($term);

	$tid = $term->tid;

	return $tid;
}


// получаем массив категорий из базы
function alxexchange_load_categories_in_massiv() {
	$massiv_categ = array();

	// получаем записи из базы
	$sql = 'select levelid, levname from {catstruct} where length(levelid) = 2';
	$args = array();
	$result = db_query($sql, $args);

	while ($record = $result->fetch()) {
		$massiv_categ[] = array('code' => $record->levelid, 'parentcode' => '', 'name' => $record->levname, 'parentname'=> '');
	}

	return $massiv_categ;
}


// процедура пакетной обработки
function alxexchange_process_structload($one, $arr, $vocabulary, &$context) {
	$tid = alxexchange_get_taxonomy_tid_struct($one, $arr, $vocabulary);

	// Эта информация будет доступна в mymodule_batch_finished
	$context['results'][] = $tid;
	// Сообщение выводимое под прогресс баром после окончания текущей операции
	//$context['message'] = 'Обновлена дата у материала <em>' . $node->title . '</em>';
	$context['message'] = 'Загрузка словаря ' . $vocabulary->machine_name . ' термин ' . $one['name'];
}

// вспомогательная процедура для пакетной обработки
function alxexchange_batch_processing_finished($success, $results, $operations) {
	if ($success) {
		//drupal_set_message('Загружено ' . count($results) . ' товаров:' . theme('item_list', array('items' => $results)));
		drupal_set_message('Загружено ' . count($results) . ' элементов');
	} else {
		drupal_set_message('Завершено с ошибками.', 'error');
	}
}


// загрузка словарей - основной блок
function alxexchange_init_structload() {

	///////////////////////////////////////////////////////////
	// загрузка структуры каталогов
	///////////////////////////////////////////////////////////

	drupal_set_message('Структура каталога');

	$mas = alxexchange_load_groups_in_massiv();
	$vocabulary = taxonomy_vocabulary_machine_name_load('productgroups');

	$operations = array();
	foreach ($mas as $val) {
		$operations[] = array('alxexchange_process_structload', array($val, $mas, $vocabulary));
	}

	// dpm(count($operations));
	// dpm($operations[0]);
	// dpm('=======================');

	// массив для инициализации пакетной обработки
	$batch = array(
		'file' 				=> drupal_get_path('module', 'alxexchange') . '/module.struct.inc',
		'operations' 		=> $operations,
		'finished' 			=> 'alxexchange_batch_processing_finished',
		'title' 			=> 'Загрузка структуры каталога',
		'init_message' 		=> 'Подготовка данных',
		'progress_message' 	=> 'Выполнено @current из @total.',
		'error_message' 	=> 'Произошла ошибка.',
	);
 
	batch_set($batch);




	///////////////////////////////////////////////////////////
	// загрузка категорий товаров
	///////////////////////////////////////////////////////////

	drupal_set_message('Категории товаров');

	$mas = alxexchange_load_categories_in_massiv();
	$vocabulary = taxonomy_vocabulary_machine_name_load('category');

	$operations = array();
	foreach ($mas as $val) {
		$operations[] = array('alxexchange_process_structload', array($val, $mas, $vocabulary));
	}

	// dpm(count($operations));
	// dpm($operations[0]);
	// dpm('=======================');

	// массив для инициализации пакетной обработки
	$batch = array(
		'file' 				=> drupal_get_path('module', 'alxexchange') . '/module.struct.inc',
		'operations' 		=> $operations,
		'finished' 			=> 'alxexchange_batch_processing_finished',
		'title' 			=> 'Загрузка категорий товаров',
		'init_message' 		=> 'Подготовка данных',
		'progress_message' 	=> 'Выполнено @current из @total.',
		'error_message' 	=> 'Произошла ошибка.',
	);
 
	batch_set($batch);

}
