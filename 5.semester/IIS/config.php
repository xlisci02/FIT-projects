<?php

define('DB_SERVER', 'localhost');
define('DB_USERNAME', 'xlisci02');
define('DB_PASSWORD', 'ungebun9');
define('DB_NAME', 'xlisci02');
define('DB_PORT', 0);
define('DB_SOCKET', '/var/run/mysql/mysql.sock');

$db = mysqli_init();
$link = mysqli_real_connect($db, DB_SERVER, DB_USERNAME, DB_PASSWORD, DB_NAME, DB_PORT, DB_SOCKET);

if (!$link) {
    die('Connect Error (' . mysqli_connect_errno() . ') '
            . mysqli_connect_error());
} 

?>
