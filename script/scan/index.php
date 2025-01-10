<?php

	include_once("cfg.php");

	echo "<!DOCTYPE html><html lang='ru'>";
	echo "<head>";
	echo "	<meta HTTP-EQUIV='Content-Type' CONTENT='text/html; charset=utf-8'>";
	echo "	<title>my home</title>";
	echo "	<link rel='icon' href='favicon.ico' type='image/x-icon'>";
	echo "	<meta NAME='description' CONTENT=''>";
	echo "</head>";


	echo "<body>";

	yandexMetrika(1);

	echo "<h2>Документы</h2>";
//	echo "<br>";
	echo "» <a href='ls.php?lst=gm.lst'>Георгий Матвеевич</a><br>";
	echo "» <a href='ls.php?lst=vi.lst'>Вера Ильинична</a><br>";
	echo "» <a href='ls.php?lst=ba.lst'>Фото из бабушкиного альбома</a><br>";
	echo "<br>";

	echo "<h2>Школьные годы</h2>";
//	echo "<br>";
	echo "» <a href='ls.php?lst=no3.lst'>Школа №3</a> - Нижнекамск, 1972-78 гг<br><br>";
	echo "» <a href='ls.php?lst=no9.lst'>Школа №9</a> - Чайковский, 1978-80 гг<br>";
	echo "» <a href='ls.php?lst=bal.lst'>Бальные танцы</a> - там же, зима 1978-79<br>";
	echo "» <a href='https://fftor.ru/ls.php?lst=p.lst'>Поездка в Прибалтику</a> - там же, январь 1980<br>";
	echo "<br>";

	echo "<h2>Институт и после</h2>";
	echo "» <a href='ls.php?lst=20060416.lst'>Физтех...</a> - день открытых дверей 16.04.2006<br>";
	echo "<br>";

	echo "<br>";
	echo "<h2>extern links:</h2>";

	readLst("i_links.lst");

/*
	echo "<a href='https://unicode-table.com/ru/'>таблица unicode</a><br>";
	echo "<a href='/test/ysq'>тест YSQ</a>, ";
	echo "<a href='/test/smi'>тест SMI</a><br>";
	echo "<a href='https://sanstv.ru/words'>поиск слов</a><br>";
	echo "<a href='http://www.xiconeditor.com/'>редактор иконок</a><br>";
	echo "<br><hr><br>";

	echo "» <a href='http://192.168.0.114/'>local )</a><br>";
	echo "» <a href='https://fftor.ru/'>ФфТОР</a><br>";
*/
	echo "</body>";
	echo "</html>";

?>
