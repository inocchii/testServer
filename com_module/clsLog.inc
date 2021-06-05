<?php
//===============================================================
// ログアクセスクラス
// 改修　2014.03.10 デバッグ判定
// 改修　2007.02.15 レポート専用出力の追加
// 　レポートファイル repFileNm
// 　レポート出力     rep(xxx)
// 初版　2005.06.23
// 　ログ出力関連の処理を行う。
//===============================================================

class CLog {
	//---------------------------------------------------------------
	// 定数
	//---------------------------------------------------------------
	var $CNAME            = "CLog";
	var $NORMAL           = "  ";
	var $ERROR            = "ER";
	var $CHECK            = "  ";
	var $WARNING          = "WA";
	var $DEBUG            = " *";
	var $NEXTLINE         = "\r\n"; // 改行コード
	var $SEPLINE          = "===============";
	//---------------------------------------------------------------
	// プロパティ
	//---------------------------------------------------------------
	var $filePath;			// ログファイルパス
	var $fileNm;			// ログファイル
	var $repFileNm;			// レポートファイル
	var $message;			// メッセージ
	var $debugMode = false;	// デバッグ
	var $pName;				// プログラム名
	var $cName;				// クラス名
	var $report;			// レポート
	//---------------------------------------------------------------
	// メソッド
	//---------------------------------------------------------------
	/* setDebug DEBUG設定 */
	function setDebug($args) { $this->debugMode=$args; }
	/* setFilePath パス設定 */
	function setFilePath($args) { $this->filePath=$args; }
	/* setFileNm ファイル名設定 */
	function setFileNm($args) { $this->fileNm=$args; }
	/* setRepFileNm レポートファイル名設定 */
	function setRepFileNm($args) { $this->repFileNm=$args; }
	/* setCName クラス名設定 */
	function setCName($args) { $this->cName=$args; }
	/* setMessage メッセージ設定 */
	function setMessage($args) { $this->message=$args; }
	/* setReport レポート設定 */
	function setReport($args) { $this->report=$args; }

	/* DEBUGモード取得 */
	function getDebug() { return $this->debugMode; }
	/* ファイル名取得 */
	function getFileNm() { return $this->fileNm; }
	/* レポートファイル名取得 */
	function getRepFileNm() { return $this->repFileNm; }
	/* プログラム名取得 */
	function getPName() { return $this->pName; }
	/* クラス名取得 */
	function getCName() { return $this->cName; }
	/* メッセージ取得 */
	function getMessage() { return $this->message; }
	/* レポート取得 */
	function getReport() { return $this->report; }

	//---------------------------------------------------------------
	// コンストラクタ
	//---------------------------------------------------------------
	//function CLog($argFileNm) {
	function __construct($argFileNm) {
		//global $LOG_PATH;
		//$this->filePath = $LOG_PATH;
		if ( defined('LOG_PATH') ){
			$this->filePath = LOG_PATH;
		} else {
			$this->filePath = COM_LOG_PATH;
		}
		$this->pName    = $argFileNm;	// 仮決め
		$this->fileNm   = $argFileNm.".log";
		$this->repFileNm = $argFileNm.".rep";
		//echo("PATH:".$this->filePath."/".$this->fileNm);
		if ( DEBUG_MODE != "" ) {
			$this->setDebug(DEBUG_MODE);
		}
	}
	//---------------------------------------------------------------
	// setPName プログラム名設定
	//---------------------------------------------------------------
	function setPName($args) {
		$this->pName=$args;
		$this->put($args,$this->SEPLINE);
		$this->put($args,"START..........");
	}
	//---------------------------------------------------------------
	// 出力振り分け
	//---------------------------------------------------------------
	/* NORMAL */
	function put($argCName,$argMessage) {
		$this->log($argCName,$argMessage,$this->NORMAL);
	}
	/* ERROR */
	function putErr($argCName,$argMessage) {
		$this->log($argCName,$argMessage,$this->ERROR);
		$this->report($argCName,$argMessage,$this->ERROR);
	}
	/* CHECK */
	function putChk($argCName,$argMessage) {
		$this->log($argCName,$argMessage,$this->CHECK);
		$this->report($argCName,$argMessage,$this->CHECK);
	}
	/* WARNING */
	function putWar($argCName,$argMessage) {
		$this->log($argCName,$argMessage,$this->WARNING);
		$this->report($argCName,$argMessage,$this->WARNING);
	}
	/* REPORT */
	function rep($argCName,$argMessage) {
		$this->log($argCName,$argMessage,$this->NORMAL);
		$this->report($argCName,$argMessage,$this->NORMAL);
	}
	/* DEBUG */
	function debug($argCName,$argMessage) {
		if ( $this->debugMode === true ) {
			$this->log($argCName,$argMessage,$this->DEBUG);
		}
	}
	//---------------------------------------------------------------
	// 出力部分
	// NORMAL->   2005-06-23 09:45:58+000|pName|cName|messages
	//---------------------------------------------------------------
	function log($argCName,$argMessage,$argPrefix="  ") {
		list($usec, $sec) = explode(" ", microtime());
		$usec = str_pad(number_format($usec*1000), 3, "0", STR_PAD_LEFT);
		$logLine  = $argPrefix." ".date("y-m-d H:i:s")."+".$usec;
		$logLine .= "|".$this->pName."|".$argCName."|".$argMessage;
		$logLine .= $this->NEXTLINE;
		$objFile  = fopen($this->filePath."/".$this->fileNm,"a");
		fwrite($objFile,$logLine);
		fclose($objFile);
	}
	//---------------------------------------------------------------
	// レポート部分
	// NORMAL->   2005-06-23 09:45:58+000|messages
	//---------------------------------------------------------------
	function report($argCName,$argMessage,$argPrefix) {
		list($usec, $sec) = explode(" ", microtime());
		$usec = str_pad(number_format($usec*1000), 3, "0", STR_PAD_LEFT);
		$logLine  = $argPrefix." ".date("y-m-d H:i:s")."+".$usec;
		$logLine .= "|".$argMessage;
		$logLine .= $this->NEXTLINE;
		$objFile  = fopen($this->filePath."/".$this->repFileNm,"a");
		fwrite($objFile,$logLine);
		fclose($objFile);
		$this->setReport($this->getReport().$argPrefix." ".$argMessage."\n");
	}

}
?>