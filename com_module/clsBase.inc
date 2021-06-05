<?php
/****************************************
 * 基本CLASS
 * @author H.Inokuchi
 * @version 1.0
 ****************************************/

class CBase{
    //--------------------------
    var $CNAME = "CBase";
    var $PNAME = "";
    //--------------------------
    // DB
    //--------------------------
    var $con;            // DB接続
    //--------------------------
    // プロパティ
    //--------------------------
    var $log;            // ログ
    var $canLog = false; // ログ可否
    var $dtl;            // 明細格納配列
    var $today;          // 処理日
	var $timestamp;	     // タイムスタンプ
	var $tanto;	         // 担当者
    var $message;        // メッセージ
    var $test = false;   // テスト
	// カウント
	var $cntTotal = 0;   // 総件数
	var $cntSkip  = 0;   // スキップ件数
	var $cntUpd   = 0;   // 更新件数
	var $cntNew   = 0;   // 新規件数
	var $cntErr   = 0;   // エラー件数
	var $cntDel   = 0;   // 削除件数
    //--------------------------
    // コンストラクタ
    //--------------------------
    //function CBase(&$log) {
    function __construct(&$log) {
        $this->setLog($log);
		$this->today = date("Ymd");
		$this->timestamp = date("YmdHis");
	}
    /** プログラム名 **/
    function setPNAME($args) { $this->PNAME = $args; }
    function getPNAME() { return $this->PNAME; }
	/** コネクション設定 */
	function setCon(&$args) { $this->con=$args; }
    /** ログ設定 */
    function setLog(&$log) { $this->log = $log; $this->canLog = true; }
    /** ログ出力 */
    function putLog($args) { if ( $this->canLog ) { $this->log->put($this->CNAME,$args); } }
    /** ログ出力 */
    function log($args)    { if ( $this->canLog ) { $this->log->put($this->CNAME,$args); } }
    /** デバッグ */
    function debug($args)  { if ( $this->canLog ) { $this->log->debug($this->CNAME,$args); } }
    /** エラー */
    function putErr($args) { if ( $this->canLog ) { $this->log->putErr($this->CNAME,$args); } }
    /** ワーニング */
    function putWar($args) { if ( $this->canLog ) { $this->log->putWar($this->CNAME,$args); } }
    /** テスト */
    function test($args=true) { $this->test = $args; }
    function isTest() { return $this->test; }
	/* タイムスタンプ設定 */
	function setTimestamp($args) { $this->timestamp=$args; }
	function getTimestamp() { return $this->timestamp; }
    /** 担当者 **/
    function setTanto($args) { $this->tanto = $args; }
    function getTanto() { return $this->tanto; }
    /** メッセージ **/
    function setMessage($args) { $this->message = $args; }
    function getMessage() { return $this->message; }
    /** 設定配列 **/
    function clear() {
		$this->dtl     = "";
		$this->message = "";
	}
    function addDtl($args) {
		if ( !is_array($args) ) { return false; }
		if ( !is_array($this->dtl) ) {
			$this->dtl = $args;
		} else {
			$wArray = array_merge($this->dtl,$args);
			$this->dtl = $wArray;
		}
	}
    function setDtl(&$args) { $this->dtl  = $args; }
    function getDtl() { return $this->dtl; }
    /** 設定値取得 **/
    function set($argKey,$argValue) { $this->dtl[$argKey] = $argValue; }
    function get($argKey) { return $this->dtl[$argKey]; }
    /** 設定値リストの取得（ログ用） **/
    function listDtl($argArr) {
		foreach ( $argArr as $val ) {
			$list .= " ".$val.":".$this->get($val);
		}
		return $list;
	}
	/** カウント設定 */
	function setCntTotal($args) { $this->cntTotal=$args; }
	function setCntSkip($args)  { $this->cntSkip=$args; }
	function setCntUpd($args)   { $this->cntUpd=$args; }
	function setCntNew($args)   { $this->cntNew=$args; }
	function setCntErr($args)   { $this->cntErr=$args; }
	function setCntDel($args)   { $this->cntDel=$args; }
	function getCntTotal() { return $this->cntTotal; }
	function getCntSkip()  { return $this->cntSkip; }
	function getCntUpd()   { return $this->cntUpd; }
	function getCntNew()   { return $this->cntNew; }
	function getCntErr()   { return $this->cntErr; }
	function getCntDel()   { return $this->cntDel; }
}
?>
