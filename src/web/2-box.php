<?php
// READ FILES FROM GALLERY FOLDER
$dir = __DIR__ . DIRECTORY_SEPARATOR . "outputs" . DIRECTORY_SEPARATOR;
$images = glob($dir . "*.{jpg,jpeg,gif,png}", GLOB_BRACE);

// DRAW HTML ?>
<!DOCTYPE html>
<html>
  <head>
    <title>Simple PHP gallery</title>
    <link href="2-box.css" rel="stylesheet">
    <script src="2-box.js"></script>
  </head>
  <body>
    <!-- [LIGHTBOX] -->
    <div id="lfront"></div>
    <div id="lback"></div>

    <!-- [THE GALLERY] -->
    <div id="outputs"><?php
    foreach ($images as $i) {
      printf("<img src='outputs/%s' onclick='outputs.show(this)'/>", basename($i));
    }
    ?></div>
  </body>
</html>