<?php

include_once("cfg.php");


// строка из _POST/_GET
function get_pg_string($str)
{
	$s = "";
	if ( isset($_POST[$str]) ){
		$s = $_POST[$str];
	}else if ( isset($_GET[$str]) ){
		$s = $_GET[$str];
	}
	return $s;
}// get_pg_string


// число из _POST/_GET
function get_pg_int($str, $def = 0)
{
	$res = $def;
	$s = get_pg_string($str);
	if ( strlen($s) > 0 ){
		$res = intval($s);
	}
	return $res;
}// get_pg_int


function checkThumb($path, $f1)
{
	$sz = 180;
	$bord = 3;             // border
	$sh = $bord + 1;       // shift
	$sz1 = $sz - 2*$sh;    // max size of inner img

	$thumb_path = $path."/thumb";
	//dp("t:path: $path");
	//dp("t:f1: $f1");
	if ( !file_exists($thumb_path) ){
		mkdir($thumb_path, 0777);
	}
	$p1 = $path."/".$f1;
	$p2 = $thumb_path."/".$f1;
	//dp("t:p1: $p1");
	//dp("t:p2: $p2");
	$mk_thumb = 0;
	if ( !file_exists($p2) ){
		$mk_thumb = 1;
	}else{
		if ( filemtime($p1) > filectime($p2) ||
		     filectime($p1) > filectime($p2) ){
			$mk_thumb = 1;
			unlink($p2);
		}
	}
	if ( $mk_thumb ){
		$size = getimagesize($p1);
		$bigimg = imageCreateFromJpeg($p1);
		$smallimg = imageCreateFromJpeg(dirname(__FILE__)."/th$sz.jpg");
//		$smallimg = imageCreateTrueColor(150, 150);
		if ( $size[0] >= $size[1] ){  // horizontal or square
			$sz2 = floor($sz1 * $size[1] / $size[0]);
			$sh2 = floor(($sz1 - $sz2) / 2);
			imagecopyresampled($smallimg, $bigimg, $sh, $sh + $sh2, 0, 0, $sz1, $sz2, $size[0], $size[1]);
		}else{                        // vertical
			$sz2 = floor($sz1 * $size[0] / $size[1]);
			$sh2 = floor(($sz1 - $sz2) / 2);
			imagecopyresampled($smallimg, $bigimg, $sh + $sh2, $sh, 0, 0, $sz2, $sz1, $size[0], $size[1]);
		}
//		imagecopyresampled($smallimg, $bigimg, 0, 0, 0, 0, 150, 150, $size[0], $size[1]);
		imagedestroy($bigimg);
		imagejpeg($smallimg, $p2, 80);
		imagedestroy($smallimg);
	}
}// checkThumb


function cmp__($a, $b)
{
	return $a['name'] > $b['name'];
}


function mklist($path, $pathw)
{
	$fl = array();

	//dp("path: $path");
	//dp("www-path: $pathw");

	if ( file_exists($path) ){

		//dp("exists");
		$d = dir($path);
		//echo "Каталог: ".$show."<br>\n";
		while ( false != ($entry = $d->read()) ){
			//dp("entry: $entry");
			if ( $entry[0] <> '.' ){
				if ( is_dir($path."/".$entry) ){
				}else{
					if ( false != strpos($entry, ".jpg") ){
						//dp("add entry: $entry");
//	echo "-- $entry<br>\n";
//						array_push($fl, $entry);
//						echo "-> $entry<br>";
						$f1 = array(
							'name' => $entry,
							'path' => $path."/".$entry,
							'pathw' => $pathw."/".$entry,
							'thumb' => $pathw."/thumb/".$entry,
						);
//						echo "==> ".$f1['name']." / ".$f1['path']."/ <br>";
						$fl[] = $f1;
					}
				}
			}
		}
		$d->close();

		usort($fl, "cmp__");
	}
	//dp();
	return $fl;
}// mklist



function chkDir($dir)
{
	$res = "";
	$c1 = substr($dir, 0, 1);
	if ( $c1 == '.' || $c1 == '/' ){
		$res = "Недопустимый адрес";
	}else if ( false !== strpos($dir, "..") ){
		$res = "Недопустимый адрес";
	}
	return $res;
}// chkDir





// start

	$cfg = readCfg();
	$res = testCfg($cfg);
	//echo "cur cfg: $cfg\n";
	if ( $res != 1 ){
		$cfg = findCfg();
		writeCfg($cfg);
	}

	$wwwdir = "/scan_";
	$basedir = dirname(__FILE__).$wwwdir;

	$dir = get_pg_string("gal");
	$cols = get_pg_int("n", 6);


	$res = chkDir($dir);

	$path = $basedir."/".$dir;
	$pathw = $wwwdir."/".$dir;

	$fl = mklist($path, $pathw);
//	var_dump($fl);

	$tit = "Каталог [$dir]";

	$colorTitle = colorText();
	$colorError = colorError();

	h1($tit);


	$cnt0 = count($fl);
	$rows0 = ceil($cnt0 / $cols);
	//dp("count: $cnt0");
	//dp("rows: $rows0");

	if ( strlen($res) > 0 ){
		echo "<h2><font color=#$colorError >Ошибка! $res</font></h2>\n";
	}else if ( $cnt0 > 0 ){
//		echo "<table border=1>\n";
		echo "<table>\n";
		echo "<tr>\n";
		$nn = 0;
		//dp("nn: $nn");

		foreach ( $fl as $f1 ){
//			dp("+");
			if ( $nn > 0 && ($nn % $cols) == 0 ){
				echo "</tr>\n<tr>\n";
			}
			echo "<td>"; //.$f1." ($nn) </td>\n";
			$fn = $f1['name'];
			$fp = $f1['path'];
			$fw = $f1['pathw'];
			$tw = $f1['thumb'];
			checkThumb($path, $fn);
			//dp("++");
			echo "<a class='gal-$gal' href='$fw' data-lightbox='gal-$gal' data-title='$fn'>";
//			echo $fw;
			echo "<img class='gal-image-$gal' src='$tw' alt='' />";
			echo "</a>";
			echo "</td>";
			$nn++;
		}

		echo "</tr>\n";
		echo "</table>\n";
	}else{
		echo "<h2><font color=#$colorError >Ошибка! $tit не найден</font></h2>\n";
	}


	echo "	<script src='/js/lightbox-plus-jquery.min.js'></script>\n";
	echo "	<script>\n";
	echo "		lightbox.option({\n";
	echo "			'resizeDuration': 200,\n";
	echo "			'positionFromTop': 10,\n";
	echo "			'fitImagesInViewport': true,\n";
	echo "			'wrapAround': false\n";
	echo "		})\n";
	echo "	</script>\n";

	h2();

?>
