<?php

include_once("cfg.php");

/*
function buts()
{
	echo "<a href='/'><img src='/home.png' alt='' style='border:2px solid black'></a>&nbsp;";
	echo "<a href='/ex.php?gal=scan'><img src='/pics.png' alt='Сканы' style='border:2px solid black'></a>&nbsp;";
	echo "<a href='/scan300.php'><img src='/scan300.png' alt='Сканировать на 300 dpi' style='border:2px solid black'></a>&nbsp;";
	echo "<a href='/scan400.php'><img src='/scan400.png' alt='Сканировать на 400 dpi' style='border:2px solid black'></a>&nbsp;";
	echo "<a href='/scan600.php'><img src='/scan600.png' alt='Сканировать на 600 dpi' style='border:2px solid black'></a>&nbsp;";
	echo "<a href='/'><img src='/dl.png' alt='Скачать все сканы' style='border:2px solid black'></a>&nbsp;";
	echo "<a href='/scan_clear.php'><img src='/clear.png' alt='Удалить все сканы' style='border:2px solid black'></a>&nbsp;";
}


function h1()
{
	$tit = "Очистка каталога [scan]";

	$colorBg    = "202020";
//	$colorBg    = "404040";
	$colorTitle = "7fafcf";
	$colorError = "ff7f7f";


	echo "<!DOCTYPE html>\n";
//	echo "<html lang='en-us'>\n";
	echo "<head>\n";
	echo "	<meta charset='utf-8'>\n";
	echo "	<title>$tit</title>\n";
	echo "	<link rel='icon' type='image/x-icon' href='/favicon.ico'>\n";
	echo "	<link rel='shortcut icon' type='image/x-icon' href='/favicon.ico'>\n";
	echo "	<link rel='stylesheet' href='/css/lightbox.min.css'>\n";  // !!!
	echo "</head>";
	
	echo "<body bgcolor=#$colorBg >";

	buts();

	echo "<h1><font color=#$colorTitle >$tit</font></h1>\n\n\n";
}


function h2()
{
	echo "</body>\n";
	echo "</html>\n";
}
*/

	
// start
	$colorTitle = colorText();

	h1("Скачивание каталога [scan]");

	system("bash /var/www/html/scan_pk.sh >/dev/null 2>&1");
	echo "<h3><a href='/scan_dl.zip'><font color=#$colorTitle >Скачать все сканы &gt;&gt;</font></a>";

	h2();

?>


