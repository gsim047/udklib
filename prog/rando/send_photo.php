<?php

include_once(__DIR__."/config.php");


$url_post = 'https://gsim047.ru'; // URL вашей страницы
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

echo $file . "\n";

$text_tg = '<a href="'.$url_post.'" target="_blank">'.$title.'</a>

'; // отступ нужен

$text_tg_des = mb_strimwidth($des, 0, 1000, "..."); // обрезаем текст

$text_tg .= $text_tg_des;


 // параметры, которые отправятся в api телеграмм
$params = array(
	'chat_id' => $myId, //'-1001442431172', // id получателя сообщения
	'photo' => new CURLFile($file), // путь к картинке
	'caption' => $info, // текст надписи
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

?>
