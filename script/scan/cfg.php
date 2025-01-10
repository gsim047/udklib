<?php
//define("DBHOST",'localhost');
//define("DBUSER",'root');
//define("DBPASS",'rrtt');
//define("DBNAME",'betatest');

if ( !defined('ABS_PATH') )
	define('ABS_PATH', dirname(__FILE__) . '/');

if ( !defined('EXT_PATH') )
	define('EXT_PATH', '');




function dp($msg)
{
	echo "$msg<br/>\n";
	return "";
}// dp


function readLst($fname)
{
	$ret = array();
	$f = fopen($fname, "r");
	if ( $f === false ){
		echo("Can't open file [$fname]! <br>\n");
		exit();
	}
	
	$nn = 0;
	while ( ($s = fgets($f)) !== false ){
		$nn++;
		$s = trim($s);
		if ( strlen($s) == 0 ){
			echo "<br>";
			continue;
		}
		if ( $s[0] == '#' ){
			continue;
		}
		if ( $s == '--' ){
			echo "<hr>";
			continue;
		}
		if ( ($pos = strpos($s, " ")) === false ){
			echo "$nn ???: $s <br>";
			continue;
		}
		$s1 = substr($s, 0, $pos);
		$s2 = trim(substr($s, $pos));
		echo "» <a href='$s1'>".$s2."</a><br>";
	}
//	echo("<br>\n");

	fclose($f);

	return $ret;
}// readLst



function buts($ex = "")
{
	echo "<a href='/'><img src='/home.png' alt='' style='border:2px solid black'></a>&nbsp;";
	echo "<a href='/ex.php?gal=scan'><img src='/pics.png' alt='Сканы' style='border:2px solid black'></a>&nbsp;";
	echo "<a href='/scan300.php'><img src='/scan300.png' alt='Сканировать на 300 dpi' style='border:2px solid black'></a>&nbsp;";
	echo "<a href='/scan400.php'><img src='/scan400.png' alt='Сканировать на 400 dpi' style='border:2px solid black'></a>&nbsp;";
	echo "<a href='/scan600.php'><img src='/scan600.png' alt='Сканировать на 600 dpi' style='border:2px solid black'></a>&nbsp;";
	if ( $ex != "dl" ){
		echo "<a href='/scan_pk.php'><img src='/dl.png' alt='Скачать все сканы' style='border:2px solid black'></a>&nbsp;";
	}
	if ( $ex != "clear" ){
		echo "<a href='/scan_clear.php'><img src='/clear.png' alt='Удалить все сканы' style='border:2px solid black'></a>&nbsp;";
	}
}


function scannerAddr()
{
	return "snapscan:libusb:001:006";
}


function colorBg()
{
	return "202020";
}

function colorText()
{
	return "7fafcf";
}

function colorError()
{
	return "ff7f7f";
}



function h1($tit = "")
{
	//$tit = "Очистка каталога [scan]";

	$colorBg    = colorBg();
	$colorTitle = colorText();
	$colorError = colorError();


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


function readCfg()
{
	$ret = "";
	$f = fopen("/var/www/html/scan.cfg", "r");
	if ( $f != FALSE ){
		$ret = fgets($f);
		fclose($f);
	}
	return $ret;
}


function writeCfg($cfg)
{
	$f = fopen("/var/www/html/scan.cfg", "w");
	if ( $f != FALSE ){
		fwrite($f, $cfg);
		fclose($f);
	}
}


function testCfg($cfg)
{
	if ( strlen($cfg) == 7 ){
		$u1 = substr($cfg, 0, 3);
		$u2 = substr($cfg, 4, 3);
		$fn = "/dev/bus/usb/" . $u1 . "/" . $u2;
		$tmp = "/tmp/__tq23__.tmp";
		if ( file_exists($fn) ){
			system("cat $fn >$tmp");
			$sz = filesize($tmp);
			//echo "file $fn: $sz\n";
			unlink($tmp);
			if ( $sz == 57 ){
				return 1;
			}
		}
	}
	return 0;
}


function findCfg()
{
	for ( $i = 1; $i < 8; $i++ ){
		for ( $j = 1; $j < 8; $j++ ){
			$cfg = sprintf("%03d:%03d", $i, $j);
			//echo $cfg . "\n";
			if ( testCfg($cfg) == 1 ){
				//echo "OK!\n";
				return $cfg;
			}
		}
	}
	//echo "Not found!\n";
	return "";
}




?>