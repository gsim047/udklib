<?php

include_once("cfg.php");

/*
function h1()
{
	echo "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n";
	echo "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n";
	echo "  <head>\n";
	echo "    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n";
	echo "    <title>Apache2 Ubuntu Default Page: It works</title>\n";
	echo "  </head>\n";
	echo "  <body>\n";
}


function h2()
{
	echo "  </body>\n";
	echo "</html>\n";
}
*/
	
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
	system("/usr/bin/scanimage -d $addr --format=jpeg --resolution 600 -o /var/www/html/scan_/scan/scan-$dt.jpg >/dev/null 2>&1 &");

	//system("ls -lA");
	h2();

?>


