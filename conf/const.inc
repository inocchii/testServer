<?php
/*----------------------------------------------------------------------------------
 * 共通定数定義
 *----------------------------------------------------------------------------------
 * @author     H.Inokuchi
 * @package    testServer
 *----------------------------------------------------------------------------------
 * ■2021.05.09 新規作成
 *----------------------------------------------------------------------------------*/

 	define("DEBUG_MODE"				, false);	// デバッグモード(通常FALSE)

	//-----------------------------------------------------------
	// インクルードパス設定
	//-----------------------------------------------------------
	//define("BASE_DIR"				,"/Users/inokuchi.hiroyuki/home/monaca/testServer/"); // ベース(内部)
	//define("COM_LIB_DIR"			,BASE_DIR."com_lib/");		// ライブラリ
	define("COM_MOD_DIR"			,BASE_DIR."com_module/");	// モジュール
	define("COM_INC_DIR"			,BASE_DIR."conf/");			// インクルード
	define("COM_LOG_PATH"			,BASE_DIR."logs/");			// ログ
	define("COM_DATA_DIR"			,BASE_DIR."data/");			// データ

	//-----------------------------------------------------------
	// DB関連
	//-----------------------------------------------------------
	// DB情報：初期
	define("INIT_DSN"            	,"pgsql:dbname=app_db;host=172.16.3.28;charset=EUC-JP");

	define("INIT_DBHOST"            ,"172.16.3.28");
	define("INIT_DBPORT"            ,"5432");
	define("INIT_DBNAME"            ,"app_db");
	define("INIT_DBUSER"            ,"kenpin");
	define("INIT_DBPASSWORD"        ,"kenpin");
	define("INIT_DBTYPE"            ,"PG");

	// DB情報：初期テスト
	define("INIT_TEST_DSN"          ,"pgsql:dbname=app_db;host=172.16.3.28;charset=EUC-JP");
 
	define("INIT_DBHOST_TEST"       ,"172.16.3.28");
	define("INIT_DBNAME_TEST"       ,"app_db");

	// DB情報：テスト用
	define("TEST_DSN"            	,"pgsql:dbname=920029;host=172.16.3.28;charset=EUC-JP");

	define("TEST_DBHOST"            ,"172.16.3.28");
	define("TEST_DBPORT"            ,"5432");
	define("TEST_DBNAME"            ,"920029");
	define("TEST_DBUSER"            ,"kenpin");
	define("TEST_DBPASSWORD"        ,"kenpin");
	define("TEST_DBTYPE"            ,"PG");

	// DB情報：テスト用
	define("MAIN_DSN"            	,"mysql:dbname=testServer;host=localhost;charset=utf8");

	define("MAIN_DBHOST"            ,"localhost");
	define("MAIN_DBPORT"            ,"3306");
	define("MAIN_DBNAME"            ,"testServer");
	define("MAIN_DBUSER"            ,"test");
	define("MAIN_DBPASSWORD"        ,"test");
	define("MAIN_DBTYPE"            ,"MY");

?>