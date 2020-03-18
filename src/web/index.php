<?php
/* outputs/ 에 있는 .jpeg 파일을 역순으로 배열에 저장 */
$dir = __DIR__ . DIRECTORY_SEPARATOR . "outputs" . DIRECTORY_SEPARATOR;
$images = glob ($dir . "*.{jpg,jpeg,gif,png}", GLOB_BRACE);
$images = array_reverse ($images);

/* outputs/results.txt 을 역순으로 한 줄씩 배열에 저장 */
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
    
    /* 배열의 첫 번째 원소 출력 */
    echo "<font size=12>".$info[0];
    printf ("<br />");
    printf ("첫 번째 원소: %s", $images[0]);
    printf ("<br />");
    printf ("<img src='outputs/%s'/>", basename($images[0]));
    printf ("<br />");

    printf ("<hr>");
    printf ("<br />");

    $j = 0;
    foreach ($images as $i) {
      echo "<font size=12>".$info[$j++];
      printf ("<br />");
      printf ("<img src='outputs/%s'/>", basename($i));
      printf ("<br />");
    }
    ?></div>
  </body>
</html>