<?php
	// изменение статуса звонка
	function change_callorder_status($call_status, $call_id) {

		if (settype($call_id, 'int')) {
			if ($call_status == '1') {
				db_query("UPDATE {ordercalls} SET answ = :status WHERE id = :id", array(':status' => $call_status, ':id' => $call_id));
			}
		}
		return true;
	}