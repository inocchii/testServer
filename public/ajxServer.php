<?php
	//----------------------------------------------------------------------------------
	// ajxServer：Ajaxサーバ
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
	// ■ 2021.05.10 新規作成 inok
	//----------------------------------------------------------------------------------
    // 初期値
    $arrD =  explode("/", dirname(__FILE__));
	$base = $arrD[count($arrD) - 2];
	define("PNAME"   ,$base);
	define("PTITLE"  ,"Ajaxサーバ");

    // セッション
    session_start(); 

    // インクルード
    require_once(__DIR__."/../require.php");
    require_once(__DIR__."/../com_module/clsDbGetter.inc");
	
	// 初期処理
	init();

    // リクエスト取得
	$log->put(PNAME,"..receiving...");
    // POSTを取得
    //$request = json_decode(file_get_contents("php://input"), true);
    $request = $_REQUEST;
	$log->put(PNAME,"..receive request:".arr2set($request));

    // メイン処理
	$response = doMain($request);

    // 処理でエラー
    if ( $response === false ) {
        $log->putErr(PNAME,"request:".$request);
    }

    // レスポンス用JSONを生成
    $json = json_encode($response, JSON_UNESCAPED_UNICODE);

    // レスポンス送信
	$log->put(PNAME,"..sending...");
    header("Content-Type: application/json; charset=UTF-8");
    echo $json;

    // 終了
	$log->put(PNAME,"ended...");
	exit;

	//============================================
	// 初期処理
	//============================================
	function init() {
		global $MESSAGES,$log,$conn;
		//------------------------------------
		// ログ
		//------------------------------------
		$log = new CLog(PNAME);
		$log->setDebug(DEBUG_MODE);
		$log->put(PNAME,"starting...");
		//------------------------------------
		// 日付関連
		//------------------------------------
		define("TIMESTAMP",date("Y-m-d H:i:s"));
		define("TODAY",date('Ymd'));
		// メッセージ
		$MESSAGES = ""; 
        return true;
	}

	//============================================
	// メイン処理
	//============================================
	function doMain($argReq) {
		global $MESSAGES,$log,$conn;
        $log->put(PNAME,"..doMain starting...");

        // 基本項目を初期化
        $result = true;
        $message = "OK";
        $func   = "testFunc";
        $bufRes = [];
        $data   = [];

        // リクエストの基本チェック
		if( $argReq == null || $argReq == "" ) {
			$result = false;
			$message = "【エラー】リクエストが空です。";
			$log->putErr(PNAME,$message);
            // 処理結果判定を加える
            array_push( $bufRes,["result"   => $result] );
            array_push( $bufRes,["message"  => $message] );
			return $bufRes;
		}

        // function=xxx にて処理振り分け
        if ( $argReq["function"] ) {
            $func = $argReq['function']; 
        } else {
            $log->putErr(PNAME,"no func go test");
        }

        // 接続
        $dbInfo = new CDbInfo($log);
        $conn = $dbInfo->makeCon4Test();
        $log->put(PNAME,"..doMain connected dbname:".db_dbname($conn));

        // 各function
        try {
            $bufRes = $func($argReq);
        } catch (Exception $ex) {
            $log->putErr(PNAME,"invalid func:".$func." exception:".$ex->getMessage());
			$result = false;
			$message = "【エラー】未対応function:".$func;
            // 処理結果判定を加える
            array_push( $bufRes,["result"   => $result] );
            array_push( $bufRes,["message"  => $message] );
			return $bufRes;
        }

        // メッセージ
		$log->put(PNAME,"..doMain ended with func:".$func." result:".$bufRes["result"]." msg:".$bufRes["message"]);

        return $bufRes;
	}
	//============================================
	// function=checkLogin
	//============================================
	function checkLogin($argReq) {
		global $MESSAGES,$log,$conn;
        $log->put(PNAME,"....".__FUNCTION__." starting...");
        // 初期化
        $myBuf  = [];   $myData = [];
        $result = true;
        // チェック
        if ( $argReq['login_id'] == null || $argReq['login_id'] == "" ||
             $argReq['login_pw'] == null || $argReq['login_pw'] == "" ) {
			$result = false;
			$message = "【エラー】ログイン情報の入力がありません";
			$log->putErr(PNAME,$message);
            // 処理結果判定を加える
            array_push( $myBuf,["result"   => $result] );
            array_push( $myBuf,["message"  => $message] );
			return $myBuf;
        }
        // 引当
        $message = "正常にログインできました";
        // ユーザ情報編集
        $myBuf["user_nm"]  = "ドラゴン桜";
        $myBuf["pref"]  = "北海道";
        $myBuf["address"]  = "札幌市";
        $myBuf["result"]  = $result;
        $myBuf["message"]  = $message;

        return $myBuf;
    }
	//============================================
	// function=getFileVersionList
	//============================================
	function getFileVersionList($argReq) {
		global $MESSAGES,$log,$conn;
        $log->put(PNAME,"....".__FUNCTION__." starting...");
        // 初期化
        $myBuf  = [];   $myData = [];
        $result = true;
        $message = "テストデータ";
        //
        // ダミー編集
        //
        $ver = editDate(TODAY,"YYDMMDDD");
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
        array_push( $myBuf,["list"      => $myData] );
        // 処理結果判定を加える
        array_push( $myBuf,["result"    => $result] );
        array_push( $myBuf,["message"   => $message] );
        
        // メッセージ
		$MESSAGES = ""; 
        $log->put(PNAME,"....".__FUNCTION__." ended...");

        return $myBuf;

    }
	//============================================
	// function=getBunList
	//============================================
	function getBunList($argReq) {
		global $MESSAGES,$log,$conn;
        $log->put(PNAME,"....".__FUNCTION__." starting...");
        // 初期化
        $myBuf  = [];   $myData = [];
        $result = true;
        $message = "分類リスト";

        // Query
        pg_set_client_encoding($conn,"UNICODE");
        $sql  = "select 分類コード,分類区分,分類名,大分類コード,税区分,税率種別";
        $sql .=  " from 分類";
        $sql .= " where 分類区分='2'";
        $sql .= " order by 分類コード";
		// SQL実行
        $log->put(PNAME,"....".__FUNCTION__." sql:".$sql);
		$resOfQuery = db_exec ($conn, $sql);
		if (!$resOfQuery) {
            $log->putErr(PNAME,"....".__FUNCTION__." last_error:".db_last_error($conn));
            die("SQL実行エラー：SQL=".$sql);
        }
		// 表示件数
		$numOfQuery = db_numrows($resOfQuery);
        $log->put(PNAME,"....".__FUNCTION__." num:".$numOfQuery);
		//----------------------------------------------------
		// ここから表示用の詳細を取得
		//----------------------------------------------------
		for ($i=0; $i<$numOfQuery; $i++) {
			$myData[$i] = db_fetch_assoc($resOfQuery, $i);
        }
        $log->put(PNAME,"....".__FUNCTION__." count(myData)".count($myData));
        // list型の場合は{"list":レコード}とする
        array_push( $myBuf,["list"      => $myData] );
        // 処理結果判定を加える
        array_push( $myBuf,["result"    => $result] );
        array_push( $myBuf,["message"   => $message] );
        
        // メッセージ
		$MESSAGES = ""; 
        $log->put(PNAME,"....".__FUNCTION__." ended...");

        return $myBuf;

    }
	//============================================
	// function=getToriList
	//============================================
	function getToriList($argReq) {
		global $MESSAGES,$log,$conn;
        $log->put(PNAME,"....".__FUNCTION__." starting...");
        // レスポンスの初期化
        $myBuf  = []; $result = true; $message = "取引先リスト";

        // SQL
		$sql =<<<SQL
select 取引先コード, ＥＤＩ取引先コード, 支払先コード, 取引先名, 取引先名カナ, 取引先略称, 郵便番号, 住所, 住所２, 電話番号, ＦＡＸ番号, メールアドレス, ＥＯＳ区分, 伝票行数, 受注方式, 税区分
  from 取引先
 where 削除フラグ<>'1'
 order by 取引先コード
SQL;

        // DbGetter
        $dbGetter = new CDbGetter($log);
        if ( $rec = $dbGetter->getList($conn,$sql,__FUNCTION__) ) {
            // list型の場合は{"list":レコード}とする
            array_push( $myBuf,["list"      => $rec] );
            $log->put(PNAME,"....".__FUNCTION__." count(res)=".count($rec));
        } else {
            $result = false;
            $message = "取引先リスト取得に失敗しました (".$dbGetter->getMessage.")";
            $log->putErr(PNAME,"....".__FUNCTION__." err:".$message);
        }

        // 処理結果判定を加える
        array_push( $myBuf,["result"    => $result] );
        array_push( $myBuf,["message"   => $message] );
        
        $log->put(PNAME,"....".__FUNCTION__." ended...");

        return $myBuf;

    }
	//============================================
	// function=getItemList
	//============================================
	function getItemList($argReq) {
		global $MESSAGES,$log,$conn;
        $log->put(PNAME,"....".__FUNCTION__." starting...");
        // レスポンスの初期化
        $myBuf  = []; $result = true; $message = "商品リスト";

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
        $dbGetter = new CDbGetter($log);
        if ( $rec = $dbGetter->getList($conn,$sql,__FUNCTION__) ) {
            $log->put(PNAME,"....".__FUNCTION__." dbGetter count(rec)=".count($rec));
            // list型の場合は{"list":レコード}とする
            array_push( $myBuf,["list"      => $rec] );
            $log->put(PNAME,"....".__FUNCTION__." array_push completed");
        } else {
            $result = false;
            $message = "商品リスト取得に失敗しました (".$dbGetter->getMessage.")";
            $log->putErr(PNAME,"....".__FUNCTION__." err:".$message);
        }

        // 処理結果判定を加える
        array_push( $myBuf,["result"    => $result] );
        array_push( $myBuf,["message"   => $message] );
        
        $log->put(PNAME,"....".__FUNCTION__." ended...");

        return $myBuf;

    }
	//============================================
	// function=testFunc
	//============================================
	function testFunc($argReq) {
		global $MESSAGES,$log,$conn;
        $log->put(PNAME,"...".__FUNCTION__." starting...");
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
        $log->put(PNAME,"....".__FUNCTION__." ended...");

        return $myBuf;
	}
	//============================================
	// function=testError
	//============================================
	function testError($argReq) {
		global $MESSAGES,$log,$conn;
        $log->put(PNAME,"...".__FUNCTION__." starting...");
        // 単純にエラーとして返す
        return false;
	}
?>