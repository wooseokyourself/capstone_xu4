<?php
/* outputs/ 에 있는 .jpeg 파일을 역순으로 배열에 저장 */
$dir = __DIR__ . DIRECTORY_SEPARATOR . "outputs" . DIRECTORY_SEPARATOR;
$images = glob ($dir . "*.{jpg,jpeg,gif,png}", GLOB_BRACE);
$images = array_reverse ($images);

/* outputs/results.txt 의 내용을 역순으로 한 줄씩 배열에 저장 */
/* $txtfp = fopen ("outputs/results.txt", "r"); */
$info = file("outputs/results.txt");
$info = array_reverse ($info);

// DRAW HTML ?>
<!DOCTYPE html>
<html>
  <head>
    <title>People Counter</title>
    <link href="index.css" rel="stylesheet">
  </head>
  <body>
    <!-- [THE GALLERY] -->
    <div id="outputs"><?php
    /*
    for ($i = 1; $i <= count($dir); $i++) {
      printf ("<img src='outputs/%s'/>", )
    */
    $j = 0;
    foreach ($images as $i) {
      printf ("%s", $info[$j++]);
      printf ("<br />");
      printf ("<img src='outputs/%s'/>", basename($i));
      printf ("<br />");
    }
    ?></div>
  </body>
</html>