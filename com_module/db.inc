<?php
//----------------------------------------------------------------------------------
// ＤＢアクセス共通関数群 db.inc
//   (postgreSQL)
// 2005/03/19 inok
//----------------------------------------------------------------------------------
	// OPEN
	function db_open($argHost,$argUser,$argPwd,$argDB,$argPort="5432") {
		$connection = pg_connect("host=".$argHost." port=".$argPort." dbname=".$argDB." user=".$argUser." password=".$argPwd);
		//$connection = @mysql_connect($argHost, $argUser, $argPwd);
		//mysql_select_db($argDB);
		return $connection;
	}
	// CLOSE
	function db_close(&$argCon) {
		pg_close($argCon);
		//@mysql_close();
	}
	// DBNAME
	function db_dbname(&$argCon) {
		return pg_dbname($argCon);
	}
	// DBHOST
	function db_host(&$argCon) {
		return pg_host($argCon);
	}
	// エラーメッセージ
	function db_last_error(&$argCon) {
		return pg_last_error($argCon);
	}
	//-----------------------------------------------------------------
	// SQL(Prepare) -----------------
	// 2014.05.31 既存チェック機能を追加（by inok）
	//-----------------------------------------------------------------
	function db_prepare(&$argCon,$argStmt,$argQuery,$argLog="",$argCheckExist=false){
		$argStmt = strtolower($argStmt);
		if ( $argLog != "" ) { $argLog->debug("db_prepare","STMT=".$argStmt." QUERY=".$argQuery); }
		// 2014.05.31 既存チェック機能を追加（by inok）
		if ( $argCheckExist ) {
			$sql = "SELECT name FROM pg_prepared_statements WHERE name = '".$argStmt."'";
			$res = pg_query($argCon,$sql);
			if ( $res ) {
				if ( pg_num_rows($res) != 0 ) {
					if ( $argLog != "" ) { $argLog->debug("db_prepare","already exists! STMT=".$argStmt." QUERY=".$argQuery); }
					$sql = "deallocate ".$argStmt;
					$res = pg_query($argCon,$sql);
					if ( $res ) {
						if ( $argLog != "" ) { $argLog->debug("db_prepare","deallocate! STMT=".$argStmt); }
					} else {
						$errmsg = pg_last_error($argCon);
						if ( $argLog != "" ) { $argLog->putErr("db_prepare","ERR:[".$errmsg."] SQL:" . $sql); }
						return false;
					}
				}
			} else {
				$errmsg = pg_last_error($argCon);
				if ( $argLog != "" ) { $argLog->putErr("db_prepare","ERR:[".$errmsg."] SQL:" . $sql); }
				return false;
			}
		}
		return pg_prepare($argCon,$argStmt,$argQuery);
	}
	// SQL(Prepare) -----------------
	function db_execute(&$argCon,$argStmt,$argParams,$argLog=""){
		$argStmt = strtolower($argStmt);
		if ( $argLog != "" ) {
			$argLog->debug(PNAME,"STMT=".$argStmt." executing...");
		}
		return pg_execute($argCon,$argStmt,$argParams);
	}
	// SQL実行 -----------------
	function db_query(&$argCon,$argQuery,$argLog=""){
		if ( $argLog != "" ) {
			$argLog->debug(PNAME,$argQuery);
		}
		return pg_query($argCon, $argQuery);
		//$wQuery = mysql_escape_string($argQuery);		// 06.02.28 4.0.20 でエラーとしてしまうため外す
		//return mysql_query($wQuery);
		//return mysql_query($argQuery);
	}
	function db_exec(&$argCon,$argQuery,$argLog=""){
		if ( $argLog != "" ) {
			$argLog->put(PNAME,$argQuery);
		}
		$res = pg_query($argCon, $argQuery);
		$bt = debug_backtrace();
		$trace = basename($bt[0]['file'] . ' | ' . $bt[0]['function'] . ' | (' . $bt[0]['line'] . ')'); 
		if (!$res) {
			$errmsg = pg_last_error($argCon);
			echo "[SQL-ERROR]>>>>>".$errmsg." | ".$trace."\r\n";
			if ( $argLog != "" ) {
				$argLog->putErr(PNAME,"ERR:[".$errmsg."] > ERR [".$trace."] SQL:" . $sql);
			}
		}
		return $res;
		//return pg_query($argCon, $argQuery);			// 10.02.23 デバッグ組込
		//$wQuery = mysql_escape_string($argQuery);		// 06.02.28 4.0.20 でエラーとしてしまうため外す
		//return mysql_query($wQuery);
		//return mysql_query($argQuery);
	}
	// レコード数 ---
	function db_num_rows(&$argRes) {
		return pg_num_rows($argRes);
		//return mysql_num_rows($argRes);
	}
	function db_numrows(&$argRes) {
		return pg_numrows($argRes);
	}
	// フィールド数
	function db_num_fields(&$argRes) {
		return pg_num_fields($argRes);
	}
	function db_numfields(&$argRes) {
		return pg_num_fields($argRes);
	}
	// フィールド名
	function db_field_name(&$argRes,$argIndex) {
		return pg_field_name($argRes,$argIndex);
	}
	// フィールドタイプ
	function db_field_type(&$argRes,$argIndex) {
		return pg_field_type($argRes,$argIndex);
	}
	// レコードセット行フェッチ ---
	function db_fetch_row(&$argRes,$argIndex) {
		return pg_fetch_row($argRes,$argIndex);
	}
	// レコードセット行フェッチ ---
	//function db_fetch_array(&$argRes) {
	function db_fetch_array(&$argRes,$argIndex="") {
		if ( $argIndex != "" ) {
			return pg_fetch_array($argRes,$argIndex);
		} else {
			return pg_fetch_array($argRes);
		}
	}
	// レコードセット行フェッチ ---
	//function db_fetch_assoc(&$argRes) {
	function db_fetch_assoc(&$argRes,$argIndex="") {
		if ( $argIndex != "" ) {
			return pg_fetch_assoc($argRes,$argIndex);
		} else {
			return pg_fetch_assoc($argRes);
		}
	}
	// レコードセット行フェッチ ---
	function db_fetch_all(&$argRes) {
		return pg_fetch_all($argRes);
	}
	// 結果開放 ---
	function db_free_result(&$argRes) {
		return pg_freeresult($argRes);
		//return mysql_free_result($argRes);
	}
	function db_freeresult(&$argRes) {
		return pg_freeresult($argRes);
	}
	// 更新行数 ---
	function db_affected_rows(&$argRes) {
		return pg_cmdtuples($argRes);
		//return mysql_affected_rows();
	}
	// escape ---
	function db_esc($arg) {
		return pg_escape_string($arg);
		//return mysql_real_escape_string();
	}
	// タイムスタンプ格納用編集
	function db_timestamp($arg) {
		return "to_timestamp('".$arg."', 'YYYYMMDDHH24MISS')";
	}
	// die
	function db_die($argSql,$argLocker="") {
		if ( $argLocker != "" ) {
			$argLocker->rollback();
		}
		die("SQL実行エラー：SQL=".$argSql);
	}

	// テーブルを配列にコピーする ---
	function db_copy_to( $connect, $table_name ) {
		return pg_copy_to( $connect, $table_name );
	}

	// 配列からテーブルに挿入する ---
	function db_copy_from ( $connect, $table_name, $rows ) {
		return pg_copy_from ( $connect, $table_name, $rows );
	}


//###################################################################
// 旧バージョン
//###################################################################
//---------------------------------------------------------------
// 各DBへの接続を戻す
//---------------------------------------------------------------
function getConForKenpin() {
	return pg_Connect("host=".MASTER_DB_SERVER." port=".MASTER_DB_PORT." dbname=".MASTER_DB_NAME." user=".MASTER_DB_USER);
}
function getConForTenpo($argTenpoCd) {
	return pg_Connect("host=".TENPO_DBHOST." port=".TENPO_DBPORT." dbname=".$argTenpoCd." user=".TENPO_DBUSER);
}
function getConForList() {
	return pg_Connect("host=".LIST_DBHOST." port=".LIST_DBPORT." dbname=".LIST_DBNAME." user=".LIST_DBUSER);
}
//----------------------------
// データ有無
//----------------------------
function hasData(&$argCon,$argSql){
	$wRs = db_query($argCon, $argSql);
	if ( !$wRs ) { die("SQL ERROR!!!! SQL=".$argSql); }
	if ( db_numrows($wRs) > 0 ) {
		return true;
	} else {
		return false;
	}
}

?>
