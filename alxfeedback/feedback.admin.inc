<?php

function feedback_messages() {

  $records = db_select('feedbackmessages', 'm')
          ->extend('PagerDefault')
          ->fields('m', array('id', 'data', 'name', 'phone', 'email', 'message'))
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
    array('data' => 'e-mail'),
    array('data' => 'Сообщение'),
    array('data' => 'Просмотр'),
  ); 


  foreach ($records as $rec) {
    $rows[] = array(
      array('data' => $rec->id),
      array('data' => $rec->data),
      array('data' => $rec->name),      
      array('data' => $rec->phone),      
      array('data' => $rec->email),      
      array('data' => substr($rec->message, 0, 20)),
      array('data' => l("просмотр", "admin/config/feedbackform/readmessage/" . $rec->id)),
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

  // return theme('table', array('header' => $header, 'rows'=> $rows));
  // return ''  ;
}


function show_message($message_id) {

  $output = '';

  $count = db_select('feedbackmessages', 'm')
          ->condition('m.id', $message_id)
          ->countQuery()
          ->execute()
          ->fetchField();


  if($count == 1) {

    $record = db_select('feedbackmessages', 'm')
            ->fields('m', array('id', 'data', 'name', 'phone', 'email', 'message'))
            ->condition('m.id', $message_id)
            ->execute()
            ->fetchObject();


    $output = '<p><span style="font-weight:bold">номер сообщения:</span> ' . $record->id . '</p>';
    $output .= '<p><span style="font-weight:bold">дата:</span> ' . $record->data . '</p>';
    $output .= '<p><span style="font-weight:bold">имя пользователя:</span> ' . $record->name . '</p>';
    $output .= '<p><span style="font-weight:bold">телефон:</span> ' . $record->phone . '</p>';
    $output .= '<p><span style="font-weight:bold">email:</span> ' . $record->email . '</p>';
    $output .= '<p><span style="font-weight:bold">сообщение:</span></p>';

    $output .= '<div style="width: 500px">' . $record->message . '</div>';
  } else {
    $output = 'Сообщение не найдено';
  }


  return $output;
}