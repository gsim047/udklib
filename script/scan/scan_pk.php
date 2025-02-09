<?php

include_once("cfg.php");

	
// start
	$colorTitle = colorText();

	h1("Скачивание каталога [scan]");

	$dir = fsDir();
	$www = "/var/www/html";
	system("rm -f $www/scan_dl.zip >/dev/null 2>&1");
	system("cd $dir");
	system("7z a $www/scan_dl.zip scan-*.jpg >/dev/null 2>&1");
	echo "<h3><a href='/scan_dl.zip'><font color=#$colorTitle >Скачать все сканы &gt;&gt;</font></a>";

	h2();

?>


