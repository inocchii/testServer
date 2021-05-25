<?php
	//require_once("fncBarcode.inc");
//----------------------------------------------------------------------------------
//	汎用関数群
//----------------------------------------------------------------------------------
//	●●●●●更新履歴●●●●●
//  2020/02/18 hidaka
//      trimSimeDt関数の処理が誤っていたので修正。
//  2019/09/11  matsuba
//      全角変換を追加（convert_kana_zen）※deal2020より移植
//      機種依存文字存在チェック 変換を追加（convert_sjis_gaiji）※deal2020より移植
//  2019/07/30  waka
//      実数マイナスの扱いを変更(trimHasu,trimHasuR)
//  2019/04/23  hidaka
//      元号が令和に変わるタイミングで、和暦関数を共通化した。
//  2018/11/12  matsuba
//      文字コード変更[eucjp-win→CP51932]を追加（encoding_Display）
//      文字コード変更[CP51932→eucjp-win]を追加（encoding_DBRegister）
//  2018/08/23  imamura
//      weekinfoを追加
//  2018/07/17  hidaka
//      dieAndOpenDialogの削除
//  2018/05/31  hidaka
//      dieAndOpenDialogの追加
//  2018/05/09  wakamatsu
//      getGetsudoFromStrのバグフィックス
//  2016/08/04  matsuba
//      メールアドレスの妥当性チェックを追加（checkMailAddress）
//      支払予定日取得を追加（getSiharaiDt）
//      締日調整を追加（trimSimeDt）
//      日付編集(漢字編集)（editDateKJ）
//  2016/08/04  onitsuka
//      arr2csvを修正
//  2016/03/04  kichise
//      2月末日の閏年補正をするleapCorrect()を追加
//  2014/05/18  inok
//      配列変換機能追加（arr2SqlArray/arrFromSqlArray）
//      月度配列生成を追加（makeArrGetsudo）
//      日数取得を追加（getIntervalByTime）
//      回帰直線を追加（getRegressionLine）
//      正数のみを戻す機能（getNumberFromZero）
//  2014/04/21  inok
//      setDebugを修正（setDebug）
//  2014/02/15  inok
//      pushUrlに処理区分の追加（pushUrl：DOWNLOAD）
//  2014/02/14  inok
//      小数点以下編集の改良（editDecimal）
//  2014/02/12  inok
//      小数点以下の取出し（getDecimalOnly）
//  2013/12/30  inok
//      漢字文字列の長さ合わせ（padKanji）
//  2013/12/28  inok
//      日付と曜日の月間リスト取得（getDateLine/getYoubiLine）
//  2013/12/24  inok
//      発注数への小数入力対応（getSuryoByUnitKbn/getBaraSuryoByUnitKbn）
//	2013/09/19	若松
//		数量×原単価=0.0(float型)の判定追加 (getGenkaGaku)
//	2013/06/12	inok
//		簡易coalesce機能（null2next 二つの値を渡し、値を有するほうを戻す）
//		　　　　　　　　（zero2next 二つの値を渡し、値を有するほうを戻す）
//	2013/03/05	inok
//		端数処理を改善（trimHasu/trimHasuR(額,端数区分) PHP誤差対応の精度向上）
//	2013/02/26	inok
//		isDebug 追加（isDebug）
//	2013/02/14	kurog
//		strposのwarning回避（pushUrl）
//	2012/12/22	inok
//		使用開始日の取得（getSiyoKaisibi）
//	2012/07/17	inok
//		URL保持除外用の処理区分条件を追加（pushUrl）
//	2012/04/06	inok
//		デバッグ設定（setDebug(&$log,DEBUG_MODE,&$_SESSION,$P)）
//	2011/04/24	inok
//		QUERY生成を改良（arr2query(QUERY設定用配列,空値設定するか)）
//	2011/02/07	inok
//		単位区分を取得（getUnitKbn(発注単位,ボール入数)）
//	2011/02/06	inok
//		FORM.TEXTAREAを改行分割して配列で取得（getArrayFromTextArea()）
//	2011/01/15	inok
//		文字列から日付8桁を取得（getHiDateFromStr(文字列)）
//	2011/01/06	kurog
//		カンマ有無を追加（editDecimal(数値,桁数,カンマ有無)）
//	2010/12/30	inok
//		端数処理を追加（trimHasu/trimHasuR(額,端数区分)）
//	2010/12/29	inok
//		期間取得を追加（getKikan(基準日,前日数,後日数)）
//	2010/12/14	kurog
//		小数点の数値編集機能を追加（editDecimal）
//	2010/12/14	inok
//		品名漢字分解機能追加（splitItemNmKj）
//	2010/12/13	inok
//		NULL関連編集を追加（null2zero等）
//	2010/11/11	kichise
//		日付編集editDate()にパターン"YYMMDDKW"（末尾に漢字曜日を付加）を追加
//

	//---------------------------------------------------------------
	// setDebug デバッグ設定
	// 引数		&$argLog      : ログオブジェクト
	//			$argDebugMode : 定数を与える（const.inc）
	//			&$argSession  : セッションオブジェクト
	//			$argParams    : 画面パラメータ（$P）
	// 戻り値	true（無意味）
	// 備考		デバッグモードの設定優先順位
	//				①画面パラメータ②セッション③定数
	//---------------------------------------------------------------
	function setDebug(&$argLog,$argDebugMode,&$argSession,$argParams) {
		//----------------------------------------------------
		// パラメータ指定は最優先しセッションに保持する
		//----------------------------------------------------
		if ( $argParams["DEBUG_MODE"] ) {
			switch ($argParams["DEBUG_MODE"]) {
				case "true" :
				case "TRUE" :
				case "Y" :
					$argSession["DEBUG_MODE"] = true;
					$argLog->setDebug(true);
					break;
				case "false" :
				case "FALSE" :
				case "N" :
					$argSession["DEBUG_MODE"] = false;
					$argLog->setDebug(false);
					break;
			}
		//----------------------------------------------------
		// セッション保持分を次に優先
		//----------------------------------------------------
		} else if ( is_bool($argSession["DEBUG_MODE"]) ) {
			$argLog->setDebug($argSession["DEBUG_MODE"]);
		//----------------------------------------------------
		// 定数を次に優先
		//----------------------------------------------------
		//} else if ( $argDebugMode != "" ) {
		} else if ( is_bool($argDebugMode) ) {
			$argLog->setDebug($argDebugMode);
		//----------------------------------------------------
		// 指定が無い場合はtrueに
		//----------------------------------------------------
		} else {
			$argLog->setDebug(true);
		}
		return true;
	}

	//---------------------------------------------------------------
	// DEBUGモード判定
	// 戻り値：boolean
	// ※ setDebugで設定された$_SESSION["DEBUG_MODE"]に依存
	// ※ 優先順： $_SESSION["DEBUG_MODE"] > 定数(DEBUG_MODE) > true
	//---------------------------------------------------------------
	function isDebug() {
		if ( in_array("DEBUG_MODE", $_SESSION) ) {
			return $_SESSION["DEBUG_MODE"];
		} else if ( defined("DEBUG_MODE") ) {
			return DEBUG_MODE;
		} else {
			return true;
		}
	}

	//---------------------------------------------------------------
	// DEBUG出力
	// 引数		$pDebug：boolean
	// 			$pMmsg：メッセージ
	//---------------------------------------------------------------
	function debug($pDebug,$pMsg) {
		if ( $pDebug == "ON" || $pDebug == true ) {
			echo($pMsg);
		}
	}

	//---------------------------------------------------------------
	// number_format(3桁毎カンマ形式)の拡張版
	// 引数		in_num : 数値(カンマ編集前)
	//			in_dec : 小数桁
	// 戻り値	数値(カンマ編集後)
	// 備考		小数桁指定時に.00の場合は整数部のみ表示する
	//---------------------------------------------------------------
	function func_num_fmt($in_num,$in_dec) {

		$lo_num1	= "";
		$lo_num2	= "";

		$in_num = doubleval($in_num);
		$lo_num1 = explode(".", $in_num);			// $in_numを整数部($lo_num1[0])と小数部($lo_num1[1])に分割

		if ($lo_num1[1] == "") {
			$lo_num2 = number_format($in_num);		// 小数部がない場合は整数部のみ表示
		} else {
			$lo_num2 = number_format($in_num,$in_dec);
		}

		return $lo_num2;

	}

	//---------------------------------------------------------------
	// checkdate(日付の妥当性チェック)の強化版
	// 引数		in_year	: 年
	//			in_month: 月
	//			in_day	: 日
	// 戻り値	true	: 正常
	//			false	: エラー
	// 備考		Nullと数値以外にも対応
	//---------------------------------------------------------------
	function func_chk_date($in_year,$in_month,$in_day) {

//		if ($in_year != "" and $in_month != "" and $in_day != ""){
			if (is_numeric($in_year) and is_numeric($in_month) and is_numeric($in_day)) {
				if (checkdate($in_month,$in_day,$in_year) == true) {
					return true;
				}
			}
//		}
		return false;

	}

	//---------------------------------------------------------------
	// 日付入力チェック(月日入力仕様)
	// 引数		in_month	: 月(入力値)
	//			in_day		: 日(入力値)
	//			in_min		: -364～364(日)が設定可能(ただし、in_max - in_min が 0～364)
	//			in_max		: -364～364(日)が設定可能(ただし、in_max - in_min が 0～364)
	// 戻り値	lo_ret		: 00:正常 01:非数値 02:非実在日 03:入力可能範囲外
	//			lo_year		: 年
	//---------------------------------------------------------------
	function func_chk_dt($in_month,$in_day,$in_min,$in_max) {

		// in_minとin_maxの設定チェック
		if ($in_min < -364 or $in_min > 364) {
			echo "<br>func_chk_dtのin_min値エラー<br>";
			exit;
		}
		if ($in_max < -364 or $in_max > 364) {
			echo "<br>func_chk_dtのin_max値エラー<br>";
			exit;
		}
		if ($in_max - $in_min < 0 or $in_max - $in_min > 364) {
			echo "<br>func_chk_dtのin_minとin_maxの範囲エラー<br>";
			exit;
		}

		// 初期化
		$lo_ret		= "";
		$lo_year	= "";
		$lo_mindate	= "";
		$lo_maxdate	= "";

		if (is_numeric($in_month) and is_numeric($in_day)) {
			// 数値の場合

			// 処理日から年を取得
			$Wk_ShoriDate	= date('Ymd');
			$lo_year		= substr($Wk_ShoriDate,0,4);

			// 入力した月日の先頭に年を仮設定
			$lo_date = sprintf("%04d",$lo_year).sprintf("%02d",$in_month).sprintf("%02d",$in_day);

			// 入力可能範囲を算出
			$lo_mindate	= getDateByKikan($Wk_ShoriDate,$in_min);
			$lo_maxdate	= getDateByKikan($Wk_ShoriDate,$in_max);

			// 年を決定
			if ($Wk_ShoriDate <= $lo_date) {
				// 	処理日 <= 入力日の場合
				if ($lo_maxdate < $lo_date) {
					// Max日付 < 入力日の場合は年を１マイナス
					$lo_year = $lo_year - 1;
					$lo_date = sprintf("%04d",$lo_year).sprintf("%02d",$in_month).sprintf("%02d",$in_day);
				}
			} else {
				// 	入力日 < 処理日の場合
				if ($lo_date < $lo_mindate) {
					// 入力日 < Min日付の場合は年を１プラス
					$lo_year = $lo_year + 1;
					$lo_date = sprintf("%04d",$lo_year).sprintf("%02d",$in_month).sprintf("%02d",$in_day);
				}
			}

			// 日付の妥当性をチェック
			if (checkdate($in_month,$in_day,$lo_year) == true) {
				// 実在する場合

				// 入力可能範囲内かチェック
				if ($lo_mindate <= $lo_date and $lo_date <= $lo_maxdate) {
					// 入力可能範囲内
					$lo_ret = "00";
				} else {
					// 入力可能範囲外
					$lo_ret = "03";
				}

			} else {
				// 実在しない場合
				$lo_ret = "02";
			}

		} else {
			// 数値以外の場合
			$lo_ret = "01";
		}

		return array($lo_ret,$lo_year);

	}

	//---------------------------------------------------------------
	// 数値の桁数チェック
	// 引数		in_num	: 数値
	//			in_int	: 整数桁(Max)
	//			in_dec	: 小数桁(Max)
	// 戻り値	true	: 正常
	//			false	: エラー
	//---------------------------------------------------------------
	function func_chk_num($in_num,$in_int,$in_dec) {

		$lo_num	= "";

		$in_num = doubleval($in_num);
		$lo_num = explode(".", $in_num);			// $in_numを整数部($lo_num[0])と小数部($lo_num[1])に分割

		if (strlen($lo_num[0]) <= $in_int && strlen($lo_num[1]) <= $in_dec) {
			return true;
		}

		return false;

	}

	//---------------------------------------------------------------
	// 金額 端数計算
	//---------------------------------------------------------------
	// 引数		in_kingaku	: 金額(端数処理前)
	//			in_hasukbn	: 端数処理区分(1:切捨、2:四捨五入、3:切上)
	// 戻り値	lo_ret		: 処理結果(00:正常 88:オーバーフロー)
	// 			lo_kingaku	: 金額(端数処理後)
	//
	// 注意		丸め誤差によりin_kingakuが循環小数(67.59999･･･等)に
	//			ならないように呼び出し側で考慮する必要があります
	//---------------------------------------------------------------
	function func_kin_hasu_calc($in_kingaku,$in_hasukbn) {

		$lo_minus	= false;
		$lo_kingaku	= 0;
		$lo_ret		= "00";

		if ($in_kingaku < 0 ) {
			$lo_minus	= true;
		}

		switch ($in_hasukbn) {
			case "1":
				// 切捨
				if ($lo_minus) {
					$lo_kingaku	= ceil($in_kingaku);
				} else {
					$lo_kingaku	= floor($in_kingaku);
				}
				break;
			case "3":
				// 切上
				if ($lo_minus) {
					$lo_kingaku	= floor($in_kingaku);
				} else {
					$lo_kingaku	= ceil($in_kingaku);
				}
				break;
			default:
				// 四捨五入
				$lo_kingaku	= round($in_kingaku);
				break;
		}

		if (abs($lo_kingaku) > 99999999) {
			$lo_ret	= "88";
			$lo_kingaku = 0;
		}

		return array($lo_ret,$lo_kingaku);

	}

	//---------------------------------------------------------------
	// FORMパラメータをESCAPE
	//---------------------------------------------------------------
	function escapeFormParam($argGet,$argPost) {
		foreach ($argGet as $item=>$value) {
			if ( is_array($value) ) {
				$wParams[$item] = $value;
			} else {
				$wParams[$item] = strip_tags($value);
			}
		}
		foreach ($argPost as $item=>$value) {
			if ( is_array($value) ) {
				$wParams[$item] = $value;
			} else {
				$wParams[$item] = strip_tags($value);
			}
		}
		return $wParams;
	}

	//---------------------------------------------------------------
	// FORMパラメータ取得
	//---------------------------------------------------------------
	function getFormParam($pname) {
		if ( $_POST[$pname] != "" ) {
			return $_POST[$pname];
		} elseif ( $_GET[$pname] != "" ) {
			return $_GET[$pname];
		} else {
			return "";
		}
	}

	//---------------------------------------------------------------
	// FORMパラメータ取得(配列対応)
	//---------------------------------------------------------------
	function getFormArray($pname,$pidx) {
		if ( $_POST[$pname][$pidx] != "" ) {
			return $_POST[$pname][$pidx];
		} elseif ( $_GET[$pname][$pidx] != "" ) {
			return $_GET[$pname][$pidx];
		} else {
			return "";
		}
	}

	//---------------------------------------------------------------
	// TEXTAREAを行の配列に
	//---------------------------------------------------------------
	function getArrayFromTextArea($pTextArea) {
		$pTextArea = str_replace("\r\n","\n",$pTextArea);
		$pTextArea = str_replace("\r","\n",$pTextArea);
		$wArr      = explode("\n",$pTextArea);
		return $wArr;
	}

	//---------------------------------------------------------------
	// nullの場合に&nbspを戻す
	//---------------------------------------------------------------
	function null2nbsp($pStr) {
		if ( $pStr == "" ) {
			return "&nbsp";
		} else {
			return $pStr;
		}
	}
	function null2null($pStr) {
		if ( $pStr == "" ) {
			return "null";
		} else {
			return $pStr;
		}
	}
	function null2zero($pStr) {
		if ( $pStr == "" ) {
			return "0";
		} else {
			return $pStr;
		}
	}
	//---------------------------------------------------------------
	// coalesce機能
	//---------------------------------------------------------------
	function null2next($pStr,$pStr2) {
		if ( $pStr == "" ) {
			return $pStr2;
		} else {
			return $pStr;
		}
	}
	function zero2next($pStr,$pStr2) {
		if ( $pStr == "" || getNumeric($pStr) == 0 ) {
			return $pStr2;
		} else {
			return $pStr;
		}
	}
	//---------------------------------------------------------------
	// ゼロをNULL化する(表示用)
	//---------------------------------------------------------------
	function zero2null($pStr) {
		$wStr = getNumeric($pStr);
		if ( is_numeric($pStr) && $pStr == 0 ) {
			return "";
		}
		return $pStr;
	}

	function addQuote($pStr) {
		return "'".$pStr."'";
	}
	function nullOrQuote($pStr) {
		if ( $pStr == "" ) {
			return "null";
		} else {
			return "'".$pStr."'";
		}
	}
	//---------------------------------------------------------------
	// count() のwrapper
	// null配列に対応（PHP5の厳しさから逃げる）
	//---------------------------------------------------------------
	function countAny($pArr) {
		if ( !is_array($pArr) ) {
			return 0;
		}
		return count($pArr);
	}


	//---------------------------------------------------------------
	// HTML文に改行つきで行を追加する。
	//---------------------------------------------------------------
	function addMessage($pOrgin,$pNew) {
		if ( strlen($pOrgin) == 0 ) {
			return $pNew;
		} else {
			return "<br />".$pNew;
		}
	}

	//---------------------------------------------------------------
	// フォーム等からの入力値を数値として取り出し
	// 引数　　：$pStr：入力された数値（カンマ編集等）
	// 戻り値　：取り出した数値
	// 処理内容：入力値から「,」を取り除いて戻す。
	//---------------------------------------------------------------
	function getNumeric($pStr) {
		return str_replace(",","",$pStr);
	}

	//---------------------------------------------------------------
	// 小数点付き数値から小数以下のみを取り出す
	// 引数１　：$pStr：入力された数値（カンマ編集等）
	// 引数２　：$pLen：小数点以下桁数
	// 戻り値　：小数点以下
	//---------------------------------------------------------------
	function getDecimalOnly($pStr,$pLen=3) {
		$wNum = getNumeric($pStr);
		$wDecimal = round( ($wNum - floor($wNum)), $pLen );
		return $wDecimal;
	}

	//---------------------------------------------------------------
	// フォーム等からの入力値を数値として取り出し
	// 引数　　：$pStr：入力された数値（カンマ編集等）
	// 戻り値　：取り出した数値を再度カンマ編集
	//---------------------------------------------------------------
	function editNumeric($pStr,$pPeriod=0) {
		$wStr = getNumeric($pStr);
		if ( is_numeric($wStr) ) {
			return number_format($wStr,$pPeriod);
		}
		return $pStr;
	}

	function trimNumber($pStr) {
		if ( $pStr == "" || $pStr == "0" ) {
			return "";
		}
		return number_format($pStr);
	}

	//---------------------------------------------------------------
	// 小数点以下の不要な'0'を削除する
	// 引数　　：$pStr   ：入力された数値
	// 		   ：$decimal：チェック桁数
	// 		   ：$flg    ：カンマ有無
	// 戻り値　：小数点以下の不要な'0'を削除した数値
	//		     $flgが'true'の場合にカンマ有り
	//---------------------------------------------------------------
	function editDecimal($pStr,$decimal=1,$flg=true) {
		//if ( $pStr == "" ) {
		if ( $pStr === "" ) {
			return "";
		}
		// カンマを取り除く
		$nStr = getNumeric($pStr);
		// 数値チェック
		if (!is_numeric($nStr)) {
			return $pStr;
		}

		$wStr = round($nStr);
		if ($wStr == $nStr) {
			if ($flg == true) {
				return number_format($nStr);
			} else {
				return getNumeric(number_format($nStr));
			}
		}
		for ($i = 1; $i <= $decimal; $i++) {
			$wStr = round($nStr,$i);
			if ($wStr == $nStr) {
				if ($flg == true) {
					return number_format($nStr,$i);
				} else {
					return getNumeric(number_format($nStr,$i));
				}
			}
		}
		if ($flg == true) {
			return number_format($nStr,$decimal);
		} else {
			return getNumeric(number_format($nStr,$decimal));
		}
	}

	//---------------------------------------------------------------
	// 整数かの確認
	//---------------------------------------------------------------
	function checkInt($pStr) {
		if ( !is_numeric($pStr) ) {
			return false;
		}
		if ( $pStr != floor($pStr) ) {
			return false;
		}
		return true;
	}

	//---------------------------------------------------------------
	// 桁数の確認
	//---------------------------------------------------------------
	function checkNumeric($pStr,$pLenInt=7,$pLenDec=0) {
		if ( !is_numeric($pStr) ) {
			return false;
		}
		if ( !func_chk_num($pStr,$pLenInt,$pLenDec) ) {
			return false;
		}
		return true;
	}

	//---------------------------------------------------------------
	// ゼロかの確認
	//---------------------------------------------------------------
	function isZero($pStr) {
		if ( !is_numeric($pStr) ) {
			return false;
		}
		//$pStr = editDecimal($pStr);
		//if ( preg_match("/^[0]+$/", $pStr) ) {
		if ( preg_match("/^[0]+\.?[0-9]*$/", $pStr) ) {
			return true;
		}
		return false;
	}

	//---------------------------------------------------------------
	// ゼロ以上として戻す
	// 　マイナスの場合はゼロとする。
	//---------------------------------------------------------------
	function getNumberFromZero($pNum) {
		$wNum = $pNum;
		if ( $wNum < 0 ) {
			$wNum = 0;
		}
		return $wNum;
	}

	//---------------------------------------------------------------
	// URLからGETパラメータを除去する。
	// 引数　　：$pStr：URL
	// 戻り値　：除去後のURL
	// 処理内容：入力値の「?」以降を取り除いて戻す。
	//---------------------------------------------------------------
	function removeParam($pStr) {
		$wArr = explode("?",$pStr);
		return $wArr[0];
	}

	//---------------------------------------------------------------
	// 文字列を検査しタイムスタンプを取得
	// $pStr ：YYYYMMDD,YYMMDD,YYYY/MM/DD,YY/MM/DD,YYYY-MM-DD,YY-MM-DD
	// 戻り値   ：int time()
	//---------------------------------------------------------------
	function getTimeFromStr($pStrDate) {
		$invalidDate = 0;
		if ( is_numeric($pStrDate) ) {
			if ( strlen($pStrDate) == 8 ) {
				$wYear  = substr($pStrDate,0,4);
				$wMonth = substr($pStrDate,4,2);
				$wDay   = substr($pStrDate,6,2);
			} else if ( strlen($pStrDate) == 6 ) {
				$wYear  = substr($pStrDate,0,2)+2000;
				$wMonth = substr($pStrDate,2,2);
				$wDay   = substr($pStrDate,4,2);
			} else {
				return $invalidDate;
			}
		} else {
			$wDate = strtr($pStrDate,"-.","//");
			if ( substr_count($wDate,"/") == 2 ) {
				$wYMD   = explode("/",$wDate);
				$wYear  = $wYMD[0];
				if ( strlen($wYear) <=2 ) {
					$wYear = "20".$wYear;
				}
				$wMonth = $wYMD[1];
				$wDay   = $wYMD[2];
			} else {
				return $invalidDate;
			}
		}
		if ( func_chk_date($wYear,$wMonth,$wDay) ) {
			return (int)strtotime($wYear."/".$wMonth."/".$wDay);
		} else {
			return $invalidDate;
		}
	}

	//---------------------------------------------------------------
	// 文字列を検査し年月度を取得
	// $pStr ：YYYYMMDD,YYMMDD,YYYY/MM/DD,YY/MM/DD,YYYY-MM-DD,YY-MM-DD
	// 戻り値：YYYYMM
	//---------------------------------------------------------------
	function getHiDateFromStr($pStrDate) {
		$invalidDate = 0;
		// 文字列を検査しタイムスタンプを取得
		$wTime = getTimeFromStr($pStrDate);
		if ( $wTime === $invalidDate ) {
			return $pStrDate;
		}
		return getHiDateFromTime($wTime);
	}

	//---------------------------------------------------------------
	// 文字列を検査し年月度を取得
	// $pStr ：YYYYMM,YYMM,YYYY/MM,YY/MM,YYYY-MM,YY-MM
	// 戻り値：YYYYMM
	//---------------------------------------------------------------
	function getGetsudoFromStr($pStrGetsudo) {
		if ( is_numeric($pStrGetsudo) ) {
			if ( strlen($pStrGetsudo) == 6 ) {
				$wYear  = substr($pStrGetsudo,0,4);
				$wMonth = substr($pStrGetsudo,4,2);
			} else if ( strlen($pStrGetsudo) == 4 ) {
				$wYear  = substr($pStrGetsudo,0,2)+2000;
				$wMonth = substr($pStrGetsudo,2,2);
			} else {
				return false;
			}
		} else {
			$wGetsudo = strtr($pStrGetsudo,"-.","//");
			if ( substr_count($wGetsudo,"/") == 1 ) {
				//$wYMD   = explode("/",$wDate);
				$wYMD   = explode("/",$wGetsudo);
				$wYear  = $wYMD[0];
				if ( strlen($wYear) <=2 ) {
					$wYear = "20".$wYear;
				}
				$wMonth = $wYMD[1];
				if ( strlen($wMonth) <2 ) {
					$wMonth = "0".$wMonth;
				}
			} else {
				return false;
			}
		}
		if ( func_chk_date($wYear,$wMonth,"01") ) {
			return $wYear.$wMonth;
		} else {
			return false;
		}
	}

	//---------------------------------------------------------------
	// $pHiDate ：YYYYMMDD
	// 戻り値   ：int 曜日（0：日曜 ～ 6：土曜）
	//---------------------------------------------------------------
	function getYoubiFromHiDate($pHiDate) {
		$wDate = substr($pHiDate,0,4)."/".substr($pHiDate,4,2)."/".substr($pHiDate,6,2);
		return date("w",strtotime($wDate));
	}

	//---------------------------------------------------------------
	// $pTimeFrom,$pTimeTo
	// 戻り値   ：int interval
	//---------------------------------------------------------------
	function getIntervalByTime($pTimeFrom,$pTimeTo) {
		return round( ($pTimeTo - $pTimeFrom) / 86400 );
	}

	//---------------------------------------------------------------
	// $pYoubi(from,to) ：int 曜日（0：日曜 ～ 6：土曜）
	// 戻り値   ：int interval
	//---------------------------------------------------------------
	function getIntervalByYoubi($pYoubiFrom,$pYoubiTo) {
		$wInterval = $pYoubiTo - $pYoubiFrom;
		if ( $wInterval >= 0 ) {
			return $wInterval;
		}
		return ($wInterval + 7);
	}

	//---------------------------------------------------------------
	// $pYoubi(from) ：int 曜日（0：日曜 ～ 6：土曜）
	// $pImterval    ：int 0-
	// 戻り値   ：int 曜日（0：日曜 ～ 6：土曜）
	//---------------------------------------------------------------
	function getYoubiByInterval($pYoubiFrom,$pInterval) {
		if ( is_numeric($pYoubiFrom) && is_numeric($pInterval) ) {
			$wYoubi = $pYoubiFrom + $pInterval;
			return $wYoubi % 7;
		} else {
			return false;
		}
	}

	//---------------------------------------------------------------
	// $pYoubi  ：int 曜日（0:日～6:土）
	// 戻り値   ：曜日漢字（日～土）
	//---------------------------------------------------------------
	function getYoubiKJ($pYoubi) {
		switch ($pYoubi) {
			case "0":
				return "日";
			case "1":
				return "月";
			case "2":
				return "火";
			case "3":
				return "水";
			case "4":
				return "木";
			case "5":
				return "金";
			case "6":
				return "土";
		}
	}

	//---------------------------------------------------------------
	// $pHiDate ：YYYYMMDD
	// 戻り値   ：int time()
	//---------------------------------------------------------------
	function getTimeFromHiDate($pHiDate) {
		$wDate = substr($pHiDate,0,4)."/".substr($pHiDate,4,2)."/".substr($pHiDate,6,2);
		return (int)strtotime($wDate);
	}

	//---------------------------------------------------------------
	// タイムスタンプを日付に変換
	// $pTime   ：int time()
	// 戻り値   ：YYYYMMDD
	//---------------------------------------------------------------
	function getHiDateFromTime($pTime) {
		return date("Ymd",$pTime);
	}

	//---------------------------------------------------------------
	// 指定日付と指定期間からを新たな日付を取得
	// $pHiDate ：YYYYMMDD
	// $pKikan  ：日数
	// 戻り値   ：YYYYMMDD
	//---------------------------------------------------------------
	function getDateByKikan($pHiDate,$pKikan) {
		$wTime = getTimeFromHiDate($pHiDate);
		return getHiDateFromTime(strtotime($pKikan." days",$wTime));
	}

	//---------------------------------------------------------------
	// 指定日付の一年前の日付を取得
	// $pHiDate ：YYYYMMDD
	// $pKikan  ：年数（省略は前年）
	// 戻り値   ：YYYYMMDD
	//---------------------------------------------------------------
	function getDateByYear($pHiDate,$pKikan="-1") {
		$wTime = getTimeFromHiDate($pHiDate);
		return getHiDateFromTime(strtotime($pKikan." year",$wTime));
	}

	//---------------------------------------------------------------
	// 末日を取得
	// $pHiDate ：YYYYMMDD
	// 戻り値   ：YYYYMMDD
	//---------------------------------------------------------------
	function getMatsubiFromHiDate($pHiDate) {
		return date("Ymt",getTimeFromHiDate($pHiDate));
	}

	//---------------------------------------------------------------
	// 日付編集
	// $pHiDate ：YYYYMMDD
	// $pPattern：各種
	// 戻り値   ：パターン編集後
	//---------------------------------------------------------------
	// 編集不可の場合はそのままで戻す
	//---------------------------------------------------------------
	function editDate($pHiDate,$pPattern) {
		if ( strlen($pHiDate) != 8 ) {
			return $pHiDate;
		}
		if ( !is_numeric($pHiDate) ) {
			return $pHiDate;
		}
		switch ($pPattern) {
			case "YYYYSMMSDD":
				return substr($pHiDate,0,4)."/".substr($pHiDate,4,2)."/".substr($pHiDate,6,2);
			case "YYSMMSDD":
				return substr($pHiDate,2,2)."/".substr($pHiDate,4,2)."/".substr($pHiDate,6,2);
			case "YYSMMSDDKW":
				$kw = getYoubiKJ( getYoubiFromHiDate($pHiDate) );
				return substr($pHiDate,2,2)."/".substr($pHiDate,4,2)."/".substr($pHiDate,6,2)."(".$kw.")";
			case "YYSMM":
				return substr($pHiDate,2,2)."/".substr($pHiDate,4,2);
			case "MMSDD":
				return substr($pHiDate,4,2)."/".substr($pHiDate,6,2);
			case "DD":
				return substr($pHiDate,6,2);
			case "YYDMMDDD":
				return substr($pHiDate,2,2).".".substr($pHiDate,4,2).".".substr($pHiDate,6,2);
			}
		return $pHiDate;
	}
	//---------------------------------------------------------------
	// タイムスタンプ編集
	//---------------------------------------------------------------
	function editTimestamp($pTime,$pPattern="YYSMMSDD") {
		if ( strlen($pTime) != 14 ) {
			return $pTime;
		}
		if ( !is_numeric($pTime) ) {
			return $pTime;
		}
		$wTime = substr($pTime,8,2).":".substr($pTime,10,2).":".substr($pTime,12);
		switch ($pPattern) {
			case "YYYYSMMSDD":
				return substr($pTime,0,4)."/".substr($pTime,4,2)."/".substr($pTime,6,2)." ".$wTime;
			case "YYSMMSDD":
				return substr($pTime,2,2)."/".substr($pTime,4,2)."/".substr($pTime,6,2)." ".$wTime;
			case "MMSDD":
				return substr($pTime,4,2)."/".substr($pTime,6,2)." ".$wTime;
		}
		return $pTime;
	}
	//---------------------------------------------------------------
	// タイムスタンプ編集
	// 引数にはDBから取得したタイムスタンプ
	//---------------------------------------------------------------
	function trimTimestamp($pTime,$pPattern="MMSDD") {
		if ( strlen($pTime) < 19 ) {
			return $pTime;
		}
		$wTime = substr($pTime,11,8);
		switch ($pPattern) {
			case "YYYYSMMSDD":
				return substr($pTime,0,4)."/".substr($pTime,5,2)."/".substr($pTime,8,2)." ".$wTime;
			case "YYSMMSDD":
				return substr($pTime,2,2)."/".substr($pTime,5,2)."/".substr($pTime,8,2)." ".$wTime;
			case "MMSDD":
				return substr($pTime,5,2)."/".substr($pTime,8,2)." ".$wTime;
			case "MMDD":
				return substr($pTime,5,2)."/".substr($pTime,8,2);
			case "HHCMM":
				return substr($pTime,11,2).":".substr($pTime,14,2).":".substr($pTime,17,2);
		}
		return $pTime;
	}
	//---------------------------------------------------------------
	// 時刻編集(Hi:SS)
	//---------------------------------------------------------------
	function editTime($pTime,$pPattern="HHCMM") {
		if ( !is_numeric($pTime) ) {
			return $pTime;
		}
		switch (strlen($pTime)) {
			case 14 :
				$wTime = substr($pTime,8);
				break;
			case 6 :
			case 4 :
				$wTime = $pTime;
				break;
			default :
				return $pTime;
		}
		switch ($pPattern) {
			case "HHCMMCSS" :
				return substr($wTime,0,2).":".substr($wTime,2,2).":".substr($wTime,4,2);
			default :
				return substr($wTime,0,2).":".substr($wTime,2,2);
		}
	}
	//---------------------------------------------------------------
	// 使用開始日の取得
	// 定数（SIYO_SDT_IS_TODAY）により当日か翌日かを戻す。
	//---------------------------------------------------------------
	function getSiyoKaisiDt($argToday) {
		if ( SIYO_SDT_IS_TODAY == "Y" ) {
			return $argToday;
		} else {
			return getDateByKikan($argToday,"+1");
		}
	}
	//---------------------------------------------------------------
	// 廃番日を編集
	//---------------------------------------------------------------
	function editHaibanDt($pHiDate,$pPattern) {
		if ( $pHiDate == DEF_HAIBAN_DT ) {
			return "";
		}
		if ( $pHiDate == "99999999" ) {
			return "";
		}
		return editDate($pHiDate,$pPattern);
	}
	//---------------------------------------------------------------
	// 廃番日を再現
	//---------------------------------------------------------------
	function getHaibanDt($pHiDate) {
		if ( $pHiDate == "" ) {
			return DEF_HAIBAN_DT;
		}
		return $pHiDate;
	}
	//---------------------------------------------------------------
	// 値配列の作成：期間（最大365個）
	//---------------------------------------------------------------
	function makeArrDates($argSdt,$argEdt) {
		$arr = array();
		// 無限ループ防止用
		if ( $argSdt > $argEdt ) {
			return false;
		}
		// 無限ループ防止用カウンタ
		$i=0;$iMax=365;
		// 開始日
		$wDt = $argSdt;
		//while ( $wDt < $argEdt ) {
		while ( $wDt <= $argEdt ) {
			$arr[]=$wDt;
			// +1日
			$wDt = getDateByKikan($wDt,"+1");
			// チェック
			$i++;
			if ( $i>$iMax ) {
				return false;
			}
		}
		return $arr;
	}
	//---------------------------------------------------------------
	// 値配列の作成：月度（最大84個）
	//---------------------------------------------------------------
	function makeArrGetsudo($argYMStart,$argYMEnd) {
		$arrGetsudo	= "";
		// 最低限のチェック
		if ( $argYMStart > $argYMEnd ) {
			return false;
		}
		if ( $argYMStart == "" ) {
			return false;
		}
		// 無限ループ防止用カウンタ
		$i=0;$iMax=84;
		// 生成
		$wYM		= $argYMStart;
		while ( $wYM <= $argYMEnd ) {
			$arrGetsudo[] 	= $wYM;
			$wTime			= getTimeFromHiDate($wYM."01");
			$wYM			= mb_substr(getHiDateFromTime(strtotime("+1 month",$wTime)),0,6);
			// チェック
			$i++;
			if ( $i>$iMax ) {
				return false;
			}
		}
		return $arrGetsudo;
	}
	//---------------------------------------------------------------
	// 期間の取得：開始日と終了日を取得
	// 使用方法：
	// 		list($sdt,$edt) = getKikan(基準日,前の日数,後の日数);
	//---------------------------------------------------------------
	function getKikan($argDt="",$argPre=7,$argPost=7) {
		// 入荷日期間の決定
		if ( $argDt == "" ) {
			$argDt = date('Ymd');
		}
		$wStartDt = getDateByKikan($argDt,"-".$argPre);
		$wEndDt   = getDateByKikan($argDt,$argPost);
		return array($wStartDt,$wEndDt);
	}
	//---------------------------------------------------------------
	// 回帰直線係数を取得
	// 使用方法：
	// 		getRegressionLine($arr)
	// 引数：
	// 		(x,y)の配列
	// 戻り値：
	// 		array(傾き,切片)
	//---------------------------------------------------------------
	function getRegressionLine($arr) {
		// 最低2要素必要
		if ( countAny($arr) < 2 ) {
			return false;
		}
		// 計算
		$wSumX = 0; $wSumY = 0; $wSumXY = 0; $wSumX2 = 0; $wSumY2 = 0;
		for ( $i=0; $i<count($arr); $i++ ) {
			$wSumX	+= $arr[$i][0];
			$wSumY	+= $arr[$i][1];
			$wSumXY	+= $arr[$i][0] * $arr[$i][1];
			$wSumX2	+= $arr[$i][0] * $arr[$i][0];
			$wSumY2	+= $arr[$i][1] * $arr[$i][1];
		}
		$wSlope		= ($wSumXY-$wSumX*$wSumY/count($arr))/($wSumX2-$wSumX*$wSumX/count($arr));
		$wIntercept	= ($wSumY-$wSumX*$wSlope)/count($arr);
		return array($wSlope,$wIntercept);
	}
	//---------------------------------------------------------------
	// 月間の日付リストを１テキストとして取得
	// 使用方法：
	// 		getDateLine($month) ... YYYYMM型
	//---------------------------------------------------------------
	function getDateLine($argMonth,$argPattern="DD") {
		$wStartDt	= $argMonth."01";
		$wEndDt		= getMatsubiFromHiDate($wStartDt);
		$arrDates	= makeArrDates($wStartDt,$wEndDt);
		$strLine	= "";
		for ( $i=0; $i<31; $i++ ) {
			if ( $i < count($arrDates) ) {
				$strLine .= mb_substr($arrDates[$i],6);
			} else {
				$strLine .= "00";
			}
		}
		return $strLine;
	}
	//---------------------------------------------------------------
	// 月間の曜日リストを１テキストとして取得
	// 使用方法：
	// 		getYoubiLine($month) ... YYYYMM型
	//---------------------------------------------------------------
	function getYoubiLine($argMonth) {
		$wStartDt	= $argMonth."01";
		$wEndDt		= getMatsubiFromHiDate($wStartDt);
		$arrDates	= makeArrDates($wStartDt,$wEndDt);
		$strLine	= "";
		for ( $i=0; $i<31; $i++ ) {
			if ( $i < count($arrDates) ) {
				$strLine .= getYoubiKj(getYoubiFromHiDate($arrDates[$i]));
			} else {
				$strLine .= "　";
			}
		}
		return $strLine;
	}
	//---------------------------------------------------------------
	// SQL：Where文の構築
	// $pItem  ：項目名
	// $pValue ：値
	// $pWhere ：元のwhere文
	// $pType1 ：クオーテーション："0"=なし,"1"=付与
	// $pLike  ：LIKE："R"=右,"L"=左,"BOTH"=両端
	// 戻り値  ：新しいwhere文
	//---------------------------------------------------------------
	function getWhere($pItem,$pValue,$pWhere,$pType1,$pLike) {
		$where = "";
		if ( $pWhere == "" ) {
			$where .= " where ";
		} else {
			$where = $pWhere . " and ";
		}
		if ( $pLike == "" ) {
			$eq   = " = ";
			$pstL = "";
			$pstR = "";
		} else if ( $pLike == "R" ) {
			$eq   = " like ";
			$pstL = "";
			$pstR = "%";
		} else if ( $pLike == "L" ) {
			$eq   = " like ";
			$pstL = "%";
			$pstR = "";
		} else if ( $pLike == "BOTH" ) {
			$eq   = " like ";
			$pstL = "%";
			$pstR = "%";
		}
		if ( $pType1 == "0" ) {
			$where .= $pItem . " = " . $pValue;
		} else {
			$where .= $pItem . $eq . "'" . $pstL . $pValue . $pstR . "'";
		}
		return $where;
	}

	//---------------------------------------------------------------
	// HTML：selected生成
	// $pLValue：リスト値
	// $pSValue：選択値
	// 戻り値  ："SELECTED"
	//---------------------------------------------------------------
	function isSelected($pLValue,$pSValue,$pStrict=false) {
		$selected = "";
		if ( $pStrict ) {
			if ( $pLValue === $pSValue ) {
				$selected = "SELECTED";
			}
		} else {
			if ( $pLValue == $pSValue ) {
				$selected = "SELECTED";
			}
		}
		return $selected;
	}
	function isChecked($pLValue,$pSValue,$pStrict=false) {
		$checked = "";
		if ( $pStrict ) {
			if ( $pLValue === $pSValue ) {
				$checked  = "CHECKED";
			}
		} else {
			if ( $pLValue == $pSValue ) {
				$checked  = "CHECKED";
			}
		}
		return $checked;
	}
	//---------------------------------------------------------------
	// HTML    ：disabled生成
	// $args   ：true/false
	// 戻り値  ："disabled" / ""
	//---------------------------------------------------------------
	function isDisabled($args) {
		if ( $args ) {
			return "disabled";
		}
		return "";
	}
	//---------------------------------------------------------------
	// 全角文字列桁そろえ（後方穴埋め）
	// $pStr  ：入力値
	// $pKeta ：桁数
	// $pChar ：穴埋め用の値
	// 戻り値 ：後方に指定値を穴埋めして戻す。
	//---------------------------------------------------------------
	function padKanji($pStr,$pKeta,$pChar="　") {
		return mb_substr($pStr.str_repeat($pChar,$pKeta),0,$pKeta);
	}

	//---------------------------------------------------------------
	// 数字桁そろえ
	// $pNo  ：番号入力値
	// $pKeta：桁数
	// 戻り値 ：指定桁前ZERO数値に編集して戻す。
	//---------------------------------------------------------------
	function trimCode($pNo,$pKeta) {
		$pNo = mb_convert_kana($pNo,"as");
		if ( strlen($pNo) > $pKeta ) {
			return substr($pNo,(strlen($pNo)-$pKeta));
		} elseif ( strlen($pNo) == $pKeta ) {
			return $pNo;
		} else {
			return str_pad($pNo,$pKeta,"0",STR_PAD_LEFT);
		}
	}

	//---------------------------------------------------------------
	// 個体識別桁そろえ
	// $pId：個体識別番号入力値
	// 戻り値 ：10桁前ZERO数値に編集して戻す。
	//---------------------------------------------------------------
	function trimKotaiId($pId) {
		return trimCode($pId,10);
	}
	//---------------------------------------------------------------
	// 個体識別チェック
	// $pId：個体識別番号入力値
	//---------------------------------------------------------------
	function checkKotaiId($pId) {
		$wCode = "253" . trimKotaiId($pId);
		$wSum  = 0;
		for ( $i=0 ; $i < (strlen($wCode)-1) ; $i++ ){
			if ( ($i%2)==0 ) {
				$w = 1;
			} else {
				$w = 3;
			}
			$wSum += substr($wCode,$i,1) * $w;
		}
		$wCheckDigit = ( 10 - $wSum % 10 ) %10;
		
		//echo("CODE=".$wCode." CD=".$wCheckDigit." SUM=".$wSum);

		if ( $wCheckDigit != substr($wCode,(strlen($wCode)-1),1) ) {
			return false;
		}
		return true;
	}

	//---------------------------------------------------------------
	// 計量器コード桁そろえ
	// $pKeiryoki：計量器コード入力値
	// 戻り値 ：3桁前ZERO数値に編集して戻す。
	//---------------------------------------------------------------
	function trimKeiryoki($pKeiryoki) {
		$pKeiryoki = mb_convert_kana($pKeiryoki,"as");
		if ( strlen($pKeiryoki) > 3 ) {
			return substr($pKeiryoki,(strlen($pKeiryoki)-4));
		} elseif ( strlen($pKeiryoki) == 3 ) {
			return $pKeiryoki;
		} else {
			return str_pad($pKeiryoki,3,"0",STR_PAD_LEFT);
		}
	}

	//---------------------------------------------------------------
	// 担当者コード桁そろえ
	// $pTanto：担当者コード入力値
	// 戻り値 ：4桁前ZERO数値に編集して戻す。
	//---------------------------------------------------------------
	function trimTanto($pTanto) {
		$pTanto = mb_convert_kana($pTanto,"as");
		if ( strlen($pTanto) > 4 ) {
			return substr($pTanto,(strlen($pTanto)-4));
		} elseif ( strlen($pTanto) == 4 ) {
			return $pTanto;
		} else {
			return str_pad($pTanto,4,"0",STR_PAD_LEFT);
		}
	}

	//---------------------------------------------------------------
	// 酒類コード桁そろえ
	// $pSakeCd：酒類コード入力値
	// 戻り値  ：2桁前ZERO数値に編集して戻す。
	//---------------------------------------------------------------
	function trimSakeBunCd($pSakeCd) {
		$pSakeCd = mb_convert_kana($pSakeCd,"as");
		if ( strlen($pSakeCd) > 2 ) {
			return $pSakeCd;
		} elseif ( strlen($pSakeCd) == 2 ) {
			return $pSakeCd;
		} elseif ( strlen($pSakeCd) == 1 ) {
			return "0".$pSakeCd;
		} else {
			return $pSakeCd;
		}
	}

	//---------------------------------------------------------------
	// バーコードよりゴンドラ値抽出
	// $pVal：入力値
	// 戻り値：ゴンドラ値
	//---------------------------------------------------------------
	function trimGondolaFromEAN($pVal) {
		if ( strlen($pVal) <= 7 ) {
			return $pVal;
		}
		// C/D除外
		$wVal = substr($pVal,0,(strlen($pVal)-1));
		// 前2桁は除外
		return substr($wVal,2);
	}

	//---------------------------------------------------------------
	// コード補正
	// ※コード指定が無い場合に代替値で補正する
	// 　　⇒第二引数を適用する（そこに代替値を指定）
	//---------------------------------------------------------------
	function reviseCd($argCd,$argDefCd="") {
		if ( $argCd == "" ) {
			return $argDefCd;
		}
		return $argCd;
	}

	//---------------------------------------------------------------
	// シンプルバーコード用コード生成
	// $pVal：入力値
	// 戻り値 ：前ZEROパディング後コード生成して戻す。
	//---------------------------------------------------------------
	function trimEAN8($pVal) {
		if ( strlen($pVal) > 8 ) {
			return substr($pVal,(strlen($pVal)-8));
		} elseif ( strlen($pVal) < 7 ) {
			$pVal = str_pad($pVal,7,"0",STR_PAD_LEFT);
		}
		return getEANShort(getEANCode($pVal));
	}

	//---------------------------------------------------------------
	// メーカーコード取り出し
	//---------------------------------------------------------------
	function getMakerCd($pVal) {
		$wLen = strlen($pVal);
		// 8桁と13桁にトリミング
		if ( $wLen < 8 ) {
			$pVal = str_pad($pVal,8,"0",STR_PAD_LEFT);
		} elseif ( $wLen > 8 && $wLen < 13 ) {
			$pVal = str_pad($pVal,13,"0",STR_PAD_LEFT);
		}
		// 8桁の場合
		if ( strlen($pVal) == 8 ) {
			if ( substr($pVal,0,2) == "02" ) {
				return DAIHYO_CD;
			} else {
				return substr($pVal,0,6);
			}
		// 13桁の場合は7桁型と9桁型を分別
		} else {
			if ( substr($pVal,0,2) == "02" ) {
				return DAIHYO_CD;
			} elseif ( substr($pVal,0,2) >= "20" && substr($pVal,0,2) <= "29" ) {
				return DAIHYO_CD;
			} elseif ( substr($pVal,0,3) >= "456" && substr($pVal,0,3) < "459" ) {
				return substr($pVal,0,9);
			} else {
				return substr($pVal,0,7);
			}
		}
	}
	//---------------------------------------------------------------
	// $pItemKbn：int 商品区分（0:発注対象外 1:EOS 2:FAX）
	// 戻り値   ：商品区分名
	//---------------------------------------------------------------
	function getItemKbnNm($pItemKbn) {
		switch ($pItemKbn) {
			case "0":
				return "対象外";
			case "1":
				return "EOS";
			case "2":
				return "FAX";
			case "6":
				return "検品";
		}
		return "未定義";
	}
	//---------------------------------------------------------------
	// $pItemKbn：char 商品区分
	// 戻り値   ：商品区分名
	//---------------------------------------------------------------
	function canOrderByItemKbn($argOrderItemKbns,$argItemKbn) {
		return strstr($argOrderItemKbns,$argItemKbn);
	}
	//---------------------------------------------------------------
	// $pTokuKbn：int 定番特売区分（0:定番 1:指定 4:企画）
	// 戻り値   ：定番特売区分名
	//---------------------------------------------------------------
	function getTokuKbnNm($pTokuKbn) {
		switch ($pTokuKbn) {
			case "0":
				return "定番";
			case "1":
				return "指定";
			case "4":
				return "企画";
			case "5":
				return "本部企画";
		}
		return "未定義";
	}
	//---------------------------------------------------------------
	// $pTokuSbts：特売種別の羅列からin句用に生成
	// 戻り値   ：in句用の文字列
	//---------------------------------------------------------------
	function getTokuSbtIn($pTokuSbts) {
		$_str = "";
		for ( $_idx=0 ; $_idx<strlen($pTokuSbts) ; $_idx++ ) {
			if ( $_idx > 0 ) {
				$_str .= ",";
			}
			$_str .= "'".substr($pTokuSbts,$_idx,1)."'";
		}
		return $_str;
	}
	//---------------------------------------------------------------
	// $pKeppinKbn：str 欠品区分（00:欠品なし）
	// 戻り値   ：定番特売区分名
	//---------------------------------------------------------------
	function getKeppinKbnNm($pKeppinKbn) {
		switch ($pKeppinKbn) {
			case "00":
				return "欠品なし";
			case "02":
				return "廃番商品";
			case "03":
				return "発注ミス";
			default :
				return "在庫切れ";
		}
		return "未定義";
	}
	//---------------------------------------------------------------
	// $pUnit：発注単位等
	// $pBowlIrisu：ボール入数
	// 戻り値：単位区分
	//---------------------------------------------------------------
	function getUnitKbn($pUnit,$pBowlIrisu="",$pCaseIrisu="") {
		$wUnit      = intval($pUnit);
		if ( isZero($wUnit) ) {
			return UNIT_KBN_BARA;
		}
		if ( $wUnit >=1 && $wUnit <= 1 ) {
			return UNIT_KBN_BARA;
		}
		if ( $pCaseIrisu != "" ) {
			if ( $wUnit == intval($pCaseIrisu) ) {
				return UNIT_KBN_CASE;
			}
		}
		if ( $pBowlIrisu != "" ) {
			if ( $wUnit == intval($pBowlIrisu) ) {
				return UNIT_KBN_BOWL;
			}
		}
		return UNIT_KBN_CASE;
	}
	//---------------------------------------------------------------
	// 漢字エリアトリミング
	//---------------------------------------------------------------
	function trimKanji($pStr) {
		$buf    = $pStr;
		// 前全角スペースを外す（この時点で全部スペースも処理される）
		$lenStr = strlen($buf);
		while ( substr($buf,0,1) == "　" ) {
			$buf = substr($buf,1);
		}
		// 後ろ全角スペースを外す
		$lenStr = strlen($buf);
		for ( $i=$lenStr; $i>0; $i-- ) {
			if (substr($buf,$i-1,1) == "　") {
				$buf = substr($buf,0,$i-1);
			} else {
				break;
			}
		}
		return $buf;
	}
	//---------------------------------------------------------------
	// 品名漢字編集
	//---------------------------------------------------------------
	function editItemNmKj($pMaker,$pItemNm,$pItemKikaku,$pLen) {
		$str_MAKER_ITEM_KIKAKU = $pItemNm;
		$str_ITEM_KIKAKU       = $pItemNm;
		// まずは単純に結合
		if ( $pItemKikaku != "" ) {
			$str_MAKER_ITEM_KIKAKU .= "　".$pItemKikaku;
			$str_ITEM_KIKAKU       .= "　".$pItemKikaku;
		}
		if ( $pMaker != "" ) {
			$str_MAKER_ITEM_KIKAKU = $pMaker."　".$str_MAKER_ITEM_KIKAKU;
		}
		// メーカー＋品名＋規格名が収まっていれば採用
		$len = mb_strlen($str_MAKER_ITEM_KIKAKU);
		if ( $len <= $pLen ) {
			return $str_MAKER_ITEM_KIKAKU;
		}
		// 品名＋規格名が収まっていれば採用
		$len = mb_strlen($str_ITEM_KIKAKU);
		if ( $len <= $pLen ) {
			return $str_ITEM_KIKAKU;
		}
		return mb_substr($str_ITEM_KIKAKU,0,$pLen);
	}
	//---------------------------------------------------------------
	// 品名カナ編集
	//---------------------------------------------------------------
	function editItemNmKn($pMaker,$pItemNm,$pItemKikaku,$pLen) {
		$str_MAKER_ITEM_KIKAKU = $pItemNm;
		$str_ITEM_KIKAKU       = $pItemNm;
		// まずは単純に結合
		if ( $pItemKikaku != "" ) {
			$str_MAKER_ITEM_KIKAKU .= " ".$pItemKikaku;
			$str_ITEM_KIKAKU       .= " ".$pItemKikaku;
		}
		if ( $pMaker != "" ) {
			$str_MAKER_ITEM_KIKAKU = $pMaker." ".$str_MAKER_ITEM_KIKAKU;
		}
		// メーカー＋品名＋規格名が収まっていれば採用
		$len = mb_strlen($str_MAKER_ITEM_KIKAKU);
		if ( $len <= $pLen ) {
			return $str_MAKER_ITEM_KIKAKU;
		}
		// 品名＋規格名が収まっていれば採用
		$len = mb_strlen($str_ITEM_KIKAKU);
		if ( $len <= $pLen ) {
			return $str_ITEM_KIKAKU;
		}
		return mb_substr($str_ITEM_KIKAKU,0,$pLen);
	}
	//---------------------------------------------------------------
	// 品名漢字をメーカー名/商品呼称/商品規格名に分ける
	//---------------------------------------------------------------
	function splitItemNmKj($pItemNm) {
		// 初期化
		$arr[] = ""; $arr[] = $pItemNm; $arr[] = "";
		// 全角空白で分ける
//		$buf   = explode("　",$pItemNm);			// 日本語（漢字）で正しく動作しない場合あり
		$buf   = mb_split("　",$pItemNm);
		// 単語数で判定
		$numWords = count($buf);
		switch ( $numWords ) {
			case 0 :
			case 1 :
				break;
			case 2 :
				$arr[1] = $buf[0];
				$arr[2] = $buf[1];
				break;
			default :
				$arr[0] = $buf[0];
				$arr[1] = "";
				for ( $i=1;$i<($numWords-1);$i++) {
					$arr[1] .= $buf[$i];
				}
				$arr[2] = $buf[$numWords-1];
				break;
		}
		return $arr;
	}
	//---------------------------------------------------------------
	// ボタントリミング
	//---------------------------------------------------------------
	function trimButton($pButton) {
		$wButton = str_replace(" ","",$pButton);
		$wButton = str_replace("　","",$wButton);
		return $wButton;
	}
	//---------------------------------------------------------------
	// 入数の設定
	//---------------------------------------------------------------
	function setIrisuByKigyo($pFlg,$pIrisu,$pMinOrder) {
		$wIrisu = $pIrisu;
		if ( $pFlg == "1" && $pMinOrder != "" ) {
			$wIrisu = $pMinOrder;
		}
		return $wIrisu;
	}
	//---------------------------------------------------------------
	// 入数の設定
	//---------------------------------------------------------------
	function calcIrisu($pBara,$pSuryo) {
		if ( !is_numeric($pBara) || !is_numeric($pSuryo) ) {
			return 1;
		}
		if ( $pSuryo == 0 ) {
			return 1;
		}
		return $pBara/$pSuryo;
	}
	//---------------------------------------------------------------
	// 発注数への小数入力対応
	// 　　受注単位区分=X（混合型）の場合に、
	// 　　数量と重量をプラスして求める
	// ※2013/12/24 （by inok）
	//---------------------------------------------------------------
	function getSuryoByUnitKbn($pUnitKbn="",$pSuryo=0,$pJuryo=0) {
		if ( $pUnitKbn != "X" ) {
			return $pSuryo;
		}
		return ($pSuryo+$pJuryo);
	}
	function getBaraSuryoByUnitKbn($pUnitKbn="",$pSuryo=0,$pJuryo=0,$pOrderUnit=1) {
		if ( $pUnitKbn != "X" ) {
			return ($pSuryo*$pOrderUnit);
		}
		return ($pSuryo+$pJuryo)*$pOrderUnit;
	}
	//---------------------------------------------------------------
	// 端数処理(マイナス対応)
	//	2013/03/05 PHP誤差対応の精度向上（by inok）
	//	2015/07/20 ゼロ判定補完（by inok）
	//  2019/07/30 実数マイナスの扱いを変更（by waka）
	//---------------------------------------------------------------
	function trimHasu($pGaku,$pHasuKbn) {
		// 2013-02-14 比較方法を元に戻す(なぜ変更したか不明)
		//if ( $pGaku === 0 ) {
		//	return 0;
		//}
		$wAbs  = abs($pGaku);
		// 2015-07-20 数値ゼロ判定の補完
		if ( $wAbs <= 0 ) {
			return 0;
		}
		//$wFugo = round($pGaku/$wAbs);
		$wFugo = (0 <= $pGaku ? 1 : -1);
		switch ( $pHasuKbn ) {
			case "2" :	// 四捨五入
				return round($wAbs)*$wFugo;
			case "3" :	// 切上げ
				// 2013-03-05 PHP誤差対応の精度向上（1ケタ⇒6ケタ）
				//return ceil(round($wAbs,1))*$wFugo;
				return ceil(round($wAbs,5))*$wFugo;
			default :	// 切捨て
				// 2013-03-05 PHP誤差対応の精度向上（1ケタ⇒5ケタ）
				//return floor(round($wAbs,1))*$wFugo;
				return floor(round($wAbs,5))*$wFugo;
		}
	}
	//---------------------------------------------------------------
	// 端数処理リバース(マイナス対応)
	//	2013/03/05 PHP誤差対応の精度向上（by inok）
	//  2019/07/30 実数マイナスの扱いを変更（by waka）
	//---------------------------------------------------------------
	function trimHasuR($pGaku,$pHasuKbn) {
		//if ( $pGaku === 0 ) {
		//	return 0;
		//}
		$wAbs  = abs($pGaku);
		// $wFugo = round($pGaku/$wAbs);
		$wFugo = (0 <= $pGaku ? 1 : -1);
		switch ( $pHasuKbn ) {
			case "2" :	// 四捨五入
				return round($wAbs)*$wFugo;
			case "3" :	// 通常切上げの場合は切捨て
				// 2013-03-05 PHP誤差対応の精度向上（1ケタ⇒5ケタ）
				//return floor(round($wAbs,1))*$wFugo;
				return floor(round($wAbs,5))*$wFugo;
			default :	// 通常切捨ての場合は切上げ
				// 2013-03-05 PHP誤差対応の精度向上（1ケタ⇒5ケタ）
				//return ceil(round($wAbs,1))*$wFugo;
				return ceil(round($wAbs,5))*$wFugo;
		}
	}
	//---------------------------------------------------------------
	// 原価金額の端数処理
	//---------------------------------------------------------------
	function getGenkaGaku($pTanka,$pSuryo,$pHasuKbn) {
		if ( floatval($pTanka*$pSuryo) === 0.0 ) {
			return 0;
		}
		return trimHasu($pTanka*$pSuryo,$pHasuKbn);
/*	マイナスを考慮していないためtrimHasu()を使用 (2012/01/10 Kurogi)
		switch ( $pHasuKbn ) {
			case "2" :	// 四捨五入
				return round($pTanka*$pSuryo);
			case "3" :	// 切上げ
				return ceil(round($pTanka*$pSuryo,1));
			default :	// 切捨て
				return floor(round($pTanka*$pSuryo,1));
		}
*/
	}
	//---------------------------------------------------------------
	// 端数処理
	//---------------------------------------------------------------
	function getTax($pGaku,$pTaxRate,$pHasuKbn) {
		switch ( $pHasuKbn ) {
			case "2" :	// 四捨五入
				return round(($pGaku*$pTaxRate)/(1+$pTaxRate));
			case "3" :	// 切上げ
				return ceil(($pGaku*$pTaxRate)/(1+$pTaxRate));
			default :	// 切捨て
				return floor(($pGaku*$pTaxRate)/(1+$pTaxRate));
		}
	}
	//---------------------------------------------------------------
	// 配列からCSV行を作成
	//---------------------------------------------------------------
	function arr2csv($pArr,$pChar="",$pMode="") {
		$wCsv = "";
		switch ($pMode) {
			case "keys" :
				$wArr = array_keys($pArr);
				break;
			case "values" :
				$wArr = array_values($pArr);
				break;
			default :
				$wArr = $pArr;
				break;
		}
		foreach ($wArr as $val) {
			if ( $wCsv == "" ) {
				$wCsv .= $pChar.$val.$pChar;
			} else {
				$wCsv .= ",".$pChar.$val.$pChar;
			}
		}
		return $wCsv;
	}
	//---------------------------------------------------------------
	// KEY配列からinsert文を作成
	//---------------------------------------------------------------
	function arr2insert($pTable,$pArr) {
        foreach( $pArr as $key => $val ){
            $keys[]   = $key;
            $values[] = $val;
        }
        $sql  = "insert into ".$pTable."(";
        $sql .= implode( ',', $keys );
        $sql .= ") values (";
        $sql .= implode( ',', $values );
        $sql .= ")";
        return $sql;
    }
	//---------------------------------------------------------------
	// KEY配列からset文を作成
	//---------------------------------------------------------------
	function arr2set($pArr) {
        // $key=$val の文字列へ変換する
        foreach( $pArr as $key => $val ){
            $buf[] = $key."=".$val;
        }
        $str = implode( ',', $buf );
        return $str;
    }
	//---------------------------------------------------------------
	// KEY配列からin句を作成
	//---------------------------------------------------------------
	function arr2in($pArr,$arg2="'") {
        for ( $i=0;$i<countAny($pArr);$i++ ){
            $buf[] = "'".$pArr[$i]."'";
        }
        $str = implode( ',', $buf );
        return $str;
    }
	//---------------------------------------------------------------
	// KEY配列からwhere句を作成
	//---------------------------------------------------------------
	function arr2where($pArr) {
        $str = implode( ' and ', $pArr );
        $str = " where ".$str;
        return $str;
    }
	//---------------------------------------------------------------
	// KEY配列からQUERYパラメータを作成
	// 2011.04.24 空値を設定するかを第二引数に指定可能とする
	//---------------------------------------------------------------
	function arr2query($pArr,$pNullYN=true) {
		// KEY配列がない場合は何もしない
		if ( $pArr == "" ) { return ""; }
        // $key=$val の文字列へ変換する
        foreach( $pArr as $key => $val ){
			if ( $pNullYN ) {
				// NULL値の場合もセット
				$buf[] = $key."=".$val;
			} else {
				// NULL値の場合はセットしない
				if ( $val != "" ) {
					$buf[] = $key."=".$val;
				}
			}
        }
        $str = implode( '&', $buf );
        return $str;
    }
	//---------------------------------------------------------------
	// 配列からSQL格納用の配列(1次元)へ
	// ※戻り値は、{"val","val","val","val"}
	// ※ダブルクォーテーションを外す場合は第二引数のタイプで「N」指定
	//---------------------------------------------------------------
	function arr2SqlArray($argArray,$argType="C") {
		$wArray = array_values($argArray);
		if ( $argType == "C" ) {
			$str = '{"'.implode('","',$wArray).'"}';
		} else {
			$str = '{'.implode(',',$wArray).'}';
		}
		return $str;
	}
	//---------------------------------------------------------------
	// SQL配列から通常配列へ（一次元用）
	//---------------------------------------------------------------
	function arrFromSqlArray($argArray,$argTrim="{}") {
		$wArray = trim($argArray,$argTrim);
		if ( $wArray != "" ) {
			$wArray = explode(",",$wArray);
		}
		return $wArray;
	}
	//---------------------------------------------------------------
	// 関連のある2つの配列から値を取り出す
	//---------------------------------------------------------------
	function getValueFromArray($pStr,$pArr,$pArr2,$pStrict=false) {
		for ( $ix=0 ; $ix<count($pArr) ; $ix++ ) {
			if ( $pStrict ) {
				if ( $pStr === $pArr[$ix] ) {
					return $pArr2[$ix];
				}
			} else {
				if ( $pStr == $pArr[$ix] ) {
					return $pArr2[$ix];
				}
			}
		}
		return "";
	}
	//---------------------------------------------------------------
	// 配列からKEY値の添字を取り出す
	//---------------------------------------------------------------
	function getIndexFromArray($pStr,$pArr,$pStrict=false) {
		for ( $ix=0 ; $ix<count($pArr) ; $ix++ ) {
			if ( $pStrict ) {
				if ( $pStr === $pArr[$ix] ) {
					return $ix;
				}
			} else {
				if ( $pStr == $pArr[$ix] ) {
					return $ix;
				}
			}
		}
		return "";
	}
	//---------------------------------------------------------------
	// 権限チェック
	// 第一引数に画面等の必要権限、第二引数にユーザの所持権限をセット
	// 必要権限側がONにも関わらず、所持権限がOFFの場合には不可とする
	//---------------------------------------------------------------
	function checkKengen($pReqKengen,$pUserKengen) {
		for ( $ix=0 ; $ix<strlen($pReqKengen) ; $ix++ ) {
			if ( substr($pReqKengen,$ix,1) == "0" ) {
				continue;
			}
			if ( strlen($pUserKengen) <= $ix ) {
				return false;
			}
			if ( substr($pReqKengen,$ix,1) > substr($pUserKengen,$ix,1) ) {
				return false;
			}
		}
		return true;
	}
	//---------------------------------------------------------------
	// HTML表示用
	//---------------------------------------------------------------
	function h($pStr) {
		return htmlentities($pStr);
	}
	// JavaScriptの値として使用するため
	function trimBoolean($pBool) {
		if ( $pBool ) {
			return "true";
		} else {
			return "false";
		}
	}
	function getLineColor($pIndex,$pCl1,$pCl2) {
		if ( ($pIndex%2)==0 ) {
			return $pCl1;
		} else {
			return $pCl2;
		}
	}
	function wrap($pStr,$pWrapper) {
		if ( $pStr == "" ) {
			return "";
		}
		switch ($pWrapper) {
			case "()":
				return "(".$pStr.")";
			case "{}":
				return "{".$pStr."}";
			case "--":
				return "-".$pStr."-";
			case "[]":
				return "[".$pStr."]";
			default:
				if ( strlen($pWrapper) == 2 ) {
					return substr($pWrapper,0,1).$pStr.substr($pWrapper,1);
				}
		}
		return $pStr;
	}
	function getFocusedClass($argNm,$argValue,$argClassNm) {
		// if ( $argNm == $argValue ) {
		if ( strstr($argValue,$argNm) ) {
			return $argClassNm;
		}
		return "";
	}
	function addATag($argStr,$argLink,$argTarget="") {
		if ( $argLink == "" ) {
			return $argStr;
		} else {
			if ( $argTarget == "" ) {
				return "<a href='".$argLink."'>".$argStr."</a>";
			} else {
				return "<a href='".$argLink."' target='".$argTarget."'>".$argStr."</a>";
			}
		}
	}
	//---------------------------------------------------------------
	// makeLink
	// 　リンク編集
	// by inok
	// PARM1：URL
	// PARM2：パラメータ
	//---------------------------------------------------------------
	function makeLink($argUrl,$argParam) {
		$wLink = $argUrl;
		if ( strpos($wLink,"?") ) {
			$wLink .= "&".$argParam;
		} else {
			$wLink .= "?".$argParam;
		}
		return $wLink;
	}
	//---------------------------------------------------------------
	// popup
	// 　ポップアップメッセージ作成
	// by inok
	// PARM1：表示文
	// PARM2：POPUP文
	//---------------------------------------------------------------
	function makePopup($arg1,$arg2) {
		$wPopup_atag_s = "<a class='popup' href='#'>";
		$wPopup_span_s = "<span class='popupBody'>";
		$wPopup_angle  = "<span class='popupAngle'><span class='popupAngleInner'></span></span>";
		$wPopup_span_e = "</span>";
		$wPopup_atag_e = "</a>";

		$wPopup = $wPopup_atag_s . $arg1 . $wPopup_span_s . $arg2 . $wPopup_angle . $wPopup_span_e . $wPopup_atag_e;

		return $wPopup;
	}
	//---------------------------------------------------------------
	// サブメニュー制御
	// 　現在のURLをメニューURLと比較し表示中か否かの判定を行う。
	// 　表示中であればリンクをOFF、そうでなければリンクをONにする。
	// by inok
	// PARM1：現在のURL
	// PARM2：メニューURL
	// PARM3：メニュー名
	//---------------------------------------------------------------
	function editNavi($argThisUrl,$argUrl,$argNm) {
		if ( strpos($argUrl,$argThisUrl)!==false ) {
		//if ( $argThisUrl == $argUrl ) {
			return "<span>".$argNm."</span>";
		} else {
			return "<a href='".$argUrl."'>".$argNm."</a>";
		}
	}
	//---------------------------------------------------------------
	// URLスタック
	// 　現在のURLを直前のURL($_SESSION["THIS_URL"])とひかくし
	// 　異なっていればURLをスタックする。
	// 　４段までの戻りに対応
	// by inok
	//---------------------------------------------------------------
	function pushUrl($argParams) {
		//echo("PREV_URL=".$_SESSION["THIS_URL"]." NEW=".$_SERVER["REQUEST_URI"]."<br/>");
		// URL部抽出
		$wUrl = explode("?",$_SERVER["REQUEST_URI"]);
		// 処理区分が更新系又は帳票系の場合はスタック不要
		switch($argParams["P_SKBN"]) {
			case "REG" :
			case "DEL" :
			case "PDF" :
			case "OBOOK" :
			case "CHKPDF" :
			case "ALLPDF" :
			case "CSVKR"  :
			case "CSVAC"  :
			case "DL"  :
			case "EXCEL" :
			case "KAKUTEI" :
			case "CLOSE" :
			case "DOWNLOAD" :
			case "OUTPUT" :
			case "COPY" :
			case "COPYALL" :
			case "COPYTITLE" :
			case "REGHEAD" :
			case "DELHEAD" :
			case "SEND" :
				return false;
		}
		// 直前URLと変更が無い場合はQUERYのみを保存
		if ( strpos($wUrl[0],$_SESSION["THIS_URL"])!==false ) {
			$_SESSION["THIS_QUERY"] = makeLink($wUrl[0],arr2query($argParams));
			//echo("No move URL=".$_SESSION["THIS_URL"]." QUERY=".$_SESSION["THIS_QUERY"]);
			return true;
		}
		//--------------------------------------
		// 直前URLと変更がある場合はスタック
		// 但し、スタック済みのURLに自身のURLがある場合は戻りとみなす
		//--------------------------------------
		//echo("Moving NOW=".$wUrl[0]." PREV=".$_SESSION["THIS_URL"]." LAST=".$_SESSION["LAST_URL"]."<br/>");
		// 2013-02-14 strposのwarning回避のため
		if ( $_SESSION["LAST_URL"]!="" && strpos($wUrl[0],$_SESSION["LAST_URL"])!==false ) {
		//if ( strpos($wUrl[0],$_SESSION["LAST_URL"])!==false ) {
			//echo("is Back! NOW=".$wUrl[0]." PREV=".$_SESSION["THIS_URL"]." LAST=".$_SESSION["LAST_URL"]." LAST2=".$_SESSION["LAST2_URL"]." LAST3=".$_SESSION["LAST3_URL"]." LAST4=".$_SESSION["LAST4_URL"]."<br/>");
			$_SESSION["LAST_URL"]    = $_SESSION["LAST2_URL"];
			$_SESSION["LAST_QUERY"]  = $_SESSION["LAST2_QUERY"];
			$_SESSION["LAST2_URL"]   = $_SESSION["LAST3_URL"];
			$_SESSION["LAST2_QUERY"] = $_SESSION["LAST3_QUERY"];
			$_SESSION["LAST3_URL"]   = $_SESSION["LAST4_URL"];
			$_SESSION["LAST3_QUERY"] = $_SESSION["LAST4_QUERY"];
			$_SESSION["LAST4_URL"]   = "";
			$_SESSION["LAST4_QUERY"] = "";
			$_SESSION["THIS_URL"]    = $wUrl[0];
			$_SESSION["THIS_QUERY"]  = makeLink($wUrl[0],arr2query($argParams));
		} else {
			//echo("is New! NOW=".$wUrl[0]." PREV=".$_SESSION["THIS_URL"]." LAST=".$_SESSION["LAST_URL"]." LAST2=".$_SESSION["LAST2_URL"]." LAST3=".$_SESSION["LAST3_URL"]." LAST4=".$_SESSION["LAST4_URL"]."<br/>");
			$_SESSION["LAST4_URL"]   = $_SESSION["LAST3_URL"];
			$_SESSION["LAST4_QUERY"] = $_SESSION["LAST3_QUERY"];
			$_SESSION["LAST3_URL"]   = $_SESSION["LAST2_URL"];
			$_SESSION["LAST3_QUERY"] = $_SESSION["LAST2_QUERY"];
			$_SESSION["LAST2_URL"]   = $_SESSION["LAST_URL"];
			$_SESSION["LAST2_QUERY"] = $_SESSION["LAST_QUERY"];
			$_SESSION["LAST_URL"]    = $_SESSION["THIS_URL"];
			$_SESSION["LAST_QUERY"]  = $_SESSION["THIS_QUERY"];
			$_SESSION["THIS_URL"]    = $wUrl[0];
			$_SESSION["THIS_QUERY"]  = makeLink($wUrl[0],arr2query($argParams));
		}
		//echo("pushed! NOW=".$_SESSION["THIS_URL"]." LAST=".$_SESSION["LAST_URL"]." LAST2=".$_SESSION["LAST2_URL"]."<br/>");
		return true;
	}
	function popUrl($argUrl) {
		if ( $_SESSION["LAST_QUERY"] != "" ) {
			return $_SESSION["LAST_QUERY"];
		}
		return $argUrl;
	}
	function clearUrl() {
		// URL部抽出
		$wUrl = explode("?",$_SERVER["REQUEST_URI"]);
		$_SESSION["LAST4_URL"]   = "";
		$_SESSION["LAST4_QUERY"] = "";
		$_SESSION["LAST3_URL"]   = "";
		$_SESSION["LAST3_QUERY"] = "";
		$_SESSION["LAST2_URL"]   = "";
		$_SESSION["LAST2_QUERY"] = "";
		$_SESSION["LAST_URL"]    = "";
		$_SESSION["LAST_QUERY"]  = "";
		$_SESSION["THIS_URL"]    = $wUrl[0];
		$_SESSION["THIS_QUERY"]  = makeLink($wUrl[0],arr2query($argParams));
		return true;
	}
	function getTenpoCdWorking($argParams) {
		if ( $argParams["P_TENPO_CD"] != "" ) {
			$_SESSION["TENPO_CD_WORKING"] = $argParams["P_TENPO_CD"];
		} else {
			if ( $_SESSION["TENPO_CD_WORKING"] == "" ) {
				$_SESSION["TENPO_CD_WORKING"] = $_SESSION["TENPO_CD"];
			}
		}
		return $_SESSION["TENPO_CD_WORKING"];
	}
	//---------------------------------------------------------------
	// テスト中かの判定
	// 　$_SESSION["TEST_MODE"]によりテスト中か否かの判定を行い
	// 　テスト中であれば、「テスト」を表示
	// by inok
	//---------------------------------------------------------------
	function isTest($argParm) {
		if ( $argParm == "Y" ) {
			return true;
		}
		return false;
	}
	function dispTest($argSession,$argParm="TEST_MODE",$argDisp="テスト") {
		if ( $argSession[$argParm] ) {
			return $argDisp;
		} else {
			return "";
		}
	}
	//---------------------------------------------------------------
	// 部門コード取得
	// 引数　　：$argBunKbn：分類区分
	// 　　　　：$argParm  ：部門コードが格納されている配列
	// 戻り値　：部門コード
	// 処理内容：分類区分に応じた部門コードを戻す
	//---------------------------------------------------------------
	function getBunCdByBunKbn($argBunKbn,$argParm) {
		switch ( $argBunKbn ) {
			case BUN_KBN_DAI :
				return $argParm["大分類コード"];
			case BUN_KBN_CHU :
				return $argParm["中分類コード"];
			case BUN_KBN_SHO :
				return $argParm["小分類コード"];
			case BUN_KBN_SAI :
				if ( $argParm["細分類コード"] != "" ) {
					return $argParm["細分類コード"];
				} else {
					return $argParm["分類コード"];
				}
			default :
				return $argParm["分類コード"];
		}
	}
	//---------------------------------------------------------------
	// 分類マスタカラム名
	// 引数　　：$argBunKbn：分類区分
	// 戻り値　：カラム名
	// 処理内容：分類区分に応じたカラム名を戻す
	//---------------------------------------------------------------
	function getBunColumnNm($argBunKbn) {
		switch ( $argBunKbn ) {
			case BUN_KBN_DAI :
				return "大分類コード";
			case BUN_KBN_CHU :
				return "中分類コード";
			case BUN_KBN_SHO :
				return "小分類コード";
			case BUN_KBN_SAI :
				return "細分類コード";
			default :
				return "";
		}
	}

	//---------------------------------------------------------------
	// 閏年補正（２月末日取得用）
	// 　日付(YYYYMMDD)の月日が2月28日～3月1日の時、その年が閏年なら
	// 　2月29日に補正する。
	// 引数　　：$argDate：日付(YYYYMMDD)
	// 戻り値　：補正された日付
	//---------------------------------------------------------------
	function leapCorrect($argDate) {
		$wYear  = substr( $argDate, 0, 4 );
		$wMonth = substr( $argDate, 4, 2 );
		$wDay   = substr( $argDate, 6, 2 );
		if ( $wMonth == "02" ) {
			if ( ( $wDay == "28" ) || ( $wDay == "29" ) ) {
				return date("Ymd", mktime( 0, 0, 0, 3, 0, $wYear ) );
			} else {
				return $argDate;
			}
		} else if ( $wMonth == "03" ) {
			if ( $wDay == "01" ) {
				return date("Ymd", mktime( 0, 0, 0, 3, 0, $wYear ) );
			} else {
				return $argDate;
			}
		} else {
			return $argDate;
		}
	}

	//---------------------------------------------------------------
	// checkMailAddress
	//---------------------------------------------------------------
	function checkMailAddress($argAddr) {
		if ( preg_match('/^([a-z0-9_]|\-|\.|\+)+@(([a-z0-9_]|\-)+\.)+[a-z]{2,6}$/i',$argAddr) ) {
			return true;
		}
		return false;
	}

	//---------------------------------------------------------------
	// 支払日を取得
	// 　締グループ情報からの計算を想定
	//---------------------------------------------------------------
	function getSiharaiDt($pHiDate,$pMonth=1,$pDate=31) {
		if ( $pHiDate == "" ) { return false; }
		// 指定日の日付を01に
		$wDate1	= mb_substr($pHiDate,0,6)."01";
		// 締日から月数を加算
		$wDate2	= getHiDateFromTime(strtotime("+".$pMonth." month",getTimeFromHiDate($wDate1)));
		// 日付を設定
		$wDate3	= mb_substr($wDate2,0,6).trimCode($pDate,2);

		return $wDate3;
	}

	//---------------------------------------------------------------
	// 締日を調整
	// 　締グループ情報からの計算を想定
	//---------------------------------------------------------------
	function trimSimeDt($pHiDate) {
		if ( $pHiDate == "" ) { return false; }
		// start 20.02.18 --31日以外ではundefinedが返される不具合を修正
		$wSimeDt = $pHiDate; 
		// end   20.02.18 
		// 日付を補正
		if ( mb_substr($pHiDate,6) == "31" ) {
			$wSimeDt	= getMatsubiFromHiDate(mb_substr($pHiDate,0,6)."01");
		}

		return $wSimeDt;
	}

	//---------------------------------------------------------------
	// 日付編集(漢字編集)
	// $pHiDate ：YYYYMMDD
	// $pPattern：各種
	// 戻り値   ：パターン編集後
	//---------------------------------------------------------------
	// 編集不可の場合はそのままで戻す
	//---------------------------------------------------------------
	function editDateKJ($pHiDate,$pPattern) {
		if ( strlen($pHiDate) != 8 ) {
			return $pHiDate;
		}
		if ( !is_numeric($pHiDate) ) {
			return $pHiDate;
		}
		switch ($pPattern) {
			case "YYYYSMMSDD":
				return substr($pHiDate,0,4)."年".substr($pHiDate,4,2)."月".substr($pHiDate,6,2)."日";
			case "YYSMMSDD":
				return substr($pHiDate,2,2)."年".substr($pHiDate,4,2)."月".substr($pHiDate,6,2)."日";
			case "YYSMMSDDKW":
				$kw = getYoubiKJ( getYoubiFromHiDate($pHiDate) );
				return substr($pHiDate,2,2)."年".substr($pHiDate,4,2)."月".substr($pHiDate,6,2)."日"."(".$kw.")";
			case "YYSMM":
				return substr($pHiDate,2,2)."年".substr($pHiDate,4,2)."月";
			case "MMSDD":
				return substr($pHiDate,4,2)."月".substr($pHiDate,6,2)."日";
			case "DD":
				return substr($pHiDate,6,2)."日";
		}
		return $pHiDate;
	}

