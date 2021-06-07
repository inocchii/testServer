<?php
//----------------------------------------------------------------------------------
// AjaxServer：Ajaxサーバ
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
// getBunList：分類リスト取得
// getToriList：取引先リスト取得
// testFunc：テスト用
// testError：エラーテスト用
//----------------------------------------------------------------------------------
// 修正履歴
// ■ 2021.06.05 新規作成 inok
//----------------------------------------------------------------------------------
// 初期値
define("PNAME"   ,"AjaxServer");
define("PTITLE"  ,"Ajaxサーバ");
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
        // POSTを取得
        //$request = json_decode(file_get_contents("php://input"), true);
        $request = $_REQUEST;
        $this->log->put(PNAME,"..receive server:".arr2set($_SERVER));
        $this->log->put(PNAME,"..receive request:".arr2set($request));

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
        $ver = editDate($this->today,"YYDMMDDD");
        $record =[
            "file_id"       => "bun_list",
            "file_nm"       => "分類リスト",
            "file_version"  => $ver.".00",
        ];
        array_push($myData,$record);
        // ダミー編集
        $record =[
            "file_id"       => "tenpo_list",
            "file_nm"       => "店舗リスト",
            "file_version"  => $ver.".00",
        ];
        array_push($myData,$record);
        // ダミー編集
        $record =[
            "file_id"       => "catg_list",
            "file_nm"       => "カテゴリリスト",
            "file_version"  => $ver.".00",
        ];
        array_push($myData,$record);
        // ダミー編集
        $record =[
            "file_id"       => "tori_list",
            "file_nm"       => "取引先リスト",
            "file_version"  => $ver.".00",
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
	// function=getBunList
    // 戻り値：list型
	//============================================
	function getBunList($argReq) {
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
select 取引先コード, ＥＤＩ取引先コード, 支払先コード, 取引先名, 取引先名カナ, 取引先略称, 郵便番号, 住所, 住所２, 電話番号, ＦＡＸ番号, メールアドレス, ＥＯＳ区分, 伝票行数, 受注方式, 税区分
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
        /*
        array_push( $myBuf,["result"    => $result] );
        array_push( $myBuf,["message"   => $message] );
        */
        
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