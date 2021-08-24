<?php

// Initialize the session
session_start();
 
// Check if the user is logged in, if not then redirect him to login page
if(!isset($_SESSION["loggedin"]) || $_SESSION["loggedin"] !== true ){
	header("location: index.php");
	exit;
}

// Include config file
require_once "config.php";
$err_string = "Ooops. Something went wrong.";

if($_SERVER["REQUEST_METHOD"] == "GET"){
	if(isset($_GET["permissions"])){
		$err_string	= "You don't have permissions to execute this action.";
	}
	else if(isset($_GET["text"])){
		$err_string	= $_GET["text"];
	}
	else if(isset($_GET["data_missing"])){
		$err_string = "Requested data wasn't inserted.";
	}
}

?>
	
<!DOCTYPE html>
<html lang="en">
<head>
	<link rel="shortcut icon" type="image/png" href="icon.png">
	<title>VETclinIS</title>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css">
	<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js"></script>
	<script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js"></script>
	<style type="text/css">
		body{ font: 14px sans-serif; }
		.wrapper{ width: 350px; padding: 20px; }
	</style>
</head>
<body>

<nav class="navbar navbar-inverse">
	<div class="container-fluid">
		<div class="navbar-header">
			<a class="navbar-brand" href="welcome.php">VETclinIS</a>
		</div>
		
		<ul class="nav navbar-nav navbar-right">
			<li><a href="#"><span class="glyphicon glyphicon-user"></span> <?php echo htmlspecialchars($_SESSION["username"]); ?></a></li>
			<li><a href="logout.php"><span class="glyphicon glyphicon-log-in"></span> Logout</a></li>
		</ul>
	</div>
</nav>

<div class="container-fluid text-center ">
		<p class= "lead">
			<span class="glyphicon glyphicon-ban-circle"></span> <?php echo $err_string; ?>
		</p>
		<p>
			<a href='index.php' class='btn btn-default btn-sm'>
		  	<span class='glyphicon glyphicon-new-window'></span> Go to main page.</a>
		</p>

</body>
</html>