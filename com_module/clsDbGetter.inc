<?php
//===============================================================
// データ収集クラス
// CDbGetter
//---------------------------------------------------------------
// 2021.05.22 初版（by inok）
//===============================================================
require_once("clsBase.inc");

class CDbGetter extends CBase {
	//---------------------------------------------------------------
	// 定数
	//---------------------------------------------------------------
	var $CNAME             = "CDbGetter";
	//---------------------------------------------------------------
    // プロパティ
	//---------------------------------------------------------------
	//---------------------------------------------------------------
    // ＤＢ
	//---------------------------------------------------------------
	//---------------------------------------------------------------
    // 格納エリア
	//---------------------------------------------------------------
	//---------------------------------------------------------------
    // コンストラクタ
	//---------------------------------------------------------------
    function CDbGetter(&$log) {
		$this->CBase($log);
	}
	//---------------------------------------------------------------
	// 拡張ＤＢ
	//---------------------------------------------------------------
	//---------------------------------------------------------------
	// getter & setter
	//---------------------------------------------------------------
	//---------------------------------------------------------------
	// getInfo
	//---------------------------------------------------------------
	/*
	function getZandakaInfo($argShiharaiCd) {
		return $this->zandakaList[$argShiharaiCd];
	}
	*/
	//---------------------------------------------------------------
	// getList：取得データをそのまま返す
	//---------------------------------------------------------------
	function getList(&$con,$argSql,$argFuncNm,$argMsg="getList") {
		$records = false;
		$res = $this->doSql($con,$argSql,$argFuncNm,$argMsg);
		if ( $res ) {
			$records = array();
			// csv編集
			for ( $i=0; $i<db_numrows($res); $i++ ) {
				$records[$i] = db_fetch_assoc($res, $i);
			}
		}
		return $records;
	}
	//---------------------------------------------------------------
	// getCsv：取得データをカンマ区切りデータで返す
	//---------------------------------------------------------------
	function getCsv(&$con,$argSql,$argFuncNm,$argMsg="getList") {
		$csv = false;
		$res = $this->doSql($con,$argSql,$argFuncNm,$argMsg);
		if ( $res ) {
			$csv = array();
			// csv編集
			for ( $i=0; $i<db_numrows($res); $i++ ) {
				$r = db_fetch_assoc($res, $i);
				$csv[$i] = arr2csv($r,'"');
			}
		}
		return $csv;
	}
	//---------------------------------------------------------------
	// doSql：SQL実行
	//---------------------------------------------------------------
	function doSql(&$con,$argSql,$argFuncNm,$argMsg="doSql") {
		$this->log->put($this->CNAME,$argFuncNm." doSql SQL=".$argSql);
		// 文字コード
		pg_set_client_encoding($con,"UNICODE");
		// SQL実行
		$res  = db_query($con,$argSql);
		if (!$res) {
			$this->setMessage($argMsg."に失敗しました（".$this->CNAME."/".$argFuncNm."）");
			$this->log->putErr($this->CNAME,$argFuncNm." ".$this->getMessage());
			$this->log->putErr($this->CNAME,$argFuncNm." ERROR=".db_last_error($con));
			$this->log->putErr($this->CNAME,$argFuncNm."   SQL=".$argSql);
			return false;
		}
		$this->log->put($this->CNAME,$argFuncNm." db_numrows=".db_numrows($res));
		return $res;
	}

