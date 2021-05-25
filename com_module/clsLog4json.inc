<?php

ini_set('display_errors', 0);

/**
 * ��前クラス
 * @author T.Suzuki
 * @version 2017.08.10
 */

class CLog {
	function __construct() {
	//       print "In BaseClass constructor\n";
	}
	function put($p,$p2) {
		$fp = fopen(LOG_DIR.PNAME.".log",'a');
		fwrite($fp, date("Y-m-d H:i:s\t")."[_]\t".$p."\t".$p2."\n");
		fclose($fp);
	}
	function putErr($p,$p2) {
//		echo "[".$p."]".$p2;
		$fp = fopen(LOG_DIR.PNAME.".log",'a');
		fwrite($fp, date("Y-m-d H:i:s\t")."[E]\t".$p."\t".$p2."\n");
		fclose($fp);
	}
	function setDebug($p) {
	}
}

?>
