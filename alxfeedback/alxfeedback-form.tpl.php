<?php
//     dpm($form);
//     print_r($form);
	//dpm($form['message']);
?>


<div class="gruppirovka">
	<div>
		Если у Вас появились вопросы по работе нашего сайта, 
		если Вы нашли неточность в материалах или у Вас есть 
		предложение по улучшению обслуживания, Вы можете 
		отправить сообщение по любому вопросу. Пожалуйста, 
		заполните форму сообщения. Ваше сообщение будет 
		прочитано и учтено в дальнейшей работе
	</div>
</div>
<div class="gruppirovka">
	<div><h2>Как к Вам обращаться</h2></div>
	<?php print render($form['name']); ?>
</div>
<div class="gruppirovka">
	<div><h2>Заполните телефон или e-mail для обратной связи</h2></div>
	<?php print render($form['phone']); ?>
	<?php print render($form['email']); ?>
</div>
<div class="gruppirovka">
	<div><h2>Ваше сообщение</h2></div>

	<?php $form['message']['#title'] = ''; ?>
	<?php print render($form['message']); ?>
</div>

	<?php hide($form['submit']); ?>

	<?php
		if (isset($form['captcha'])) {
			print '<div class="gruppirovka">';
			print '<div><h2>Проверка</h2></div>';
			print render($form['captcha']);
			print '</div>';
		}

	?>

<div class="submitter">
	<?php print render($form['submit']); ?>
</div>
	<?php print drupal_render_children($form); ?>

