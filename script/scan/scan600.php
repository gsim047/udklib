<?php

include_once("cfg.php");

// start

	$dt = date("Ymd-His");
	$colorTitle = colorText();
	//echo "scan-$dt.jpg\n\n";
	$addr = scannerAddr(); //"snapscan:libusb:001:006";

	h1("Сканирование 600 dpi");

	echo "<h3><font color=#$colorTitle >Scanner start... <br/>Image: 'scan-$dt.jpg'<br/><br/>\n";
	echo "Внимание! После долгого простоя, сканер прогревается до 45 секунд! Подождите немного...<br/></font></h3>\n";

	//echo "<br/><a href=\"/ex.php?gal=scan\"><b>Перейти к сканам &gt;&gt;</b></a><br/>";
//	system("whoami");
	$dir = fsDirScans();
	system("/usr/bin/scanimage -d $addr --format=jpeg --resolution 600 -o $dir/scan-$dt.jpg >/dev/null 2>&1 &");

	//system("ls -lA");
	h2();

?>


