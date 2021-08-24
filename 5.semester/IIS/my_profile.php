<?php
// Initialize the session
session_start();
 
// Check if the user is logged in, if not then redirect him to login page
if(!isset($_SESSION["loggedin"]) || $_SESSION["loggedin"] !== true){
	header("location: index.php");
	exit;
}
else if ($_SESSION["user_type"] != "vet" &&  $_SESSION["user_type"] != "nurse"){
	header("location: error.php?text= No profile available for admin");
	exit;
}

$err_string = "";

if($_SERVER["REQUEST_METHOD"] == "GET"){
	if(!isset($_GET["user_id"])){
		header("location: welcome.php"); // ERR PAGE, you have not permission !!!
		exit;
	}
	$user_id = trim($_GET["user_id"]);

	require_once "config.php";

	if($_SESSION["user_type"] == "vet"){
		$sql = "SELECT PERSON_ID FROM USERS NATURAL JOIN VET WHERE USER_ID = ?";
		if(($stmt = mysqli_prepare($db, $sql))){
			
			mysqli_stmt_bind_param($stmt, "s", $user_id);
			
			$user_id = (trim($_GET["user_id"]))? trim($_GET["user_id"]) : "";
			
			if(mysqli_stmt_execute($stmt)){
				// Store result
				mysqli_stmt_store_result($stmt);
				
				if(mysqli_stmt_num_rows($stmt) == 1){
					// Bind result variables
					mysqli_stmt_bind_result($stmt, $vet_id);
					mysqli_stmt_fetch($stmt);
					
				}
				else $err_string = "Vet wasn't found!";
			}
			else $err_string = "Vet wasn't found!";
		}
		else $err_string = "Vet wasn't found!";
	
		if(strlen($err_string)){
			header("location: error.php?text=$err_string");
			exit;
		}
		else{
			header("location: vet_profile.php?vet_id=${vet_id}");
			exit;
		}
	}
	else{
		$sql = "SELECT PERSON_ID FROM USERS NATURAL JOIN NURSE WHERE USER_ID = ?";
		if(($stmt = mysqli_prepare($db, $sql))){
			
			mysqli_stmt_bind_param($stmt, "s", $user_id);
			
			$user_id = (trim($_GET["user_id"]))? trim($_GET["user_id"]) : "";
			
			if(mysqli_stmt_execute($stmt)){
				// Store result
				mysqli_stmt_store_result($stmt);
				
				if(mysqli_stmt_num_rows($stmt) == 1){
					// Bind result variables
					mysqli_stmt_bind_result($stmt, $nurse_id);
					mysqli_stmt_fetch($stmt);
					
				}
				else $err_string = "Nurse wasn't found!";
			}
			else $err_string = "Nurse wasn't found!";
		}
		else $err_string = "Nurse wasn't found!";
		if($err_string){
			header("location: error.php?text=$err_string");
			exit;
		}
		else		
		header("location: nurse_profile.php?nurse_id=${nurse_id}");	
	}
}
else
	header("location: index.php");

?>