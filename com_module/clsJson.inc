<?php

ini_set('display_errors', 0);
/****
 *  CDealJson
 *  UTF8のJSONデータをEUC環境下で取り扱えるようにするクラス
 * @author T.Suzuki
 * @version 2017.08.10
 */
class CDealJson {
	private $json = null;//UTF8データ形式のJSON
//	private $log ;
	/* __construct
	 * 今ストラクタ
	 * @param $json JSONデータ
	 */
	function __construct() {
//		$this->log = new CLog();
	}

	/* setJson
	 * JSONセッター
	 * @param $json JSONデータ
	 * @return 成功:true , 失敗:false
	 */
	public function setJson($json) {
		try {
			$this->json = $json;
		} catch (Exception $e) {
			return false;
		}
		return true;
	}
	
	/* setJson
	 * JSONゲッター
	 * @return 成功:JSONデータ , 失敗:false
	 */
	public function getJson() {
		try {
			return $this->json;
		} catch (Exception $e) {
			return false;
		}
	}
	
	/* set
	 * JSONデータセッター
	 * EUCのキーとデータを渡してUTF-8でデータ保管します。
	 * @param $pKey キー
	 * @param $pValue データ
	 * @return 成功:true , 失敗:false
	 */
	public function set($pKey, $pValue) {
		try {
			$k = self::e2u($pKey);
			$v = self::e2u($pValue);
			$this->json->$k = $v;
		} catch (Exception $e) {
			//throw $e;
			return false;
		}
		return true;
	}

	/* setRaw
	 * JSONデータセッター
	 * EUCのキーとUTF-8データを渡してUTF-8でデータ保管します。
	 * @param $pKey キー
	 * @param $pValue データ
	 * @return 成功:true , 失敗:false
	 */
	public function setRaw($pKey, $pValue) {
		try {
			$k = self::e2u($pKey);
			$v = $pValue;
			$this->json->$k = $v;
		} catch (Exception $e) {
			//throw $e;
			return false;
		}
		return true;
	}

	/* get
	 * JSONデータゲッター
	 * EUCのキーを渡してEUCでデータ返します。
	 * @param $pKey キー
	 * @return 成功:データ , 失敗:false
	 */
	public function get($pKey) {
		$v = null;
		try {
			$k = self::e2u($pKey);
			$v = self::u2e($this->json->$k);
		} catch (Exception $e) {
			//throw $e;
			return false;
		}
		return $v;
	}
	
	/* getRaw
	 * JSONデータゲッター
	 * EUCのキーを渡してUTF-8でデータ返します。
	 * @param $pKey キー
	 * @return 成功:データ , 失敗:false
	 */
	public function getRaw($pKey) {
		$v = null;
		try {
			$k = self::e2u($pKey);
			$v = $this->json->$k;
		} catch (Exception $e) {
			//throw $e;
			return false;
		}
		return $v;
	}
	
	/**
	 * e2u
	 * EUCをUTF-8に変換
	 * @param $data 変換対象データ
	 * @return 変換後データ
	 */
	public static function e2u($data) {
		return self::mb_convert_encoding_recursive($data, 'UTF-8', 'eucJP-win');
		//return mb_convert_variables('UTF-8', 'eucJP-win', $data);
	}
	
	/**
	 * u2e
	 * UTF-8をEUCに変換
	 * @param $data 変換対象データ
	 * @return 変換後データ
	 */
	public static function u2e($data) {
		return self::mb_convert_encoding_recursive($data, 'eucJP-win', 'UTF-8');
		//return mb_convert_variables('eucJP-win', 'UTF-8', $data);
		
	}

	/**
	 * 文字コード変換を再帰的に行う関数 mb_convert_encodingの拡張
	 * @param mixed $cnvArray 変換する配列または文字列
	 * @param $to_encoding 変換後の文字エンコーディング
	 * @param $from_encoding  変換前の文字エンコーディング名を指定します。 
	 * @return mixed 文字コード変換した後の配列または文字列を返します
	 *
	 * (注) mb_convert_variablesでは連想配列キーまで変換出来なかったため自作
	 *
	 */
	private static function mb_convert_encoding_recursive( $cnvArray, $to_encoding, $from_encoding ){
		if (!is_array($cnvArray) ) {
			return mb_convert_encoding($cnvArray, $to_encoding, $from_encoding);
		} else {
			$result = array();
			foreach ($cnvArray as $k => $v) {
				if( is_array($v) ) {
					$result[mb_convert_encoding($k, $to_encoding, $from_encoding)] = self::mb_convert_encoding_recursive($v, $to_encoding, $from_encoding);
				} else {
					$result[mb_convert_encoding($k, $to_encoding, $from_encoding)] = mb_convert_encoding($v, $to_encoding, $from_encoding);
				}
			}
			return $result;
		}
	}
}
 
?>