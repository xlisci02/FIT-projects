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
	if(	!isset($_POST["address"]) || 
		!isset($_POST["mail"]) || 
		!isset($_POST["phone"]) || 
		!isset($_POST["id_num"]) || 
		!isset($_POST["birthdate"]) || 
		!isset($_POST["sex"]) ||
		!isset($_POST["name"]) ||
		!isset($_POST["person_id"])){
		
		header("location: error.php?data_missing");
		exit;
	}
	mysqli_autocommit($db, FALSE);
	$query_success = TRUE;

	$sql = "UPDATE PERSON SET `NAME`=?, IDENTIFICATION_NUMBER=?, BIRTHDATE=?, ADDRESS=?, SEX=? WHERE PERSON_ID = ?";
	
	if($stmt = mysqli_prepare($db, $sql)){

		mysqli_stmt_bind_param($stmt, "ssssss", $name, $identification_number, $birthdate, $address, $sex, $person_id);

		$address = trim($_POST["address"]);
		$identification_number = trim($_POST["id_num"]);
		$birthdate = trim($_POST["birthdate"]);
		$sex = trim($_POST["sex"]);
		$person_id = trim($_POST["person_id"]);
		$name = trim($_POST["name"]);
	
		if(mysqli_stmt_execute($stmt)){}
		else {	
			$err_string = "Sorry. Operation update person failed.";
			$query_success = FALSE;	
		}
		mysqli_stmt_close($stmt);
	}

	if($query_success){
		$sql = "UPDATE `OWNER` SET MAIL=?, PHONE_NUMBER=? WHERE PERSON_ID = ?";
		
		if($stmt = mysqli_prepare($db, $sql)){

			mysqli_stmt_bind_param($stmt, "sss", $mail, $phone, $person_id);

			$mail = trim($_POST["mail"]);
			$phone = trim($_POST["phone"]);
		
			if(mysqli_stmt_execute($stmt)){}
			else {	
				$err_string = "Sorry. Operation update person failed.";
				$query_success = FALSE;
			}
			mysqli_stmt_close($stmt);
		}
	}

	if ($query_success) {
		mysqli_commit($db);
		header("location: owner_profile.php?owner_id=$person_id");
	} 
	else {
		mysqli_rollback($db);
		header("location: error.php?text=$err_string");	
	}

}

?>