<?php
//===============================================================
// CBaseServer：Ajax基本サーバクラス
// 　　クライアントからの要求を解析し各処理に回す。
// 　　処理後のレスポンスをJSON文字列にして戻す。
// ※ ログは呼び出し側で準備する
// ※ 各処理は呼び出し側に定義する
//---------------------------------------------------------------
// 修正履歴
// ■ 2021.06.05 新規作成 inok
//===============================================================
require_once("clsBase.inc");

class CBaseServer extends CBase {
	//---------------------------------------------------------------
    // プロパティ
	//---------------------------------------------------------------
	var $CNAME  = "CBaseServer";
	//---------------------------------------------------------------
    // コンストラクタ(ログは呼び出し側で準備する)
	//---------------------------------------------------------------
    function __construct($argName,&$log) {
        parent::__construct($log);
        $this->setPNAME($argName);
	}
	//============================================
	// 初期処理
	//============================================
	function doInit() {
        $this->log->put($this->PNAME,"..doInit starting...");
	}
	//============================================
	// メイン処理
	//============================================
	function doMain($argReq) {
       $this->log->put($this->PNAME,"..doMain starting...");

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
			$this->log->putErr($this->PNAME,$message);
            // 処理結果判定を加える
            array_push( $bufRes,["result"   => $result] );
            array_push( $bufRes,["message"  => $message] );
			return $bufRes;
		}

        // function=xxx にて機能名を取得
        if ( $argReq["function"] ) {
            $func = $argReq['function']; 
            $this->log->put($this->PNAME,"..doMain function=".$func);
        } else {
            $this->log->putErr($this->PNAME,"no func go test");
        }

        // 接続
        $dbInfo = new CDbInfo($log);
        $this->conn = $dbInfo->makeCon4Test();
        $this->log->put($this->PNAME,"..doMain connected dbname:".db_dbname($this->conn));

        //------------------------
        // 各function
        //  各機能は呼び出し側に定義
        //------------------------
        try {
            $bufRes = $this->$func($argReq);
        } catch (Exception $ex) {
            $this->log->putErr($this->PNAME,"invalid func:".$func." exception:".$ex->getMessage());
			$result = false;
			$message = "【エラー】未対応function:".$func;
            // 処理結果判定を加える
            array_push( $bufRes,["result"   => $result] );
            array_push( $bufRes,["message"  => $message] );
			return $bufRes;
        }

        // メッセージ
		$this->log->put($this->PNAME,"..doMain ended with func:".$func." result:".$bufRes["result"]." msg:".$bufRes["message"]);

        return $bufRes;
	}
    //============================================
	// 返信
	//============================================
	function doResponse($argResponse) {
        // レスポンス用JSONを生成
        $json = json_encode($argResponse, JSON_UNESCAPED_UNICODE);

        // ヘッダ送信
        $this->log->put($this->PNAME,"..sending... ver.2021.06.03.01");
        //header("Content-Type: text/plain; charset=UTF-8");
        header("Content-Type: application/json; charset=UTF-8");

        // CORS対応ヘッダ
        //header('Access-Control-Allow-Origin: *');
        //header('Access-Control-Allow-Origin: true');
        header('Access-Control-Allow-Origin: '.$_SERVER["HTTP_ORIGIN"]);
        //header('Access-Control-Allow-Headers: X-Requested-With, Origin, X-Csrftoken, Content-Type, Accept');

        // 本体
        echo $json;

        $this->log->put($this->PNAME,"..sent");
    }
	//============================================
	// 終了処理
	//============================================
	function doTerm() {
        $this->log->put($this->PNAME,"..doTerm");
	}
}