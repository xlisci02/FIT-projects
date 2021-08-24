<?php 
// Initialize the session
session_start();
 
// Check if the user is logged in, if not then redirect him to login page
if(!isset($_SESSION["loggedin"]) || $_SESSION["loggedin"] !== true ){
	header("location: index.php");
	exit;
}
else if ($_SESSION["username"] != "admin"){
	header("location: error.php?permissions");
	exit;
}

// Include config file
require_once "config.php";
$err_string = "";

if($_SERVER["REQUEST_METHOD"] == "POST"){
	mysqli_autocommit($db, FALSE);
	$query_success = TRUE;
	echo ($query_success)?"true":"false";

	$sql = "SET foreign_key_checks = 0;";
	if($stmt = mysqli_prepare($db, $sql)){
		if(!mysqli_stmt_execute($stmt)){
		 	$err_string = "Sorry. Database setting error.";
		 	$query_success = FALSE;
		}
	}

	$sql = "DELETE FROM USERS WHERE USER_ID = ?";
	if($stmt = mysqli_prepare($db, $sql)){
		mysqli_stmt_bind_param($stmt, "s", $user_id);

		$user_id = $_POST["user_id"];
		if(mysqli_stmt_execute($stmt)){}
		else{
			$err_string = "No user with this ID";
			$query_success = FALSE;
		}
	}

	$sql = "SET foreign_key_checks = 1;";
	if($stmt = mysqli_prepare($db, $sql)){
		if(!mysqli_stmt_execute($stmt)){
		 	$err_string = "Sorry. Database setting error.";
		 	$query_success = FALSE;
		}
	}
	if ($query_success) {
		mysqli_commit($db);
		if($_POST["user"] == "vet")
			header("location: welcome.php");
		else if($_POST["user"] == "nurse")
			header("location: show_nurses.php");
		exit;
		
	} 
	else {
		mysqli_rollback($db);
		header("location: error.php?text=$err_string");
		exit;
	}
}


?>