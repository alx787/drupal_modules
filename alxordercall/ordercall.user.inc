<?php

function alxordercall_form($form_state, &$form_submit) {

  $form['name'] = array(
    '#title' => t('Ваше имя'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#maxlength' => 50,
    '#size' => 30,
    //'#value' => $username,
  );

  // $form['necessary_label'] = array(
  //   '#markup' => '<p>Заполните телефон или e-mail для обратной связи</p>',
  // );

  $form['phone'] = array(
    '#title' => t('Ваш телефон'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#maxlength' => 50,
    '#size' => 30,
    //'#value' => $userphone,
  );

  $form['timetocall'] = array(
    '#title' => t('Когда лучше Вам перезвонить'),
    '#type' => 'textfield',
    '#required' => FALSE,
    '#maxlength' => 50,
    '#size' => 50,
    '#default_value' => 'сейчас',
  );

  $form['message'] = array(
    '#title' => t('Ваш вопрос'),
    '#type' => 'textarea',
    '#required' => FALSE,
    '#rows' => 10,
    '#cols' => 60,
    '#resizable' => FALSE
  );


  $form['submit'] = array(
    '#type'=>'submit',
    '#value'=>t('Отправить'),
  );

  return $form;
}


function alxordercall_form_submit($form, &$form_state) {
    global $user;

    //Получаем даные преданные из формы и пишем в переменные для удосбства
    $name         = $form_state['values']['name'];
    $phone        = $form_state['values']['phone'];
    $timetocall   = $form_state['values']['timetocall'];
    $message      = $form_state['values']['message'];
    $data = date('Y.m.d H:i:s');

    // добавляем запись в базу
    $id = db_insert('ordercalls')
          ->fields(array(
              'data'        => $data,
              'name'        => $name,
              'phone'       => $phone,
              'timetocall'  => $timetocall,
              'message'     => $message,
              'userid'      => $user->uid,
              'answ'        => 0,
            ))->execute();


    //Формируем тело письма
    // $str  = "Имя ".$name."\r\n";
    // $str .= "Телефон ".$phone."\r\n";

    //Отправляем через функцию друпала
    // На мейл администратора сайта variable_get('site_mail')
    // drupal_mail('system', 'mail', variable_get('site_mail'), language_default(), array(
    //   'context' => array(
    //     'subject' => 'Заполнена форма обратного звонка',
    //     'message' => $str,
    //   )
    // ));

  drupal_set_message('Ваша заявка принята');
  //$form_state['redirect'] = '<front>';

  $form_state['redirect'] = 'callbackreceived';
}
