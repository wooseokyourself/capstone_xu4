<?php

  // Save 'outputs/*.jpeg' in @images with reverse order.
  $dir = __DIR__ . DIRECTORY_SEPARATOR . "outputs" . DIRECTORY_SEPARATOR;
  $images = glob ($dir . "*.jpeg", GLOB_BRACE);
  $images = array_reverse ($images);

  // Save the contents of 'outputs/results.txt' in @info with line by line, reverse order.
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
    <!-- Inputs Button -->
    <button
      onclick="location.href='inputs/'">
      View Previous Inputs here
    </button>

    <!-- Outputs Button -->
    <button
      onclick="location.href='outputs/'">
      View Previous Outputs here
    </button>


    <div id="outputs">
      <?php
        // Print the first element of the array.
        printf ("<br />");
        echo "<font size=12>".$info[0];
        printf ("<br />");
        printf ("<img src='outputs/%s'/>", basename($images[0]));
        printf ("<br />");

        printf ("<hr>");
        printf ("<br />");

        /*
        // Print the whole element of the array.
        $j = 0;
        foreach ($images as $i) {
          echo "<font size=12>".$info[$j++];
          printf ("<br />");
          printf ("<img src='outputs/%s'/>", basename($i));
          printf ("<br />");
        }
        */
      ?>
    </div>
  </body>
</html>