//	//---------------------------------------------------------
//	// エラー時のダイアログ表示
//	//---------------------------------------------------------
//	function dieAndOpenDialog($str) {
//		$output  = "<textarea id='dMessageId' style='visibility:collapse;'>";
//		$output .= $str;
//		$output .= "</textarea><script type='text/javascript'> var popupErr=1; alert(document.getElementById('dMessageId').value);</script>";
//		die($output);
//	}

	//---------------------------------------------------------------
	// weekinfo($arg1,$arg2)
	// 【取引先納品曜日から情報取得】
	// 引数１    ：接続
	// 引数２    ：商品情報(Array)
	// 戻り値    ：週情報(Array)
	//---------------------------------------------------------------
	//-------------------------------------------------------
	// 【取引先納品曜日から情報取得】
	//-------------------------------------------------------
	function weekinfo($arg1,$arg2) {
		// グローバル変数
		global $log;

		// 引数
		$connect  = $arg1;
		$iteminfo = $arg2;

		$weekinfo = array();
		
		$weekinfo[0] = 0;	//日曜日
		$weekinfo[1] = 0;	//月曜日
		$weekinfo[2] = 0;	//火曜日
		$weekinfo[3] = 0;	//水曜日
		$weekinfo[4] = 0;	//木曜日
		$weekinfo[5] = 0;	//金曜日
		$weekinfo[6] = 0;	//土曜日

		//②-1品目区分=000だったら、
		//---------------------
		// ドライ商品の場合
		//---------------------
		if ($iteminfo["品目区分"] == "000") {
			//「取引先納品曜日」から情報取得
			$sql  = "select * from 取引先納品曜日 ";
			$sql .= "where 取引先コード =  '".$iteminfo["取引先コード"]."' ";
			$sql .= "and 部門コード = '".$iteminfo["部門コード"]."' ";
			$sql .= "and 店舗コード = '".$iteminfo["店舗コード"]."' ";
			$result = db_query ($connect, $sql);
			if (!$result) {
				echo "SQL実行エラー";
				$log->putErr(PNAME,"   SQL実行エラー".$sql);
				return false;
			}
			$num = db_num_rows($result);
			if ($num != 0) {
				$r = db_fetch_array($result, 0);
				$sun = $r["sun1"];
				$mon = $r["mon1"];
				$tue = $r["tue1"];
				$wed = $r["wed1"];
				$thu = $r["thu1"];
				$fri = $r["fri1"];
				$sat = $r["sat1"];

				//納品可能の判断
				//sun1～sat1にて、どこかに"0"があれば、日曜日は納品可。
				if (($sun == "0") or ($mon == "0") or ($tue == "0") or ($wed == "0") or ($thu == "0") or ($fri == "0") or ($sat == "0")) {
					$weekinfo[0] = 1;
				}
				//sun1～sat1にて、どこかに"1"があれば、月曜日は納品可。
				if (($sun == "1") or ($mon == "1") or ($tue == "1") or ($wed == "1") or ($thu == "1") or ($fri == "1") or ($sat == "1")) {
					$weekinfo[1] = 1;
				}
				//sun1～sat1にて、どこかに"2"があれば、火曜日は納品可。
				if (($sun == "2") or ($mon == "2") or ($tue == "2") or ($wed == "2") or ($thu == "2") or ($fri == "2") or ($sat == "2")) {
					$weekinfo[2] = 1;
				}
				//sun1～sat1にて、どこかに"3"があれば、水曜日は納品可。
				if (($sun == "3") or ($mon == "3") or ($tue == "3") or ($wed == "3") or ($thu == "3") or ($fri == "3") or ($sat == "3")) {
					$weekinfo[3] = 1;
				}
				//sun1～sat1にて、どこかに"4"があれば、木曜日は納品可。
				if (($sun == "4") or ($mon == "4") or ($tue == "4") or ($wed == "4") or ($thu == "4") or ($fri == "4") or ($sat == "4")) {
					$weekinfo[4] = 1;
				}
				//sun1～sat1にて、どこかに"5"があれば、金曜日は納品可。
				if (($sun == "5") or ($mon == "5") or ($tue == "5") or ($wed == "5") or ($thu == "5") or ($fri == "5") or ($sat == "5")) {
					$weekinfo[5] = 1;
				}
				//sun1～sat1にて、どこかに"6"があれば、土曜日は納品可。
				if (($sun == "6") or ($mon == "6") or ($tue == "6") or ($wed == "6") or ($thu == "6") or ($fri == "6") or ($sat == "6")) {
					$weekinfo[6] = 1;
				}
			}

		} else {  // ドライだったらループend
			//---------------------
			// 生鮮商品の場合
			//---------------------
			//全曜日入力可
			$weekinfo[0] = 1;	//日曜日
			$weekinfo[1] = 1;	//月曜日
			$weekinfo[2] = 1;	//火曜日
			$weekinfo[3] = 1;	//水曜日
			$weekinfo[4] = 1;	//木曜日
			$weekinfo[5] = 1;	//金曜日
			$weekinfo[6] = 1;	//土曜日
		}
		return $weekinfo;
	}

	//---------------------------------------------------------------
	// encoding_Display($arg1)
	// 【文字エンコーディングを変換する[eucjp-win→CP51932]】
	// 　※DB値を画面表示する際に機種依存文字（厄桙ﾈど）が文字化けを起こさないように。
	// 引数１    ：変換前文字列
	// 戻り値    ：変換後文字列
	//---------------------------------------------------------------
	//-------------------------------------------------------
	// 【文字エンコーディングを変換する[eucjp-win→CP51932]】
	//-------------------------------------------------------
	function encoding_Display($arg1) {
		// 特殊文字対応(UTF-8を挟んで変換)
		$work = mb_convert_encoding($arg1, "UTF-8", "eucjp-win");
		$work = mb_convert_encoding($work, "CP51932", "UTF-8");

		return $work;
	}

	//---------------------------------------------------------------
	// encoding_DBRegister($arg1)
	// 【文字エンコーディングを変換する[CP51932→eucjp-win]】
	// 　※画面表示値をDB登録する際に機種依存文字（厄桙ﾈど）が文字化けを起こさないように。
	// 引数１    ：変換前文字列
	// 戻り値    ：変換後文字列
	//---------------------------------------------------------------
	//-------------------------------------------------------
	// 【文字エンコーディングを変換する[CP51932→eucjp-win]】
	//-------------------------------------------------------
	function encoding_DBRegister($arg1) {
		// 特殊文字対応(UTF-8を挟んで変換)
		$work = mb_convert_encoding($arg1, "UTF-8", "CP51932");
		$work = mb_convert_encoding($work, "eucjp-win", "UTF-8");

		return $work;
	}

	//---------------
	//西暦を和暦に変換
	//---------------
	// $sYear:西暦年  YYYY
	// $sMonth:西暦月   mm
	// $sDay:西暦日     dd
	// $gannenCheck  返り値の和暦年のフォーマット：　0=1年、1=元年
	// $gannendoCheck  返り値の和暦年度のフォーマット：　0=(平成)31年、1=(令和)元年
	// return:和暦、和暦年、月、日
	function getWarekiYear($sYear, $sMonth, $sDay, $gannenCheck=0, $gannendoCheck=0) {

		if(!isset($sYear) || !isset($sMonth) || !isset($sDay)) {
			return null;
		}

		$wareki = doGetWarekiYear($sYear, $sMonth, $sDay);

		//平成31年度と令和元年度の切替
		if($wareki["year"] == 31 && $wareki["warekiIni"] == "H" ) {
//		if($wareki["year"] == 30 && $wareki["warekiIni"] == "H" ) { //★★★テスト
			if($gannendoCheck == 1) {
				$wareki["warekiIni"]    = 'R';
				$wareki["warekiHeader"]   = '令';
				$wareki["wareki"]  = '令和';
				$wareki["year"] = 1;
			}
		}
		//元と1の切替
		if($wareki["year"] == 1) {
			if($gannenCheck == 1) {
				$wareki["year"] = "元";
			}
		}

		return $wareki;
	}

	function doGetWarekiYear($sYear, $sMonth, $sDay) {
		//---------------
		//フォーマットのチェックと文字列への変更
		//---------------
		$sYear  = int2string($sYear, 4, '20');	//4桁の文字列へ
		$sMonth = int2string($sMonth, 2, '0');	//2桁の文字列へ
		$sDay   = int2string($sDay, 2, '0');	//2桁の文字列へ

		$wareki["month"] = $sMonth;
		$wareki["day"] = $sDay;

		//---------------
		//西暦を和暦に変換
		//---------------
		$yyyymmdd = $sYear.$sMonth.$sDay;
		$yyyymmdd = (int)$yyyymmdd; 	//8桁の数字
		if($yyyymmdd < 19890108) {//昭和
			$wareki["warekiIni"]    = 'S';
			$wareki["warekiHeader"]   = '昭';
			$wareki["wareki"]  = '昭和';
			$wareki["year"] = (int)$sYear - 1925;
		} else if($yyyymmdd < 20190501) {//平成
//		} else if($yyyymmdd < 20180501) {//★★★テスト
			$wareki["warekiIni"]    = 'H';
			$wareki["warekiHeader"]   = '平';
			$wareki["wareki"]  = '平成';
			$wareki["year"] = (int)$sYear - 1988;
		} else {//令和
			$wareki["warekiIni"]    = 'R';
			$wareki["warekiHeader"]   = '令';
			$wareki["wareki"]  = '令和';
			$wareki["year"] = 1;
			$wareki["year"] = (int)$sYear - 2018;
//			$wareki["year"] = (int)$sYear - 2017;//★★★テスト
		}

		//---------------
		//配列の返り値を返す
		//---------------
		return $wareki;
	}

	//---------------
	//和暦を西暦に変換
	//---------------
	// $gengo:RE:令和 HE:平成 SH:昭和:string
	// $wYear:和暦年
	function getSeirekiYear($gengo, $wYear) {

		if($wYear == "元年" || $wYear == "元"){
			$wYear = 1;
		}

		if($gengo == "SH") { //昭和
			$sYear = (int)$wYear + 1925;
		} else if($gengo == "HE") { //平成
			$sYear = (int)$wYear + 1988;
		} else { //令和
			$sYear = (int)$wYear + 2018;
//			$sYear = (int)$wYear + 2017;//★★★テスト
		}

		return $sYear;
	}

	function int2string($argVal, $argKeta, $argKeyword) {
		if(gettype($argVal) == 'string') {
			return str_pad($argVal, $argKeta, $argKeyword, STR_PAD_LEFT);
		} else {
			return str_pad((string)$argVal, $argKeta, $argKeyword, STR_PAD_LEFT);
		}
	}

	//---------------------------------------------------------------
	// 全角変換（変換されない半角文字も含む）
	// 引数　　：$str：変換前文字
	//     　　：$opt：オプション(初期値は半角英字、半角数字、半角スペースを全角に変換
	//     　　：                         半角カタカナ、全角カタカナを全角ひらがな変換)
	// 戻り値　：変換後文字
	//---------------------------------------------------------------
	function convert_kana_zen($str,$opt='HVRNSc',$sjis=false) {
		
		if ($sjis) {
			//機種依存文字の変換
			$str = convert_sjis_gaiji($str,strlen($str),$chgChar);
		}
		
		//ここから[mb_convert_kana]で全角変換されない文字を追加
		
		//半角ハイフンを全角に
		$phyhen = array('-', '－', '‐', '―');
		$change = '－';
		$str = str_replace($phyhen, $change, $str);
		
		//半角括弧を全角に
		$phyhen = array('(');
		$change = '（';
		$str = str_replace($phyhen, $change, $str);
		
		$phyhen = array(')');
		$change = '）';
		$str = str_replace($phyhen, $change, $str);
		
		//ドットを全角に
		$phyhen = array('.');
		$change = '．';
		$str = str_replace($phyhen, $change, $str);
		
		
		// 通常の全角変換
		$str = mb_convert_kana($str, $opt);
		
		return $str;
	}

	//----------------------------------------------------------------------------------
	// 【 機種依存文字存在チェック 変換　】
	// convert_sjis_gaiji($arg1,$len,&$chgChar)
	// 引数１    ：検査項目値 (ＥＵＣコード）
	// 引数２    ：検査レングス(バイト数)
	// 引数３    ：変換後文字データ(ＥＵＣコード）
	// 戻り値    ：変換文字　　　　(ＥＵＣコード）
	//　　注：戻り値の変換文字は入力の桁数を越えることがあります。
	//     例：㈱→（株）の場合、４バイト増えます。
	//
	//******* 文字コード説明 シフトJIS(Shift_JIS) *****
	// ※入力の文字列はSJIS-winからeucJP-winで変換されたデータであること
	// 　SJISからEUCで変換されたデータの場合、文字化けするデータがあるため
	//   
	//  シフトJISの1バイトコード（半角文字）のエリア 
	//   0x00～0x1f、0x7f は制御コードです 
	//   0x20～0x7e はASCII文字です 
	//   0xa1～0xdf は半角カタカナです 
	// 
	// シフトJISの2バイトコード（全角文字）のエリア（JIS X 0208の漢字エリア） 
	// 　 上位1バイト　 0x81～0x9f、 0xe0～0xef 
	// 　 下位1バイト　 0x40～0x7e、 0x80～0xfc 
	//     0x8540～ 0x889e　は機種依存文字の主なエリアです 
	//     0xeb40～ 0xeffc　はMacOS では縦書用文字、Windows では特殊な外字エリアです 
	//     0xf040～　は外字エリアです 
	//----------------------------------------------------------------------------------
	function convert_sjis_gaiji($arg1,$len,&$chgChar) {
		$outData = "";
		$chgChar ="";
		$encSJIS     = "SJIS-win";
		$encOriginal = "eucJP-win";
		$sjisStr = mb_convert_encoding($arg1, $encSJIS, $encOriginal);
		// １文字抽出
		for($i = 0; $i < $len; $i++) {
			// 指定位置の文字を取り出す
			$ch = $sjisStr[$i];
			$c = mb_substr($arg1, $i, 1);
			
			// 取得文字を１６進数に変換
			$hex = intval(bin2hex($ch), 16);
			
			// 1文字の半角／全角の判断
			if ( ($hex>=0x00 && $hex<=0x7f) || ($hex>=0xa1 && $hex<=0xdf )){    // 半角文字
				// 半角文字の場合、置換文字
				switch ($hex ) {
					case 0x09:                // タブ
					case 0x22:                // "
					case 0x27:                // '
					case 0x2c:                // ,
						$ch= " ";            // スペース
						$chgChar .=" ";
						break;
				}
				$outData = $outData .$ch;
			} else  {
				// 指定位置の文字を取り出す
				$ch = $sjisStr[$i].$sjisStr[$i+1];
				$i = $i  +1 ;
				// 取得文字を１６進数に変換
				$hex = intval(bin2hex($ch), 16);
				
				// 機種依存文字のチェック
				if ( $hex>=0x8540 && $hex<=0x889e) {
					$ch = mb_convert_encoding("　","SJIS-win");
					$kisyu = array(
						 0x8740  =>  "１"    ,  0x8741  => "２"   ,  0x8742  => "３"    ,  0x8743  => "４",
						 0x8744  =>  "５"    ,  0x8745  => "６"   ,  0x8746  => "７"    ,  0x8747  => "８",
						 0x8748  =>  "９"    ,  0x8749  => "１０" ,  0x874a  => "１１"  ,  0x874b  => "１２",
						 0x874c  =>  "１３"  ,  0x874d  => "１４" ,  0x874e  => "１５"  ,  0x874f  => "１６",
						 0x8750  =>  "１７"  ,  0x8751  => "１８" ,  0x8752  => "１９"  ,  0x8753  => "２０",
						 0x8754  =>  "１"    ,  0x8755  => "２"   ,  0x8756  => "３"    ,  0x8757  => "４",
						 0x8758  =>  "５"    ,  0x8759  => "６"   ,  0x875a  => "７"    ,  0x875b  => "８",
						 0x875c  =>  "９"    ,  0x875d  => "１０" ,  0x876f  => "ｍｍ"  ,  0x8770  => "ｃｍ",
						 0x8771  =>  "ｋｍ"  ,  0x8772  => "ｍｇ" ,  0x8773  => "ｋｇ"  ,  0x8774  => "ｃｃ"  ,
						 0x8775  =>  "ｍ２"  ,  0x8785  => "上"   ,  0x8786  => "中"    ,  0x8787  => "下"    ,
						 0x8788  =>  "左"    ,  0x8789  => "右"   ,  0x878a  => "（株）",  0x878b  => "（有）",
						 0x878c  => "（代）",
						);
					// 配列にデータがある場合置換
					foreach ($kisyu as $key => $value) {
						if ($key == $hex ) {
							$ch =  mb_convert_encoding($value,"SJIS-win");
							break;
						}
					}
					$chgChar .=$ch;
					
					$outData = $outData .$ch;
				} elseif ( ($hex>=0xfa40 && $hex<=0xfc4c) || ($hex>=0xed40 && $hex<=0xeefc )){ 
					//IBM拡張文字　NEC選定IBM拡張文字  のチェック
					$ch = mb_convert_encoding("　","SJIS-win");
					$ibmK = array(
						 0xed4a   => "昇"    ,  0xed64   =>   "宣"  ,  0xed75  =>    "和" ,
						 0xed80   => "塚"    ,  0xed81   =>   "増"  ,  0xed8e   =>   "寛" ,  0xed95   =>    "崎"  ,
						 0xed9e   => "徳"    ,  0xeda1   =>   "悦"  ,  0xedb1   =>   "教" ,  0xedb4   =>    "昂"  ,
						 0xedbc   => "晴"    ,  0xedc4   =>   "朗"  ,  0xedc5   =>   "杉" ,  0xedc7   =>    "桑"  ,
						 0xedc9   => "柳"    ,  0xedd2   =>   "横"  ,  0xede4   =>   "清" ,  0xedee   =>    "浜"  ,
						 0xedf1   => "瀬"    ,  0xee42   =>   "猪"  ,  0xee52   =>   "瓶" ,  0xee59   =>    "益"  ,
						 0xee61   => "礼"    ,  0xee62   =>   "神"  ,  0xee63   =>   "祥" ,  0xee65   =>    "福"  ,
						 0xee69   => "靖"    ,  0xee6c   =>   "精"  ,  0xee70   =>   "緑" ,  0xee71   =>    "緒"  ,
						 0xee75   => "羽"    ,  0xee82   =>   "薫"  ,  0xee8d   =>   "諸" ,  0xee92   =>    "頼"  ,
						 0xee93   => "賢"    ,  0xee95   =>   "赳"  ,  0xee96   =>   "軌" ,  0xee98   =>    "逸"  ,
						 0xee9a   => "郎"    ,  0xee9b   =>   "都"  ,  0xee9c   =>   "郷" ,  0xeecc   =>    "間"  ,
						 0xeecd   => "隆"    ,  0xeed6   =>   "青"  ,  0xeeda   =>   "飯" ,  0xeedb   =>    "飼"  ,
						 0xeedd   => "館"    ,  0xeee0   =>   "高"  ,  0xeeea   =>   "鶴" ,  0xeeec   =>    "黒"  ,
						 0xeeef   => "１"    ,  0xeef0   =>   "２"   , 0xeef1   =>   "３"  , 0xeef2   =>    "４"  ,
						 0xeef3   => "５"    ,  0xeef4   =>   "６"   , 0xeef5   =>   "７"  , 0xeef6   =>    "８"  ,
						 0xeef7   => "９"    ,  0xeef8   =>   "１０" ,
						 0xfa40   => "１"    ,  0xfa41   =>   "２"   , 0xfa42   =>   "３"  , 0xfa43   =>    "４",
						 0xfa44   => "５"    ,  0xfa45   =>   "６"   , 0xfa46   =>   "７"  , 0xfa47   =>    "８",
						 0xfa48   => "９"    ,  0xfa49   =>   "１０" ,
						 0xfa4a   => "１"    ,  0xfa4b   =>   "２"   , 0xfa4c   =>   "３"  , 0xfa4d   =>    "４",
						 0xfa4e   => "５"    ,  0xfa4f   =>   "６"   , 0x4a50   =>   "７"  , 0xfa51   =>    "８",
						 0xfa52   => "９"    ,  0xfa53   =>   "１０" ,
						 0xfa58   => "（株）",  0xfa67   =>   "昇"   , 0xfa81   =>   "宣"  , 0xfa93   =>    "和"  ,
						 0xfa9c   => "塚"    ,  0xfa9d   =>   "増"   , 0xfaaa   =>   "寛"  , 0xfab1   =>    "崎"  ,
						 0xfaba   => "徳"    ,  0xfabd   =>   "悦"   , 0xfacd   =>   "教"  , 0xfad2   =>    "昂"  ,
						 0xfad8   => "晴"    ,  0xfae0   =>   "朗"   , 0xfae1   =>   "杉"  , 0xfae3   =>    "桑"  ,
						 0xfae5   => "柳"    ,  0xfaee   =>   "横"   , 0xfb43   =>   "清"  , 0xfb4d   =>    "浜"  ,
						 0xfb50   => "瀬"    ,  0xfb5e   =>   "猪"   , 0xfb6e   =>   "瓶"  , 0xfb75   =>    "益"  ,
						 0xfb7d   => "礼"    ,  0xfb7e   =>   "神"   , 0xfb80   =>   "祥"  , 0xfb82   =>    "福"  ,
						 0xfb86   => "靖"    ,  0xfb89   =>   "精"   , 0xfb8d   =>   "緑"  , 0xfb8e   =>    "緒"  ,
						 0xfb92   => "羽"    ,  0xfb9e   =>   "薫"   , 0xfba9   =>   "諸"  , 0xfbae   =>    "頼"  ,
						 0xfbaf   => "賢"    ,  0xfbb1   =>   "赳"   , 0xfbb2   =>   "軌"  , 0xfbb4   =>    "逸"  ,
						 0xfbb6   => "郎"    ,  0xfbb7   =>   "都"   , 0xfbb8   =>   "郷"  , 0xfbe8   =>    "間"  ,
						 0xfbe9   => "隆"    ,  0xfbf2   =>   "青"   , 0xfbf6   =>   "飯"  , 0xfbf7   =>    "飼"  ,
						 0xfbf9   => "館"    ,  0xfbfc   =>   "高"   , 0xfc49   =>   "鶴"  , 0xfc4b   =>    "黒"  
						);
					
					// 配列にデータがある場合置換
					foreach ($ibmK as $key => $value) {
						if ($key == $hex ) {
							 $ch =  mb_convert_encoding($value,"SJIS-win");
							 break;
						}
					}
					$chgChar .=$ch;
					$outData = $outData .$ch;
				} else {
					$outData = $outData .$ch;
				}
			
			}
		}
		
		$chgChar = mb_convert_encoding($chgChar,$encOriginal, $encSJIS);
		return  mb_convert_encoding($outData,$encOriginal, $encSJIS);
	}

?>
