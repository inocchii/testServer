<?php

ini_set('display_errors', 0);
/****
 *  CDealJson
 *  UTF8��JSON�ǡ�����EUC�Ķ����Ǽ�갷����褦�ˤ��륯�饹
 * @author T.Suzuki
 * @version 2017.08.10
 */
class CDealJson {
	private $json = null;//UTF8�ǡ���������JSON
//	private $log ;
	/* __construct
	 * �����ȥ饯��
	 * @param $json JSON�ǡ���
	 */
	function __construct() {
//		$this->log = new CLog();
	}

	/* setJson
	 * JSON���å���
	 * @param $json JSON�ǡ���
	 * @return ����:true , ����:false
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
	 * JSON���å���
	 * @return ����:JSON�ǡ��� , ����:false
	 */
	public function getJson() {
		try {
			return $this->json;
		} catch (Exception $e) {
			return false;
		}
	}
	
	/* set
	 * JSON�ǡ������å���
	 * EUC�Υ����ȥǡ������Ϥ���UTF-8�ǥǡ����ݴɤ��ޤ���
	 * @param $pKey ����
	 * @param $pValue �ǡ���
	 * @return ����:true , ����:false
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
	 * JSON�ǡ������å���
	 * EUC�Υ�����UTF-8�ǡ������Ϥ���UTF-8�ǥǡ����ݴɤ��ޤ���
	 * @param $pKey ����
	 * @param $pValue �ǡ���
	 * @return ����:true , ����:false
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
	 * JSON�ǡ������å���
	 * EUC�Υ������Ϥ���EUC�ǥǡ����֤��ޤ���
	 * @param $pKey ����
	 * @return ����:�ǡ��� , ����:false
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
	 * JSON�ǡ������å���
	 * EUC�Υ������Ϥ���UTF-8�ǥǡ����֤��ޤ���
	 * @param $pKey ����
	 * @return ����:�ǡ��� , ����:false
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
	 * EUC��UTF-8���Ѵ�
	 * @param $data �Ѵ��оݥǡ���
	 * @return �Ѵ���ǡ���
	 */
	public static function e2u($data) {
		return self::mb_convert_encoding_recursive($data, 'UTF-8', 'eucJP-win');
		//return mb_convert_variables('UTF-8', 'eucJP-win', $data);
	}
	
	/**
	 * u2e
	 * UTF-8��EUC���Ѵ�
	 * @param $data �Ѵ��оݥǡ���
	 * @return �Ѵ���ǡ���
	 */
	public static function u2e($data) {
		return self::mb_convert_encoding_recursive($data, 'eucJP-win', 'UTF-8');
		//return mb_convert_variables('eucJP-win', 'UTF-8', $data);
		
	}

	/**
	 * ʸ���������Ѵ���Ƶ�Ū�˹Ԥ��ؿ� mb_convert_encoding�γ�ĥ
	 * @param mixed $cnvArray �Ѵ���������ޤ���ʸ����
	 * @param $to_encoding �Ѵ����ʸ�����󥳡��ǥ���
	 * @param $from_encoding  �Ѵ�����ʸ�����󥳡��ǥ���̾����ꤷ�ޤ��� 
	 * @return mixed ʸ���������Ѵ������������ޤ���ʸ������֤��ޤ�
	 *
	 * (��) mb_convert_variables�Ǥ�Ϣ�����󥭡��ޤ��Ѵ�����ʤ��ä����Ἣ��
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