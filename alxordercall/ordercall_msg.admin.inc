<?php

function show_callorder($message_id) {

  $output = '';

  $count = db_select('ordercalls', 'm')
          ->condition('m.id', $message_id)
          ->countQuery()
          ->execute()
          ->fetchField();


  if($count == 1) {

    $record = db_select('ordercalls', 'm')
            ->fields('m', array('id', 'data', 'name', 'phone', 'timetocall', 'message', 'answ'))
            ->condition('m.id', $message_id)
            ->execute()
            ->fetchObject();



  // dpm(base_path());
  // dpm(request_uri());
  // dpm(request_path());
  // dpm(current_path());
//http://' . $_SERVER['HTTP_HOST']

    $output = '<input name="call_id" type="hidden" value="' . $record->id . '"/>';
    $output .= '<input name="url" type="hidden" value="' . 'http://' . $_SERVER['HTTP_HOST'] . base_path() . '"/>';


    $output .= '<p><span style="font-weight:bold">номер заявки:</span> ' . $record->id . '</p>';
    $output .= '<p><span style="font-weight:bold">дата:</span> ' . $record->data . '</p>';
    $output .= '<p><span style="font-weight:bold">имя пользователя:</span> ' . $record->name . '</p>';
    $output .= '<p><span style="font-weight:bold">телефон:</span> ' . $record->phone . '</p>';
    $output .= '<p><span style="font-weight:bold">когда звонить:</span> ' . $record->timetocall . '</p>';
    $output .= '<p><span style="font-weight:bold">сообщение:</span></p>';

    $output .= '<div style="width: 500px">' . $record->message . '</div>';

    if ($record->answ == 0) {
    	drupal_add_library('system', 'ui.dialog');

    	$output .= '<p><span style="font-weight:bold; color: #FF0000;">Заявка не обработана</span></p>';

    	$output .= '<a id="callorder_onproc" href="#">Пометить как обработанную</a>';
    }
    


  } else {
    $output = 'Сообщение не найдено';
  }


  return $output;
}