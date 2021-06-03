<?php

/* **********************************************
メール送信の使い方

$log->putErr(PNAME,"!!!SELECT失敗!!!");
$log->putErr(PNAME,"!!!異常終了。システム担当に連絡してください!!!");
// 結果メール送信
$repMsg = $this->log->getReport();
mail_sender("[ERR]"."".date("Y/m/d H:i:s"),$repMsg,MAIL_FROM_ADDRESS,TO_OPRT);
mail_sender("[ERR]"."".date("Y/m/d H:i:s"),$repMsg,MAIL_FROM_ADDRESS,TO_SYSINFO);
************************************************** */






//===================================================================================
//	(内容)
//		メール送信する
//
//	※修正履歴
//		VER.1.0		2017.10.10	suzuki	※新規作成
//===================================================================================


	function mail_sender($argTitle,$argStr,$argFrom,$argKbn) {
		global $OPRT_MAIL_ADDRESS;
		global $ABEND_MAIL_ADDRESS;
		global $SYSINFO_MAIL_ADDRESS;
		global $DEV_MAIL_ADDRESS;

		//タイトル
		$argTitle2 = mb_encode_mimeheader ( $argTitle, "eucJP-win", "B", "\r\n", 0 );

		// 本文
		$argStr = mb_convert_encoding($argStr, "ISO-2022-JP", "EUC-JP");
		// FROM
		$argFrom="From: ".$argFrom."\r\n"."Reply-To: webmaster@himuka.ne.jp\r\n"."X-Mailer: PHP/".phpversion()."\r\n"
								."Content-Type: text/plain; charset=ISO-2022-JP";
		// メール配信
		$TO_MAIL_ADDRESS = Array();
		switch ( $argKbn) {
			case TO_OPRT : // 当番向け
				$TO_MAIL_ADDRESS = $OPRT_MAIL_ADDRESS;
				break;
			case TO_ABEND : // 緊急用
				$TO_MAIL_ADDRESS = $ABEND_MAIL_ADDRESS;
				break;
			case TO_SYSINFO : // 開発向け
				$TO_MAIL_ADDRESS = $SYSINFO_MAIL_ADDRESS;
				break;
			case TO_DEV : // 個人監視用
				$TO_MAIL_ADDRESS = $DEV_MAIL_ADDRESS;
				break;
			default : // 未定義
				break;
		}
		foreach ($TO_MAIL_ADDRESS as $mkey=>$toMailAd) {
			mail($toMailAd,  $argTitle2, $argStr, $argFrom);
		}
	}

?>