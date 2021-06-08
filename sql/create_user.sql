--
-- テーブル定義 `user`
--

CREATE TABLE `user` (
  `userid` varchar(50) NOT NULL,
  `usernm` text NOT NULL,
  `email` text,
  `passwd` text NOT NULL,
  `token` text,
  `last_login` text,
  `indt` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `udt` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `utanto` varchar(50) NOT NULL DEFAULT 'ANYONE'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;