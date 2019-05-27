<?php

	function callorder_list() {
		$records = db_select('ordercalls', 'm')
	          ->extend('PagerDefault')
	          ->fields('m', array('id', 'data', 'name', 'phone', 'message', 'timetocall', 'userid', 'answ'))
	          ->limit(10)
	          ->orderBy('id', 'ASC')
	          ->execute()
	          ->fetchAll();


		// создаем переменную, которая содержит массив  
		$header = array( 
			array('data' => 'id'),   
			array('data' => 'Дата'),   
			array('data' => 'Имя пользователя'),
			array('data' => 'Телефон'),
			array('data' => 'Сообщение'),
			array('data' => 'Когда перезвонить'),
			array('data' => 'Ид пользователя'),
			array('data' => 'Обработано'),
			array('data' => 'Просмотр'),
		); 

		//dpm($records);

		foreach ($records as $rec) {
			$rows[] = array(
				array('data' => $rec->id),
				array('data' => $rec->data),
				array('data' => $rec->name),      
				array('data' => $rec->phone),      
				array('data' => substr($rec->message, 0, 20)),      
				array('data' => $rec->timetocall),
				array('data' => $rec->userid),
				array('data' => ($rec->answ == 1 ? 'Да' : 'Нет')),
				array('data' => l("просмотр", "admin/config/callbackform/readorder/" . $rec->id)),
			);
		}

		// Create a render array ($build) which will be themed as a table with a
		// pager.
		$build['pager_table'] = array(
			'#theme' => 'table',
			'#header' => $header,
			'#rows' => $rows,
			'#empty' => t('There are no date formats found in the db'),
		);


		$build['pager_pager'] = array('#theme' => 'pager');

		return $build;
	}