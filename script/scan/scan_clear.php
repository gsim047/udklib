<?php

include_once("cfg.php");


	
// start
	$colorTitle = colorText();

	h1("Очистка каталога [scan]");

	$dir = fsDirScans();
	system("rm -f $dir/scan-*.jpg >/dev/null 2>&1");
	system("rm -f $dir/thumb/scan-*.jpg >/dev/null 2>&1");
	echo "<h3><font color=#$colorTitle >Все сканы удалены!</font></h3>";

	h2();

?>


