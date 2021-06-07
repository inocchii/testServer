<?php

ini_set('display_errors', 0);

/**
 * DBアクセスクラス
 * @author inok
 * @version 2021.06.06
 */

 require_once("clsBase.inc");

class CDbInfo extends CBase {
	//---------------------------------------------------------------
    // プロパティ
	//---------------------------------------------------------------
	var $CNAME             = "CDbInfo";
	//---------------------------------------------------------------
    // コンストラクタ
	//---------------------------------------------------------------
	function __construct($log) {
    	parent::__construct($log);
	}
	//---------------------------------------------------------------
    // 接続
	//---------------------------------------------------------------
	function makeCon($p1=INIT_DBHOST,$p2=INIT_DBPORT,$p3=INIT_DBNAME,$p4=INIT_DBUSER,$p5=INIT_DBPASSWORD,$p6=INIT_DBTYPE) {
		if ( $p6 == "MY" ) {
			mysqli_connect($p1,$p4,$p5,$p3,$p2);
		} else {
			return pg_connect("host=".$p1." port=".$p2." dbname=".$p3." user=".$p4." password=".$p5."");
		}
	}
	function makeCon4Test($p1=TEST_DBHOST,$p2=TEST_DBPORT,$p3=TEST_DBNAME,$p4=TEST_DBUSER,$p5=TEST_DBPASSWORD,$p6=TEST_DBTYPE) {
		if ( $p6 == "MY" ) {
			mysqli_connect($p1,$p4,$p5,$p3,$p2);
		} else {
			return pg_connect("host=".$p1." port=".$p2." dbname=".$p3." user=".$p4." password=".$p5."");
		}
	}
	function makeCon4Main($p1=MAIN_DBHOST,$p2=MAIN_DBPORT,$p3=MAIN_DBNAME,$p4=MAIN_DBUSER,$p5=MAIN_DBPASSWORD,$p6=MAIN_DBTYPE) {
		if ( $p6 == "MY" ) {
			mysqli_connect($p1,$p4,$p5,$p3,$p2);
		} else {
			return pg_connect("host=".$p1." port=".$p2." dbname=".$p3." user=".$p4." password=".$p5."");
		}
	}
}

?>