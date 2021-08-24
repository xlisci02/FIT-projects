<?php

// Initialize the session
session_start();
 
// Check if the user is logged in, if not then redirect him to login page
if(!isset($_SESSION["loggedin"]) || $_SESSION["loggedin"] !== true){
	header("location: index.php");
	exit;
}
else if ($_SESSION["user_type"] != "vet" &&  $_SESSION["user_type"] != "nurse"){
	header("location: error.php?permissions");
	exit;
}

// Include config file
require_once "config.php";

$err_string = "";

if($_SERVER["REQUEST_METHOD"] == "POST"){
	if(	!isset($_POST["name"]) ||
		!isset($_POST["mail"]) ||		
		!isset($_POST["phone_number"]) ||
		!isset($_POST["identification_number"]) ||
		!isset($_POST["birthdate"]) ||
		!isset($_POST["address"]) ||
		!isset($_POST["sex"]) ){
			header("location: error.php?data_missing");
			exit;
	}


	mysqli_autocommit($db, FALSE);
	$query_success = TRUE;
	
	if (mysqli_query($db, "SET foreign_key_checks = 0") === FALSE) {
		$query_success = FALSE;
	}
	
	$sql = "INSERT INTO PERSON (`NAME`, IDENTIFICATION_NUMBER, BIRTHDATE, ADDRESS, SEX) VALUES (?, ?, STR_TO_DATE(?, '%Y-%m-%d'), ?, ?)";

	if(($stmt = mysqli_prepare($db, $sql))){
		mysqli_stmt_bind_param($stmt, "sssss", $name, $identification_number, $birthdate, $address, $sex);
		

		$address = trim($_POST["address"]);
		$name =  trim($_POST["name"]);
		$identification_number = trim($_POST["identification_number"]);
		$birthdate = $_POST["birthdate"];
		$sex = $_POST["sex"];
		// Attempt to execute the prepared statement
		if(mysqli_stmt_execute($stmt)){
			$person_id = mysqli_insert_id($db);

		}
		else {
			$err_string = "Sorry. Operation add person failed.";
			$query_success = FALSE;
		}
	}
	mysqli_stmt_close($stmt);

	if($query_success){
		$sql = "INSERT INTO `OWNER` (PERSON_ID, MAIL, PHONE_NUMBER) VALUES 
			(?, ?, ?)";

		if(($stmt = mysqli_prepare($db, $sql))){
			mysqli_stmt_bind_param($stmt, "sss", $person_id, $mail, $phone_number);
			
			$mail = trim($_POST["mail"]);
			$phone_number = trim($_POST["phone_number"]);

			// Attempt to execute the prepared statement
			if(mysqli_stmt_execute($stmt)){}
			else {
				$err_string = "Sorry. Operation add owner failed.";
				$query_success = FALSE;
			}
		}
		mysqli_stmt_close($stmt);
	}
	
	if (mysqli_query($db, "SET foreign_key_checks = 1") === FALSE) {
		$query_success = FALSE;
	}

	if ($query_success) {
		mysqli_commit($db);
	} 
	else {
		mysqli_rollback($db);
		header("location: error.php?text=$err_string");
		exit;
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
		.wrapper{ width: 750px; padding: 20px; }
	</style>
</head>
<body>
	
<nav class="navbar navbar-inverse">
	<div class="container-fluid">
			<div class="navbar-header">
				<a class="navbar-brand" href="welcome.php">VETclinIS</a>
			</div>
		<ul class="nav navbar-nav">
			<li><a href="welcome.php">Search for animal</a></li>
			<li class="active" ><a href="search_owner.php">Search for owner</a></li>
			<li class="<?php echo ($_SESSION["user_type"] == "nurse")?'hidden':''; ?>"> <a href="add_treatment.php">Add new treatment</a></li>
			<li><a href="add_intervention.php">Add new intervention</a></li>
		</ul>
		<ul class="nav navbar-nav navbar-right">
			<li><a href="#"><span class="glyphicon glyphicon-user"></span> <?php echo htmlspecialchars($_SESSION["username"]); ?></a></li>
			<li><a href="logout.php"><span class="glyphicon glyphicon-log-in"></span> Logout</a></li>
		</ul>
	</div>
</nav>

<div class="container-fluid text-center <?php echo ($query_success)?'':'hidden';?>">
		<p class= "lead">
			<span class="glyphicon glyphicon-ok-circle"></span> Owner was successfully added.
		</p>
		<p>
			<a href='add_animal.php<?php echo (isset($person_id))?'?selected='.$person_id:''?>' class='btn btn-default btn-sm'>
		  	<span class='glyphicon glyphicon-new-window'></span> Add animal to this owner.</a>
		</p>
</div>

<div class="container-fluid wrapper <?php echo (!$query_success)?'':'hidden';?> ">
	<form action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]); ?>" method="post">
	   <h2>Add owner </h2>

		<div class="form-group">
			<label>Name and Surname</label>
			<input type="text" name="name" class="form-control" maxlength="32" required>
		</div>

		<div class="form-group">
			<label>Mail</label>
			<input type="email" name="mail" class="form-control" maxlength="32" >
		</div>
	
		<div class="form-group">
			<label>Phone number</label>
			<input type="text" name="phone_number" class="form-control" maxlength="13">
		</div>
	
		<div class="form-group">
			<label>Identification number</label>
				<input type="text" name="identification_number" class="form-control" >
		</div>

		<div class="form-group">
			<label>Birthdate</label>
			<input type="date" name="birthdate" class="form-control" value="<?php echo date('Y-m-d'); ?>" max="<?php echo date('Y-m-d'); ?>">
		</div>

		<div class="form-group">
			<label>Address</label>
			<input type="text" name="address" class="form-control" maxlength="32" required>
		</div>

		<div class="form-group <?php echo (!empty($sex_err)) ? 'has-error' : ''; ?>">
			<label class="show">Sex</label>

			<label class="radio-inline">
				<input type="radio" name="sex"  checked>Male
			</label>
			<label class="radio-inline">
				<input type="radio" name="sex" >Female
			</label>
		</div>

		<div class="form-group">
				<input type="submit" class="btn btn-primary" >
				<input type="reset" class="btn btn-default" >
		</div>
	</form>
</div>



</body>
</html>