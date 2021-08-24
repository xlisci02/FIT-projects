<?php 
// Initialize the session
session_start();
 
// Check if the user is logged in, if not then redirect him to login page
if(!isset($_SESSION["loggedin"]) || $_SESSION["loggedin"] !== true ){
	header("location: index.php");
	exit;
}
else if( $_SESSION["user_type"] != "admin"){
	header("location: error.php?permissions");
	exit;
}


// Include config file
require_once "config.php";

if($_SERVER["REQUEST_METHOD"] == "POST"){
	if(	!isset($_POST["user"]) ||
		!isset($_POST["degree"]) ||
		!isset($_POST["account_number"]) ||
		!isset($_POST["hourly_wage"]) ||
		!isset($_POST["person_id"]) ||
		!isset($_POST["name"]) ||
		!isset($_POST["identification_number"]) ||
		!isset($_POST["birthdate_year"]) ||
		!isset($_POST["birthdate_month"]) ||
		!isset($_POST["birthdate_day"]) ||
		!isset($_POST["address"]) ||
		!isset($_POST["sex"]) ||
		!isset($_POST["mail"]) ||
		!isset($_POST["user_id"]) ){
			header("location: error.php?data_missing");
			exit;
	}
	mysqli_autocommit($db, FALSE);
	$query_success = TRUE;
	

	if($_POST["user"] == "vet")
		$sql = "UPDATE VET SET DEGREE=?, ACCOUNT_NUMBER=?, HOURLY_WAGE=? WHERE PERSON_ID = ?";
	else if($_POST["user"] == "nurse")
		$sql = "UPDATE NURSE SET DEGREE=?, ACCOUNT_NUMBER=?, HOURLY_WAGE=? WHERE PERSON_ID = ?";
	else 
		return;

	if($stmt = mysqli_prepare($db, $sql)){

		mysqli_stmt_bind_param($stmt, "ssss", $degree, $account_number, $hourly_wage, $person_id);
		$degree = trim($_POST["degree"]);
		$account_number = trim($_POST["account_number"]);
		$hourly_wage = trim($_POST["hourly_wage"]);
		$person_id = $_POST["person_id"];

		if(mysqli_stmt_execute($stmt)){}
		else {
			$err_string = "Sorry. Operation update vet | nurse failed.";
			$query_success = FALSE;
		}
		mysqli_stmt_close($stmt);
	}

	if($query_success){

		$sql = "UPDATE PERSON SET `NAME`=?, IDENTIFICATION_NUMBER=?, BIRTHDATE=?, ADDRESS=?, SEX=? WHERE PERSON_ID = ?";
		if($stmt = mysqli_prepare($db, $sql)){

			mysqli_stmt_bind_param($stmt, "ssssss", $name, $identification_number, $birthdate, $address, $sex, $person_id);

			$name = trim($_POST["name"]);
			$identification_number = trim($_POST["identification_number"]);
			$birthdate = trim($_POST["birthdate_year"])."-".trim($_POST["birthdate_month"])."-".trim($_POST["birthdate_day"]);
			$address = trim($_POST["address"]);
			$sex = trim($_POST["sex"]);


			if(mysqli_stmt_execute($stmt)){}
			else {
				$err_string = "Sorry. Operation update person failed.";
				$query_success = FALSE;
			}
			mysqli_stmt_close($stmt);
		}
	}

	if($query_success){
		$sql = "UPDATE USERS SET MAIL=? WHERE USER_ID = ?";
		if($stmt = mysqli_prepare($db, $sql)){

			mysqli_stmt_bind_param($stmt, "ss", $mail, $user_id);
			$mail = trim($_POST["mail"]);
			$user_id = $_POST["user_id"];
			if(mysqli_stmt_execute($stmt)){}
			else {
				$err_string = "Sorry. Operation update user failed.";
				$query_success = FALSE;
			}
			mysqli_stmt_close($stmt);
		}
	}

	if ($query_success) {
		mysqli_commit($db);
	} 
	else {
		mysqli_rollback($db);
		header("location: error.php?text=$err_string");
		exit;
	}
	if($_POST["user"] == "vet")
		header("location: show_vets.php");	
	else if($_POST["user"] == "nurse")
		header("location: show_nurses.php");
}
else
	header("location: index.php");
exit;
?>