	//---------------------------------------------------------------
	// getZandakaList：対象買掛残高をリストし配列に
	//---------------------------------------------------------------
	/*
	function getZandakaList() {
		$this->log->put($this->CNAME,__FUNCTION__." 締グループ=".$this->getSimeGrpCd()." 期間=".$this->getStartDt()."-".$this->getEndDt());
		// 対象買掛残高取得
		$sql  = "select *";
		$sql .=  " from 買掛残高";
		$sql .= " where 支払先コード in (";
		$sql .=         "select 支払先コード from 取引先 where 締グループコード='".$this->getSimeGrpCd()."' and 削除フラグ='0'";
		$sql .=         ")";
		$sql .=   " and 締グループコード='".$this->getSimeGrpCd()."'";
		$sql .=   " and 集計区分='1'";
		$sql .=   " and 終了日='".$this->getEndDt()."'";
		$sql .= " order by 支払先コード";
		$res  = $this->doSql($this->con,$sql,__FUNCTION__,"買掛残高取得");
		if (!$res) { return false; }
		// 買掛残高件数
		$this->log->put($this->CNAME,__FUNCTION__." 買掛残高対象=".db_numrows($res));
		$this->set("買掛残高対象件数",db_numrows($res));

		// 買掛残高リスト格納
		for ( $i=0; $i<db_numrows($res); $i++ ) {
			$r = db_fetch_assoc($res, $i);
			$this->zandakaList[$r["支払先コード"]]	= $r;
		}
		db_freeresult($res);

		return true;
	}
	*/
	//---------------------------------------------------------------
	// makeLogin：対象支払先のログイン情報を作成
	//---------------------------------------------------------------
	/*
	function makeLogin() {
		$this->log->put($this->CNAME,__FUNCTION__." 締グループ=".$this->getSimeGrpCd()." 支払先件数=".countAny($this->shiharaiList));

		foreach ( $this->shiharaiList as $keyShiharaiCd => $r ) {
			// 取引先コード桁数を揃える
			$wShiharaiCd	= trimCode($keyShiharaiCd,$this->getLenToriCd());
			//------------------------------------------
			// 	1) ログインＩＤ
			//------------------------------------------
			//if ( $r["ＥＤＩ取引先コード"] == "" ) {
				$wLoginId		= $wShiharaiCd;
				$wOrosiKigyoCd	= $this->getKouriKigyoCd().$wLoginId;
			//} else {
			//	$wLoginId		= substr($r["ＥＤＩ取引先コード"],0,6);
			//	$wOrosiKigyoCd	= $wLoginId;
			//}
			$this->shiharaiList[$keyShiharaiCd]["代替卸企業コード"]	= $wOrosiKigyoCd;
			//------------------------------------------
			// 	2) ログイン情報が無ければ作成
			//------------------------------------------
			$sql  = "select * from ログイン情報";
			$sql .= " where ログインＩＤ='".$wLoginId."'";
			$sql .=   " and 小売企業コード='".$this->getKouriKigyoCd()."'";
			$res  = $this->doSql($this->conOro,$sql,__FUNCTION__,"ログイン情報存在チェック");
			if (!$res) { return false; }
			if ( db_numrows($res) > 0 ) {
				$this->set("ログイン既存件数",$this->get("ログイン既存件数")+1);
			} else {
				$wCd						= $this->getDefaultPwd($wLoginId);
				$arr = "";
				$arr["ログインＩＤ"]		= addQuote($wLoginId);
				$arr["小売企業コード"]		= addQuote($this->getKouriKigyoCd());
				$arr["担当者"]				= addQuote("");
				$arr["取引先コード"]		= addQuote($wShiharaiCd);
				$arr["卸企業コード"]		= addQuote($wOrosiKigyoCd);
				$arr["代替卸企業コード"]	= addQuote($wOrosiKigyoCd);
				$arr["パスワード"]			= addQuote(crypt($wCd,$this->getKouriKigyoCd()));
				$arr["作成担当"]			= addQuote($this->getTanto());
				$arr["作成時刻"]			= "to_timestamp('".TIMESTAMP."','YYYYMMDDHH24MISS')";
				$arr["更新担当"]			= addQuote($this->getTanto());
				$arr["更新時刻"]			= "to_timestamp('".TIMESTAMP."','YYYYMMDDHH24MISS')";
				$arr["コメント２"]			= addQuote($wCd);

				$sql  = arr2insert("ログイン情報",$arr);
				$res  = $this->doSql($this->conOro,$sql,__FUNCTION__,"ログイン情報登録");
				if (!$res) { return false; }

				$this->set("ログイン登録件数",$this->get("ログイン登録件数")+1);
			}
		}
		db_freeresult($res);
		$this->log->put($this->CNAME,__FUNCTION__." 完了");

		return true;
	}
	*/
	//---------------------------------------------------------------
	// makePaymentHeader：対象支払先の支払案内ヘッダを作成
	//---------------------------------------------------------------
	/*
	function makePaymentHeader() {
		$this->log->put($this->CNAME,__FUNCTION__." 締グループ=".$this->getSimeGrpCd()." 支払先件数=".countAny($this->shiharaiList));

		foreach ( $this->shiharaiList as $keyShiharaiCd => $r ) {
			// 取引先コード桁数を揃える
			$wShiharaiCd	= trimCode($keyShiharaiCd,$this->getLenToriCd());
			//------------------------------------------
			// 	1) 支払案内ヘッダ有無
			//------------------------------------------
			$sql  = "select * from payment_header";
			$sql .= " where kouri_kigyo_cd='".$this->getKouriKigyoCd()."'";
			$sql .=   " and kouri_tori_cd='".$wShiharaiCd."'";
			$sql .=   " and sime_dt='".$this->getEndDt()."'";
			$res  = $this->doSql($this->conOro,$sql,__FUNCTION__,"支払案内ヘッダ存在チェック");
			if (!$res) { return false; }

			$arr = "";

			$wShiharaiInfo				= $this->getShiharaiInfo($keyShiharaiCd);
			$wZandakaInfo				= $this->getZandakaInfo($keyShiharaiCd);

			$arr["siharai_dt"]			= addQuote($wZandakaInfo["支払予定日"]);
			$arr["orosi_kigyo_cd"]		= addQuote($r["代替卸企業コード"]);
			$arr["kouri_kigyo_nm"]		= addQuote(KIGYO_NM);
			$arr["orosi_kigyo_nm"]		= addQuote($wShiharaiInfo["取引先名"]);
			$arr["paylist_type"]		= addQuote($this->getPayListType());
			$arr["udt"]					= addQuote(substr(TIMESTAMP,0,8));
			$arr["utm"]					= addQuote(substr(TIMESTAMP,8));

			if ( db_numrows($res) > 0 ) {

				$sql  = "update payment_header set ".arr2set($arr);
				$sql .= " where kouri_kigyo_cd='".$this->getKouriKigyoCd()."'";
				$sql .=   " and kouri_tori_cd='".$wShiharaiCd."'";
				$sql .=   " and sime_dt='".$this->getEndDt()."'";

				$res  = $this->doSql($this->conOro,$sql,__FUNCTION__,"支払案内ヘッダ登録");
				if (!$res) { return false; }
				$this->set("支払案内ヘッダ更新件数",$this->get("支払案内ヘッダ更新件数")+1);
			} else {
				$arr["kouri_kigyo_cd"]		= addQuote($this->getKouriKigyoCd());
				$arr["kouri_tori_cd"]		= addQuote($wShiharaiCd);
				$arr["sime_dt"]				= addQuote($this->getEndDt());
				$arr["indt"]				= addQuote(substr(TIMESTAMP,0,8));
				$arr["intm"]				= addQuote(substr(TIMESTAMP,8));

				$sql  = arr2insert("payment_header",$arr);
				$res  = $this->doSql($this->conOro,$sql,__FUNCTION__,"支払案内ヘッダ登録");
				if (!$res) { return false; }

				$this->set("支払案内ヘッダ作成件数",$this->get("支払案内ヘッダ作成件数")+1);
			}
		}
		db_freeresult($res);
		$this->log->put($this->CNAME,__FUNCTION__." 完了");

		return true;
	}
	*/
	//---------------------------------------------------------------
	// getDefaultPwd($pKigyoCd)
	// 引数		KIGYO_CD
	// 戻り値	PASSWORD BY CALC
	//---------------------------------------------------------------
	/*
	function getDefaultPwd($pKigyoCd) {
		$wVal        = $pKigyoCd;
		$wDefaultPwd = "";
		if ( is_numeric($wVal) ) {
			$wVal = $wVal * 7;
		}
		if ( strlen($wVal) < 5 ) {
			$wVal = str_pad($wVal,5,"0",STR_PAD_LEFT);
		}
		for ( $i=strlen($wVal) ; strlen($wDefaultPwd)<5 ; $i-- ) {
			$wDefaultPwd .= substr($wVal,$i-1,1);
		}
		return $wDefaultPwd;
	}
	*/
}
?>