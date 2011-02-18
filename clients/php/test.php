<?php

$fs = new Fstorage("fstp://127.0.0.1:8001", 0, "");

$key = "fx2";

$fs->file_send($key, "./pic.jpg");

echo  "HASH = " .Fstorage::hash($key) . "\n";

//$fs->file_remove("dupa1");

?>
