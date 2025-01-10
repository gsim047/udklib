<?php

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


function buts()
{
//	echo "<table width=100%><tr>";
//	if ( $root == 0 ){
//		echo "<td align=left><a href='/'><img src='/back.gif'></a></td>";
	echo "<a href='/'><img src='/home.png' style='border:2px solid black'></a>&nbsp;";
	echo "<a href='/ex.php?gal=scan'><img src='/pics.png' style='border:2px solid black'></a>&nbsp;";
	echo "<a href='/scan300.php'><img src='/scan300.png' style='border:2px solid black'></a>&nbsp;";
	echo "<a href='/scan400.php'><img src='/scan400.png' style='border:2px solid black'></a>&nbsp;";
	echo "<a href='/scan600.php'><img src='/scan600.png' style='border:2px solid black'></a>&nbsp;";
//	}
//	echo "<td align=right>";
//	echo '<!-- Yandex.Metrika informer --> <a href="https://metrika.yandex.ru/stat/?id=26971377&amp;from=informer" target="_blank" rel="nofollow"><img src="https://informer.yandex.ru/informer/26971377/1_1_CCCCCCFF_CCCCCCFF_0_pageviews" style="width:80px; height:15px; border:0;" alt="Яндекс.Метрика" title="Яндекс.Метрика: данные за сегодня (просмотры)" class="ym-advanced-informer" data-cid="26971377" data-lang="ru" /></a> <!-- /Yandex.Metrika informer --> <!-- Yandex.Metrika counter --> <script type="text/javascript" > (function(m,e,t,r,i,k,a){m[i]=m[i]||function(){(m[i].a=m[i].a||[]).push(arguments)}; m[i].l=1*new Date();k=e.createElement(t),a=e.getElementsByTagName(t)[0],k.async=1,k.src=r,a.parentNode.insertBefore(k,a)}) (window, document, "script", "https://mc.yandex.ru/metrika/tag.js", "ym"); ym(26971377, "init", { clickmap:true, trackLinks:true, accurateTrackBounce:true }); </script> <noscript><div><img src="https://mc.yandex.ru/watch/26971377" style="position:absolute; left:-9999px;" alt="" /></div></noscript> <!-- /Yandex.Metrika counter -->';
//	echo '<!-- Yandex.Metrika informer --> <a href="https://metrika.yandex.ru/stat/?id=62026240&amp;from=informer" target="_blank" rel="nofollow"><img src="https://informer.yandex.ru/informer/62026240/1_1_FFFFFFFF_EFEFEFFF_0_pageviews" style="width:80px; height:15px; border:0;" alt="Яндекс.Метрика" title="Яндекс.Метрика: данные за сегодня (просмотры)" class="ym-advanced-informer" data-cid="62026240" data-lang="ru" /></a> <!-- /Yandex.Metrika informer --> <!-- Yandex.Metrika counter --> <script type="text/javascript" > (function(m,e,t,r,i,k,a){m[i]=m[i]||function(){(m[i].a=m[i].a||[]).push(arguments)}; m[i].l=1*new Date();k=e.createElement(t),a=e.getElementsByTagName(t)[0],k.async=1,k.src=r,a.parentNode.insertBefore(k,a)}) (window, document, "script", "https://mc.yandex.ru/metrika/tag.js", "ym"); ym(62026240, "init", { clickmap:true, trackLinks:true, accurateTrackBounce:true }); </script> <noscript><div><img src="https://mc.yandex.ru/watch/62026240" style="position:absolute; left:-9999px;" alt="" /></div></noscript> <!-- /Yandex.Metrika counter -->';
//	echo "</td></tr></table>";
}

	
// start

	$dt = date("Ymd-His");
	//echo "scan-$dt.jpg\n\n";
	$addr = "snapscan:libusb:001:006";

	h1();
	buts();
	h2();

?>


