<?php
//----------------------------------------------------------------------------------
// himdyServer：himdy Ajaxサーバ
//----------------------------------------------------------------------------------
// 受付パラメータ
// 　必須：function=xxxxx　機能指定（例：function=checkLogin）
// 　他は機能毎
//----------------------------------------------------------------------------------
// 返信データ
// 　機能毎
//----------------------------------------------------------------------------------
// 機能群（function=xxxで指定）
//----------------------------------------------------------------------------------
// regUser：ユーザ登録
// checkLogin：ログインチェック
// getFileVersionList：ファイルバージョンリスト
// getTermConf：設定情報取得
// getBunList：分類リスト取得
// getToriList：取引先リスト取得
// testFunc：テスト用
// testError：エラーテスト用
//----------------------------------------------------------------------------------
// 修正履歴
// ■ 2022.01.07 新規作成 inok
//----------------------------------------------------------------------------------
// 初期値
define("PNAME"   ,"himdyServer");
define("PTITLE"  ,"himdyAjaxサーバ");
// インクルード
require_once(__DIR__."/../require.php");

class AjaxServer extends CBaseServer {
    // 返信パラメータ
    public $PARAM_NM_RESULT     = "result";
    public $PARAM_NM_MESSAGE    = "message";
    public $PARAM_NM_INFO       = "info";
    public $PARAM_NM_LIST       = "list";

    public function main() {

        session_start(); 

        $this->doInit();

        // リクエスト取得
        $this->log->put(PNAME,"..receiving...");
        $request = $_REQUEST;
        $this->log->put(PNAME,"..receive server:".arr2set($_SERVER));
        $this->log->put(PNAME,"..receive request:".arr2set($request));

        // JSONを取得
        $json = file_get_contents("php://input");
        $obj = json_decode($json,true);
        //$obj = json_decode(file_get_contents("php://input"), true);
        // 
        $this->log->put(PNAME,"..receive json:".arr2set($obj));

        // doMain
        $response = $this->doMain($request);
        // 処理でエラー
        if ( $response === false ) {
            $this->log->putErr(PNAME,"response error request:".$request);
        }

        // 返信
        $this->doResponse($response);

        $this->doTerm();

    }

    //============================================
	// handleError
    // 　共通処理：エラー結果をセットして戻す
	//============================================
    function handleError($argBuf, $argMsg ) {
        $this->log->putErr(PNAME,$argMsg);
        /*
        array_push($argBuf,[$this->PARAM_NM_RESULT  => false]);
        array_push($argBuf,[$this->PARAM_NM_MESSAGE => $argMsg]);
        */
        $argBuf[$this->PARAM_NM_RESULT] = false;
        $argBuf[$this->PARAM_NM_MESSAGE] = $argMsg;
        return $argBuf;
    }

    //============================================
	// getToken
    // 　共通処理：トークンを生成して戻す
	//============================================
    function getToken() {
        return uniqid('', true);
    }

