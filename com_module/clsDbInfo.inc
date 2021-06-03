<?php

ini_set('display_errors', 0);

/**
 * DBアクセスクラス
 * @author T.Suzuki
 * @version 2017.08.10
 */


class CDbInfo {
	function __construct($log) {
	//       print "In BaseClass constructor\n";
	}
	function makeCon($p,$p2) {
//dev		return pg_connect("host=172.16.3.32 port=5432 dbname=950262 user=kenpin password=kenpin");
		return pg_connect("host=".INIT_DBHOST." port=".INIT_DBPORT." dbname=".INIT_DBNAME." user=".INIT_DBUSER." password=".INIT_DBPASSWORD."");
	}
	function makeCon4Test() {
		return pg_connect("host=".TEST_DBHOST." port=".TEST_DBPORT." dbname=".TEST_DBNAME." user=".TEST_DBUSER." password=".TEST_DBPASSWORD."");
	}
}

?>