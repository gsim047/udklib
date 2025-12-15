<?php

include_once(__DIR__."/config.php");


//$url_post = 'https://gsim047.ru'; // URL вашей страницы
$title = 'Анкор ссылки'; // Текст анкора ссылки
$des = 'текст';


$info = '';

if ( $argc < 2 )
	exit(-1);

$file = $argv[1];
if ( !file_exists($file) )
	exit(-2);

if ( $argc > 2 )
	$info = $argv[2];

//echo $file . "\n";
$base = basename($file);
$s1 = substr($base, 0, 1);
$url_post = 'https://fftor.ru/';
if ( $s1 == "B" || $s1 == "b" ){
	$url_post .= "ex.php?gal=";
	$s2 = substr($base, 1, 3);
	if ( $s2 <= "020" ){
		$url_post .= "b001-020/b" . $s2;
	}else if ( $s2 <= "040" ){
		$url_post .= "b021-040/b" . $s2;
	}else if ( $s2 <= "060" ){
		$url_post .= "b041-060/b" . $s2;
	}else if ( $s2 <= "080" ){
		$url_post .= "b061-080/b" . $s2;
	}else if ( $s2 <= "100" ){
		$url_post .= "b081-100/b" . $s2;
	}else if ( $s2 <= "120" ){
		$url_post .= "b101-120/b" . $s2;
	}else if ( $s2 <= "140" ){
		$url_post .= "b121-140/b" . $s2;
	}else if ( $s2 <= "160" ){
		$url_post .= "b141-160/b" . $s2;
	}else if ( $s2 <= "180" ){
		$url_post .= "b161-180/b" . $s2;
	}else if ( $s2 <= "200" ){
		$url_post .= "b181-200/b" . $s2;
	}else if ( $s2 <= "220" ){
		$url_post .= "b201-220/b" . $s2;
	}
}else if ( $s1 == "A" || $s1 == "a" ){
	$s2 = substr($base, 1, 3);
	$url_post .= "ex.php?gal=a/a" . $s2;
}else if ( $s1 == "F" || $s1 == "f" ){
	$s2 = substr($base, 1, 3);
	$url_post .= "ex.php?gal=f/f" . $s2;
}

echo $url_post."\n";
//$url_post = 'https://fftor.ru/ex.php?gal='; // URL вашей страницы

//$text_tg = '<a href="'.$url_post.'" target="_blank">'.$title.'</a>
$text_tg = '<a href="'.$url_post.'" target="_blank">'.$info.'</a>

'; // отступ нужен

//$text_tg_des = mb_strimwidth($des, 0, 1000, "..."); // обрезаем текст
//$text_tg .= $text_tg_des;


 // параметры, которые отправятся в api телеграмм
$params = array(
	'chat_id' => $myId, //'-1001442431172', // id получателя сообщения
	'photo' => new CURLFile($file), // путь к картинке
	'caption' => $text_tg, //$info, // текст надписи
	'protect_content' => false, // значение true или false
	'parse_mode' => 'HTML', // режим отображения сообщения
);

 // параметры, которые отправятся в api телеграмм

$curl = curl_init();
curl_setopt($curl, CURLOPT_URL, 'https://api.telegram.org/bot' . $botId . '/sendPhoto'); // адрес api телеграмм
curl_setopt($curl, CURLOPT_POST, true); // отправка данных методом POST
curl_setopt($curl, CURLOPT_TIMEOUT, 20); // максимальное время выполнения запроса
curl_setopt($curl, CURLOPT_CONNECTTIMEOUT,20);
curl_setopt($curl, CURLOPT_POSTFIELDS, $params); // параметры запроса
curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);
curl_setopt($curl, CURLOPT_HEADER, false);
curl_setopt($curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
curl_setopt($curl, CURLOPT_SSL_VERIFYPEER, false);
curl_setopt($curl, CURLOPT_SSL_VERIFYHOST, false);
$result = curl_exec($curl); // запрос к api
curl_close($curl);  

echo $result . "\n";

$rsd = json_decode($result, true);
$r1 = $rsd["ok"];
printf("-----\n[$r1]\n");

if ( $r1 )
	exit(0);
exit(1);

?>
