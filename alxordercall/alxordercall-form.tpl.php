<?php 
	dpm($form);
?>


<div class="gruppirovka">
	<div>
		Вы можете оставить заявку на телефонный звонок, наш менеджер перезвонит Вам в указанное время
	</div>
</div>

<div class="gruppirovka">

	<?php //print render($form['message']); ?>
	<?php print render($form['name']); ?>
	<?php print render($form['phone']); ?>


	<?php print render($form['timetocall']); ?>
	<?php print render($form['message']); ?>
</div>

<div class="submitter">
	<?php print render($form['submit']); ?>
</div>
	<?php print drupal_render_children($form); ?>
