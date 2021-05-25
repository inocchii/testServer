<?php
/*----------------------------------------------------------------------------------
 * 必要ファイルのインクルードを行う
 *----------------------------------------------------------------------------------
 * @author     H.Inokuchi
 * @package    testServer
 *----------------------------------------------------------------------------------*/

session_start(); //add 170706 suzuki

//定数読み込み
define("CONST_DIR",   "conf/");
require_once(CONST_DIR."const.inc");

//----------------------------------------------------------
// 共通インクルード設定
$arrInc = array(
    ///* セッションチェック */
    //COM_MOD_DIR . 'sess_chk.inc',
    /* 共通ファンクション */
    COM_MOD_DIR . 'fncGeneral.inc',
    /* dbアクセス */
    COM_MOD_DIR . 'db.inc',
    /* ログクラス */
    COM_MOD_DIR . 'clsLog.inc',
    /* 定数クラス */
    COM_MOD_DIR . 'clsConst.inc',
    /* DB操作 */
    COM_MOD_DIR . 'clsDbInfo.inc',
    /* JSON */
    COM_MOD_DIR . 'clsJson.inc',
    /* mail */
    COM_MOD_DIR . 'fncMail.inc',
);

// 共通インクルード読込
foreach ($arrInc as $incFile) {
    if (is_readable($incFile) && is_file($incFile)) {
        require_once($incFile);
    } else {
        die("必要なファイル（{$incFile}）が読み込めません");
    }
}

?>