	//============================================
	// function=checkSignup
    // 戻り値：info型
	//============================================
	function checkSignup($argReq) {
        $this->log->put(PNAME,"....".__FUNCTION__." starting...");

        // 初期化
        $myRec      = [];
        $myBuf      = [];

        // パラメータ基本チェック
        if ( $argReq['userid'] == null || $argReq['userid'] == "" ||
             $argReq['usernm'] == null || $argReq['usernm'] == "" ||
             $argReq['email']  == null || $argReq['email']  == "" ||
             $argReq['passwd'] == null || $argReq['passwd'] == "" ) {
            $message = "【エラー】未入力項目があります";
            // エラー判定
            return $this->handleError($myBuf, $message );
        }

        // DB接続
        $dbUtil = new CDbUtil($this->log);
        $dbh = $dbUtil->connect(MAIN_DSN, MAIN_DBUSER, MAIN_DBPASSWORD);
        //$dbh = new PDO(MAIN_DSN, MAIN_DBUSER, MAIN_DBPASSWORD);

        //-------------------------
        // 登録済みかのチェック
        //-------------------------
        // SQL
		$sql =<<<SQL
select userid,usernm,email
  from user
 where userid = :userid
SQL;
        // prepare
        $stmt = $dbh -> prepare($sql);
        $stmt -> bindValue("userid",$argReq['userid'],PDO::PARAM_STR);
        // 実行
        if ( ! $stmt -> execute() ) {
            $message = "【エラー】ユーザチェックに失敗しました (".arr2set($stmt->errorInfo()).")";
            // エラー判定
            return $this->handleError($myBuf, $message );
        }
        // 結果取得
        $myRec = $stmt->fetch(PDO::FETCH_ASSOC);
        // 結果判定
        if ( countAny($myRec) > 0 ) {
            $message = "【エラー】登録済みのアカウントです";
            // エラー判定
            return $this->handleError($myBuf, $message );
        }
        //-------------------------
        // 登録
        //-------------------------
        $token      = $this->getToken();    // トークン
        $lastLogin  = $this->timestamp;     // 前回ログイン

        // SQL
		$sql =<<<SQL
insert into user (
   userid , usernm , email , passwd , token , last_login
)values(
  :userid ,:usernm ,:email ,:passwd ,:token ,:last_login
)
SQL;
        // prepare
        $stmt = $dbh -> prepare($sql);
        $stmt -> bindValue("userid",$argReq['userid'],PDO::PARAM_STR);
        $stmt -> bindValue("usernm",$argReq['usernm'],PDO::PARAM_STR);
        $stmt -> bindValue("email", $argReq['email'],PDO::PARAM_STR);
        $stmt -> bindValue("passwd",md5($argReq['passwd']),PDO::PARAM_STR);
        $stmt -> bindValue("token", $token,PDO::PARAM_STR);
        $stmt -> bindValue("last_login",$lastLogin,PDO::PARAM_STR);
        // 実行
        if ( ! $stmt -> execute() ) {
            $message = "【エラー】ユーザ登録に失敗しました (".arr2set($stmt->errorInfo()).")";
            // エラー判定
            return $this->handleError($myBuf, $message );
        }
        // ログインまで完了とする
        $myRec["userid"]        = $argReq['userid'];
        $myRec["usernm"]        = $argReq['usernm'];
        $myRec["email"]         = $argReq['email'];
        $myRec["token"]         = $token;
        $myRec["last_login"]    = $lastLogin;

        // 処理結果
        $this->log->put(PNAME,"....".__FUNCTION__." rec=".arr2set($myRec));
        $myBuf[$this->PARAM_NM_INFO]    = $myRec;
        $myBuf[$this->PARAM_NM_RESULT]  = true;
        $myBuf[$this->PARAM_NM_MESSAGE] = "正常に登録できました";
        /*
        array_push( $myBuf,["info"      => $myRec] );
        array_push( $myBuf,["result"    => true] );
        array_push( $myBuf,["message"   => $message] );
        */

        return $myBuf;
    }
	//============================================
	// function=checkLogin
    // 戻り値：info型
	//============================================
	function checkLogin($argReq) {
        $this->log->put(PNAME,"....".__FUNCTION__." starting...");

        // 初期化
        $myRec      = [];
        $myBuf      = [];

        // パラメータ基本チェック
        if ( $argReq['userid'] == null || $argReq['userid'] == "" ||
             $argReq['passwd'] == null || $argReq['passwd'] == "" ) {
			$message = "【エラー】ログイン情報の入力がありません";
            // エラー判定
            return $this->handleError($myBuf, $message );
        }

        // DB接続
        $dbUtil = new CDbUtil($this->log);
        $dbh = $dbUtil->connect(MAIN_DSN, MAIN_DBUSER, MAIN_DBPASSWORD);
        //$dbh = new PDO(MAIN_DSN, MAIN_DBUSER, MAIN_DBPASSWORD);

        // SQL
		$sql =<<<SQL
select userid,usernm,email,token,last_login
  from user
 where userid = :userid
   and passwd = :passwd
SQL;

        // prepare
        $stmt = $dbh -> prepare($sql);
        $stmt -> bindValue("userid",$argReq['userid'],PDO::PARAM_STR);
        $stmt -> bindValue("passwd",md5($argReq['passwd']),PDO::PARAM_STR);

        // 実行
        if ( ! $stmt -> execute() ) {
            $message = "【エラー】ログインに失敗しました (".arr2set($stmt->errorInfo()).")";
            // エラー判定
            return $this->handleError($myBuf, $message );
        }

        // 結果取得
        $myRec = $stmt->fetch(PDO::FETCH_ASSOC);

        // 結果判定
        if ( countAny($myRec) <= 0 ) {
            $message = "【エラー】ログインできません";
            // エラー判定
            return $this->handleError($myBuf, $message );
        }

        // 処理結果
        $this->log->put(PNAME,"....".__FUNCTION__." rec=".arr2set($myRec));
        $myBuf[$this->PARAM_NM_INFO]    = $myRec;
        $myBuf[$this->PARAM_NM_RESULT]  = true;
        $myBuf[$this->PARAM_NM_MESSAGE] = "正常にログインできました";
        /*
        array_push( $myBuf,["info"      => $myRec] );
        array_push( $myBuf,["result"    => true] );
        array_push( $myBuf,["message"   => $message] );
        */

        return $myBuf;
    }
	//============================================
	// function=getFileVersionList
    // 戻り値：list型
	//============================================
	function getFileVersionList($argReq) {
        $this->log->put(PNAME,"....".__FUNCTION__." starting...");
        // 初期化
        $myBuf  = [];   $myData = [];
        $result = true;
        $message = "テストデータ";
        //
        // ダミー編集
        //
        $ver = editDate($this->today,"YYDMMDDD").".".date("His");
        // 端末別設定
        $record =[
            "file_id"       => "term_conf",
            "file_nm"       => "端末別設定",
            "server_file_version"  => $ver,
            "local_file_version"  => "",
        ];
        array_push($myData,$record);
        // 区分定義
        $record =[
            "file_id"       => "kbn_def_list",
            "file_nm"       => "区分定義",
            "server_file_version"  => $ver,
            "local_file_version"  => "",
        ];
        array_push($myData,$record);
        // 店舗(端末VIEW)
        $record =[
            "file_id"       => "tenpo_list",
            "file_nm"       => "店舗",
            "server_file_version"  => $ver,
            "local_file_version"  => "",
        ];
        array_push($myData,$record);
        // 分類(端末VIEW)
        $record =[
            "file_id"       => "bun_list",
            "file_nm"       => "分類",
            "server_file_version"  => $ver,
            "local_file_version"  => "",
        ];
        array_push($myData,$record);
        // 取引先(端末VIEW)
        $record =[
            "file_id"       => "tori_list",
            "file_nm"       => "取引先",
            "server_file_version"  => $ver,
            "local_file_version"  => "",
        ];
        array_push($myData,$record);
        // 店舗商品(端末VIEW)
        $record =[
            "file_id"       => "tenpo_item_list",
            "file_nm"       => "店舗商品",
            "server_file_version"  => $ver,
            "local_file_version"  => "",
        ];
        array_push($myData,$record);

        // list型の場合は{"list":レコード}とする
        // 処理結果
        // list型の場合は{"list":レコード}とする
        $this->log->put(PNAME,"....".__FUNCTION__." count(myData)".count($myData));
        $myBuf[$this->PARAM_NM_LIST]    = $myData;
        $myBuf[$this->PARAM_NM_RESULT]  = true;
        $myBuf[$this->PARAM_NM_MESSAGE] = $message;
        /*
        array_push( $myBuf,["list"      => $myData] );
        // 処理結果判定を加える
        array_push( $myBuf,["result"    => $result] );
        array_push( $myBuf,["message"   => $message] );
        */
        
        // メッセージ
		$MESSAGES = ""; 
        $this->log->put(PNAME,"....".__FUNCTION__." ended...");

        return $myBuf;

    }
	//============================================
	// function=getTermConf
    // 戻り値：list型
	//============================================
	function getTermConf($argReq) {
        $this->log->put(PNAME,"....".__FUNCTION__." starting...");
        // 初期化
        $myBuf  = [];   $myData = [];
        $result = true;
        $message = "テストデータ";

        // 設定情報をダミー編集
        $udt = date("Y.m.d.His");
        // (本来はテーブルから取得する)
        array_push($myData,["group"=>"system", "label"=>"端末番号", "id"=>"term_no", "val"=>"101", "type"=>"text", "level"=>"U","udt"=>$udt]);
        array_push($myData,["group"=>"system", "label"=>"端末ＩＤ", "id"=>"term_id", "val"=>"123456", "type"=>"text", "level"=>"A" ,"udt"=>$udt]);
        array_push($myData,["group"=>"system", "label"=>"利用区分", "id"=>"user_kbn", "val"=>"1", "type"=>"select", "level"=>"A" ,"udt"=>$udt]);
        array_push($myData,["group"=>"system", "label"=>"企業コード", "id"=>"kigyo_cd", "val"=>"920029", "type"=>"text", "level"=>"A" ,"udt"=>$udt]);
        array_push($myData,["group"=>"system", "label"=>"店舗コード", "id"=>"tenpo_cd", "val"=>"92002901", "type"=>"text", "level"=>"U" ,"udt"=>$udt]);
        array_push($myData,["group"=>"system", "label"=>"卸コード", "id"=>"oroshi_cd", "val"=>"", "type"=>"text", "level"=>"A" ,"udt"=>$udt]);
        array_push($myData,["group"=>"system", "label"=>"顧客内グループ", "id"=>"user_group", "val"=>"base", "type"=>"text", "level"=>"A" ,"udt"=>$udt]);
        array_push($myData,["group"=>"system", "label"=>"サーバＵＲＬ", "id"=>"server_url", "val"=>"", "type"=>"text", "level"=>"A" ,"udt"=>$udt]);
        /* term */
        array_push($myData,["group"=>"term", "label"=>"スキャン対象（JAN・UPC）", "id"=>"scan_ean_yn", "val"=>true, "type"=>"check", "level"=>"U" ,"udt"=>$udt]);
        array_push($myData,["group"=>"term", "label"=>"スキャン対象（ITF）", "id"=>"scan_itf_yn", "val"=>false, "type"=>"check", "level"=>"U" ,"udt"=>$udt]);
        array_push($myData,["group"=>"term", "label"=>"スキャン対象（code39）", "id"=>"scan_code39_yn", "val"=>false, "type"=>"check", "level"=>"U" ,"udt"=>$udt]);
        array_push($myData,["group"=>"term", "label"=>"スキャン対象（NW7）", "id"=>"scan_nw7_yn", "val"=>false, "type"=>"check", "level"=>"U" ,"udt"=>$udt]);
        array_push($myData,["group"=>"term", "label"=>"スキャン対象（code128）", "id"=>"scan_code128_yn", "val"=>false, "type"=>"check", "level"=>"U" ,"udt"=>$udt]);
        array_push($myData,["group"=>"term", "label"=>"スキャナバイブ", "id"=>"scanner_vibration", "val"=>true, "type"=>"check", "level"=>"U" ,"udt"=>$udt]);
        array_push($myData,["group"=>"term", "label"=>"スキャナサウンド", "id"=>"scanner_sound", "val"=>true, "type"=>"check", "level"=>"U" ,"udt"=>$udt]);
        /* user */
        array_push($myData,["group"=>"user", "label"=>"テストフラグ", "id"=>"test_yn", "val"=>true, "type"=>"check", "level"=>"U" ,"udt"=>$udt]);
        array_push($myData,["group"=>"user", "label"=>"発注ロット区分", "id"=>"lot_kbn", "val"=>true, "type"=>"check", "level"=>"A" ,"udt"=>$udt]);
        array_push($myData,["group"=>"user", "label"=>"当日納品可否", "id"=>"tojitsu_nohin_yn", "val"=>true, "type"=>"check", "level"=>"A" ,"udt"=>$udt]);
        array_push($myData,["group"=>"user", "label"=>"特売リードタイム", "id"=>"toku_lead_time", "val"=>5, "type"=>"number", "level"=>"A" ,"udt"=>$udt]);
        array_push($myData,["group"=>"user", "label"=>"特売単価設定区分", "id"=>"toku_tanka_set_kbn", "val"=>"1", "type"=>"select", "level"=>"A" ,"udt"=>$udt]);
        /* menu */
        array_push($myData,["group"=>"menu", "label"=>"発注", "id"=>"menu_order", "val"=>true, "type"=>"check", "level"=>"A" ,"udt"=>$udt]);
        array_push($myData,["group"=>"menu", "label"=>"履歴", "id"=>"menu_hist", "val"=>false, "type"=>"check", "level"=>"A" ,"udt"=>$udt]);
        array_push($myData,["group"=>"menu", "label"=>"棚卸", "id"=>"menu_tana", "val"=>true, "type"=>"check", "level"=>"U" ,"udt"=>$udt]);
        /* env */
        array_push($myData,["group"=>"env", "label"=>"開発モード", "id"=>"dev_mode", "val"=>true, "type"=>"check", "level"=>"U" ,"udt"=>$udt]);

        $this->log->put(PNAME,"....".__FUNCTION__." count(myData)".count($myData));
        // list型の場合は{"list":レコード}とする
        $myBuf[$this->PARAM_NM_LIST]    = $myData;
        // 処理結果判定を加える
        $myBuf[$this->PARAM_NM_RESULT]  = true;
        $myBuf[$this->PARAM_NM_MESSAGE] = $message;
        
        $this->log->put(PNAME,"....".__FUNCTION__." ended...");

        return $myBuf;

    }
	//============================================
	// function=postTermConf
    // 戻り値：boolean
	//============================================
	function postTermConf($argReq) {
        $this->log->put(PNAME,"....".__FUNCTION__." starting...");
        // 初期化
        $myBuf  = [];   $myData = [];
        $result = true;
        $message = "とりあえず受信だけしてみました";
        $cntJson = 0;
        $cntReg = 0;

        // 設定情報を取得
        $this->log->put(PNAME,"....".__FUNCTION__." starting...");
        // JSONを取得
        $json = file_get_contents("php://input");
        $obj = json_decode($json,true);
        //$obj = json_decode(file_get_contents("php://input"), true);
        $this->log->put(PNAME,"..receive json:".arr2set($obj));
        // レコードとして取り出す
        foreach ( $obj as $arr ) {
            $this->log->put(PNAME,"..receive json:".arr2set($arr));
            $cntJson++;
        }
        // テーブルに反映
        // 処理結果判定を加える
        $myBuf[$this->PARAM_NM_RESULT]  = true;
        $myBuf[$this->PARAM_NM_MESSAGE] = $message." 受信(".$cntJson.")件 登録(".$cntReg.")件";
        
        $this->log->put(PNAME,"....".__FUNCTION__." ended...");

        return $myBuf;

    }
	//============================================
	// function=getTenpoItemList
    // 戻り値：list型
	//============================================
	function getTenpoItemList($argReq) {
        $this->log->put(PNAME,"....".__FUNCTION__." starting...");
        // レスポンスの初期化
        $myBuf  = []; $result = true; $message = "商品リスト";

        // DB接続
        $objDbInfo = new CDbInfo($log);
        $this->conn = $objDbInfo->makeCon4Test();

/*
select i.商品コード||i.店舗コード||i.使用開始日 as 店舗商品キー
, i.商品コード, i.店舗コード
, i.使用開始日 as 適用開始日, '99999999' as 適用終了日
, i.品名漢字, i.品名カナ, i.メーカー名, i.商品呼称, i.商品規格名
, i.分類コード, i.取引先コード, i.原単価, i.売単価, i.入数, i.発注単位
, i.ＥＯＳ区分, i.削除フラグ, i.更新担当, i.更新時刻
  from 適用商品 i left join (
    select 商品コード,店舗コード,max(使用開始日) as 適用開始日
          from 適用商品
         where 適用商品.店舗コード='920029'
           and 使用開始日<'20220112'
         group by 商品コード,店舗コード
  ) im on i.商品コード=im.商品コード and i.店舗コード=im.店舗コード and i.使用開始日>=im.適用開始日
where i.店舗コード='920029'
and i.使用開始日>'20191231'
order by i.商品コード,i.使用開始日 desc
*/
        // SQL
		$sql =<<<SQL
select i.商品コード||i.店舗コード||i.使用開始日 as tenpo_item_key
, i.商品コード as item_cd, i.店舗コード as tenpo_cd
, i.使用開始日 as start_dt, '99999999' as end_dt
, i.品名漢字 as item_nm, i.品名カナ as item_nm_kana
, i.メーカー名 as item_maker_nm, i.商品呼称 as item_koshou, i.商品規格名 as item_kikaku_nm
, i.分類コード as bun_cd, i.取引先コード as tori_cd
, i.原単価 as genka, i.売単価 as baika, i.入数 as irisu, i.発注単位 as order_unit
, i.ＥＯＳ区分 as eos_kbn
, i.削除フラグ as del_flg, i.更新担当 as utanto, i.更新時刻 as udt
  from 適用商品 i left join (
    select 商品コード,店舗コード,max(使用開始日) as 適用開始日
          from 適用商品
         where 適用商品.店舗コード='920029'
           and 使用開始日<'20220112'
         group by 商品コード,店舗コード
  ) im on i.商品コード=im.商品コード and i.店舗コード=im.店舗コード and i.使用開始日>=im.適用開始日
where i.店舗コード='920029'
and i.使用開始日>'20191231'
order by i.商品コード,i.使用開始日 desc
SQL;

        // DbGetter
        $dbGetter = new CDbGetter($this->log);
        if ( $rec = $dbGetter->getList($this->conn,$sql,__FUNCTION__) ) {
            $this->log->put(PNAME,"....".__FUNCTION__." dbGetter count(rec)=".count($rec));
            // list型の場合は{"list":レコード}とする
            //array_push( $myBuf,["list"      => $rec] );
            $myBuf[$this->PARAM_NM_LIST]    = $rec;
            $this->log->put(PNAME,"....".__FUNCTION__." array_push completed");
        } else {
            $result = false;
            $message = "商品リスト取得に失敗しました (".$dbGetter->getMessage.")";
            $this->log->putErr(PNAME,"....".__FUNCTION__." err:".$message);
        }

        // 処理結果判定を加える
        $myBuf[$this->PARAM_NM_RESULT]  = $result;
        $myBuf[$this->PARAM_NM_MESSAGE] = $message;
        /*
        array_push( $myBuf,["result"    => $result] );
        array_push( $myBuf,["message"   => $message] );
        */
        
        $this->log->put(PNAME,"....".__FUNCTION__." ended...");

        return $myBuf;

    }
	//============================================
	// function=getKbnDefList
    // 戻り値：list型
	//============================================
	function getKbnDefList($argReq) {
        $this->log->put(PNAME,"....".__FUNCTION__." starting...");
        // レスポンスの初期化
        $myBuf  = []; $result = true; $message = "区分定義リスト";

        // DB接続
        $objDbInfo = new CDbInfo($log);
        $this->conn = $objDbInfo->makeCon4Test();

        // SQL
		$sql =<<<SQL
select 区分名, 区分値, 区分値名, 区分値名略称, 表示順
  from 区分定義
 where 削除フラグ<>'1'
 order by 区分名, 区分値
SQL;

        // DbGetter
        $dbGetter = new CDbGetter($this->log);
        if ( $rec = $dbGetter->getList($this->conn,$sql,__FUNCTION__) ) {
            // list型の場合は{"list":レコード}とする
            //array_push( $myBuf,["list"      => $rec] );
            $myBuf[$this->PARAM_NM_LIST]    = $rec;
            $this->log->put(PNAME,"....".__FUNCTION__." count(res)=".count($rec));
        } else {
            $result = false;
            $message = "区分定義リスト取得に失敗しました (".$dbGetter->getMessage.")";
            $this->log->putErr(PNAME,"....".__FUNCTION__." err:".$message);
        }

        // 処理結果判定を加える
        $myBuf[$this->PARAM_NM_RESULT]  = $result;
        $myBuf[$this->PARAM_NM_MESSAGE] = $message;
        
        $this->log->put(PNAME,"....".__FUNCTION__." ended...");

        return $myBuf;

    }
	//============================================
	// function=getTenpoList
    // 戻り値：list型
	//============================================
	function getTenpoList($argReq) {
        $this->log->put(PNAME,"....".__FUNCTION__." starting...");
        // レスポンスの初期化
        $myBuf  = []; $result = true; $message = "店舗リスト";

        // DB接続
        $objDbInfo = new CDbInfo($log);
        $this->conn = $objDbInfo->makeCon4Test();

        // SQL
		$sql =<<<SQL
select 店舗コード, ＥＤＩ店舗コード, 会社コード
, 店舗名, 店舗名カナ, 店舗略称, 会社名, 会社略称
, 郵便番号, 住所, 住所２, 電話番号, ＦＡＸ番号, メールアドレス
  from 店舗
 where 削除フラグ<>'1'
 order by 店舗コード
SQL;

        // DbGetter
        $dbGetter = new CDbGetter($this->log);
        if ( $rec = $dbGetter->getList($this->conn,$sql,__FUNCTION__) ) {
            // list型の場合は{"list":レコード}とする
            //array_push( $myBuf,["list"      => $rec] );
            $myBuf[$this->PARAM_NM_LIST]    = $rec;
            $this->log->put(PNAME,"....".__FUNCTION__." count(res)=".count($rec));
        } else {
            $result = false;
            $message = "店舗リスト取得に失敗しました (".$dbGetter->getMessage.")";
            $this->log->putErr(PNAME,"....".__FUNCTION__." err:".$message);
        }

        // 処理結果判定を加える
        $myBuf[$this->PARAM_NM_RESULT]  = $result;
        $myBuf[$this->PARAM_NM_MESSAGE] = $message;
        
        $this->log->put(PNAME,"....".__FUNCTION__." ended...");

        return $myBuf;

    }
	//============================================
	// function=getBunList
    // 戻り値：list型
	//============================================
	function getBunList($argReq) {
        $this->log->put(PNAME,"....".__FUNCTION__." starting...");
        // レスポンスの初期化
        $myBuf  = []; $result = true; $message = "分類リスト";

        // DB接続
        $objDbInfo = new CDbInfo($log);
        $this->conn = $objDbInfo->makeCon4Test();

        // SQL
		$sql =<<<SQL
select 分類コード, 分類区分, 分類名, 大分類コード, 中分類コード
, 税区分, ＰＯＳ税区分, 税端数, 税率種別
, 棚卸入力価格区分
  from 分類
 where 分類区分='2' and 削除フラグ<>'1'
 order by 分類コード
SQL;

        // DbGetter
        $dbGetter = new CDbGetter($this->log);
        if ( $rec = $dbGetter->getList($this->conn,$sql,__FUNCTION__) ) {
            // list型の場合は{"list":レコード}とする
            //array_push( $myBuf,["list"      => $rec] );
            $myBuf[$this->PARAM_NM_LIST]    = $rec;
            $this->log->put(PNAME,"....".__FUNCTION__." count(res)=".count($rec));
        } else {
            $result = false;
            $message = "分類リスト取得に失敗しました (".$dbGetter->getMessage.")";
            $this->log->putErr(PNAME,"....".__FUNCTION__." err:".$message);
        }

        // 処理結果判定を加える
        $myBuf[$this->PARAM_NM_RESULT]  = $result;
        $myBuf[$this->PARAM_NM_MESSAGE] = $message;
        
        $this->log->put(PNAME,"....".__FUNCTION__." ended...");

        return $myBuf;

    }
	//============================================
	// function=getBunList
    // 戻り値：list型
	//============================================
	function getBunListOld($argReq) {
        $this->log->put(PNAME,"....".__FUNCTION__." starting...");
        // 初期化
        $myBuf  = [];   $myData = [];
        $result = true;
        $message = "分類リスト";

        // DB接続
        $objDbInfo = new CDbInfo($log);
        $this->conn = $objDbInfo->makeCon4Test();

        // Query
        db_set_client_encoding($this->conn,"UNICODE");
        $sql  = "select 分類コード,分類区分,分類名,大分類コード,税区分,税率種別";
        $sql .=  " from 分類";
        $sql .= " where 分類区分='2'";
        $sql .= " order by 分類コード";
		// SQL実行
        $this->log->put(PNAME,"....".__FUNCTION__." sql:".$sql);
		$resOfQuery = db_exec ($this->conn, $sql);
		if (!$resOfQuery) {
            $this->log->putErr(PNAME,"....".__FUNCTION__." last_error:".db_last_error($conn));
            die("SQL実行エラー：SQL=".$sql);
        }
		// 表示件数
		$numOfQuery = db_numrows($resOfQuery);
        $this->log->put(PNAME,"....".__FUNCTION__." num:".$numOfQuery);
		//----------------------------------------------------
		// ここから表示用の詳細を取得
		//----------------------------------------------------
		for ($i=0; $i<$numOfQuery; $i++) {
			$myData[$i] = db_fetch_assoc($resOfQuery, $i);
        }
        /*
        array_push( $myBuf,["list"      => $myData] );
        // 処理結果判定を加える
        array_push( $myBuf,["result"    => $result] );
        array_push( $myBuf,["message"   => $message] );
        */

        // 処理結果
        // list型の場合は{"list":レコード}とする
        $this->log->put(PNAME,"....".__FUNCTION__." count(myData)".count($myData));
        $myBuf[$this->PARAM_NM_LIST]    = $myData;
        $myBuf[$this->PARAM_NM_RESULT]  = true;
        $myBuf[$this->PARAM_NM_MESSAGE] = "分類リスト";

        $this->log->put(PNAME,"....".__FUNCTION__." ended...");

        return $myBuf;

    }
	//============================================
	// function=getToriList
    // 戻り値：list型
	//============================================
	function getToriList($argReq) {
        $this->log->put(PNAME,"....".__FUNCTION__." starting...");
        // レスポンスの初期化
        $myBuf  = []; $result = true; $message = "取引先リスト";

        // DB接続
        $objDbInfo = new CDbInfo($log);
        $this->conn = $objDbInfo->makeCon4Test();

        // SQL
		$sql =<<<SQL
select 取引先コード, ＥＤＩ取引先コード, 支払先コード, 取引先名, 取引先名カナ, 取引先略称
, 郵便番号, 住所, 住所２, 電話番号, ＦＡＸ番号, メールアドレス
, ＥＯＳ区分, 伝票行数, 受注方式, 税区分
  from 取引先
 where 削除フラグ<>'1'
 order by 取引先コード
SQL;

        // DbGetter
        $dbGetter = new CDbGetter($this->log);
        if ( $rec = $dbGetter->getList($this->conn,$sql,__FUNCTION__) ) {
            // list型の場合は{"list":レコード}とする
            //array_push( $myBuf,["list"      => $rec] );
            $myBuf[$this->PARAM_NM_LIST]    = $rec;
            $this->log->put(PNAME,"....".__FUNCTION__." count(res)=".count($rec));
        } else {
            $result = false;
            $message = "取引先リスト取得に失敗しました (".$dbGetter->getMessage.")";
            $this->log->putErr(PNAME,"....".__FUNCTION__." err:".$message);
        }

        // 処理結果判定を加える
        $myBuf[$this->PARAM_NM_RESULT]  = $result;
        $myBuf[$this->PARAM_NM_MESSAGE] = $message;
        
        $this->log->put(PNAME,"....".__FUNCTION__." ended...");

        return $myBuf;

    }
	//============================================
	// function=getItemList
    // 戻り値：list型
	//============================================
	function getItemList($argReq) {
        $this->log->put(PNAME,"....".__FUNCTION__." starting...");
        // レスポンスの初期化
        $myBuf  = []; $result = true; $message = "商品リスト";

        // DB接続
        $objDbInfo = new CDbInfo($log);
        $this->conn = $objDbInfo->makeCon4Test();

        // SQL
		$sql =<<<SQL
select 商品コード, 使用開始日, 品名漢字
, 分類コード, 取引先コード, 原単価, 売単価, 入数, ボール入数, 発注単位
, 最新納品日, 最新売上日, 廃番日
  from 適用商品
 where (商品コード,店舗コード,使用開始日)
    in (select 商品コード,店舗コード,max(使用開始日)
          from 適用商品
         where 店舗コード='920029'
           and 使用開始日<'20210521'
         group by 商品コード,店舗コード)
   and 削除フラグ<>'1'
order by 商品コード
limit 20000
SQL;

        // DbGetter
        $dbGetter = new CDbGetter($this->log);
        if ( $rec = $dbGetter->getList($this->conn,$sql,__FUNCTION__) ) {
            $this->log->put(PNAME,"....".__FUNCTION__." dbGetter count(rec)=".count($rec));
            // list型の場合は{"list":レコード}とする
            //array_push( $myBuf,["list"      => $rec] );
            $myBuf[$this->PARAM_NM_LIST]    = $rec;
            $this->log->put(PNAME,"....".__FUNCTION__." array_push completed");
        } else {
            $result = false;
            $message = "商品リスト取得に失敗しました (".$dbGetter->getMessage.")";
            $this->log->putErr(PNAME,"....".__FUNCTION__." err:".$message);
        }

        // 処理結果判定を加える
        $myBuf[$this->PARAM_NM_RESULT]  = $result;
        $myBuf[$this->PARAM_NM_MESSAGE] = $message;
        /*
        array_push( $myBuf,["result"    => $result] );
        array_push( $myBuf,["message"   => $message] );
        */
        
        $this->log->put(PNAME,"....".__FUNCTION__." ended...");

        return $myBuf;

    }
	//============================================
	// function=testFunc
	//============================================
	function testFunc($argReq) {
        $this->log->put(PNAME,"...".__FUNCTION__." starting...");
        // 初期化
        $myBuf  = [];   $myData = [];

        $value = 0; $price = "不明"; $result = true; $message = "OK";
        switch ($argReq['code']) {
            case "1":
                $value  = 100;
                $price  = "百円";
                break;
            case "2":
                $value = 200;
                $price  = "二百円";
                break;
            case "3":
                $value = 300;
                $price  = "三百円";
                break;
        }
        $record =[
            "value"     => $value,
            "value2"    => $value*2,
            "価格"      => $price,
        ];
        // dataにrecordを追加
        array_push($myData,$record);
        array_push($myData,$record);

        array_push( $myBuf,["data"     => $myData] );
        // 処理結果判定を加える
        array_push( $myBuf,["result"   => $result] );
        array_push( $myBuf,["message"  => $message] );
        
        // メッセージ
		$MESSAGES = ""; 
        $this->log->put(PNAME,"....".__FUNCTION__." ended...");

        return $myBuf;
	}
	//============================================
	// function=testError
	//============================================
	function testError($argReq) {
        $this->log->put(PNAME,"...".__FUNCTION__." starting...");
        // 単純にエラーとして返す
        return false;
	}
}

//------------------------------------
// ログ
//------------------------------------
$log = new CLog(PNAME);
$log->setDebug(DEBUG_MODE);
$log->put(PNAME,"starting...");

//------------------------------------
// 実行
//------------------------------------
$ajaxServer = new AjaxServer(PNAME,$log);
$ajaxServer->main();