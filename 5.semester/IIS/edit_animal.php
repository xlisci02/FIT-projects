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
	if(	!isset($_POST["person_id"]) || 
		!isset($_POST["animal_species_id"]) || 
		!isset($_POST["sex"]) || 
		!isset($_POST["birthdate"]) ||
		!isset($_POST["anim_name"]) ||
		!isset($_POST["animal_id"]) || 
		!isset($_POST["last_examination"])){
		
		header("location: error.php?data_missing");
		exit;
	}

	mysqli_autocommit($db, FALSE);
	$query_success = TRUE;

	$sql = "UPDATE ANIMAL SET ANIMAL_SPECIES_ID=?, PERSON_ID=?, ANIM_NAME=?, ANIM_BIRTHDATE=?, LAST_EXAMINATION=?, ANIM_SEX =? WHERE ANIMAL_ID=?";

	if($stmt = mysqli_prepare($db, $sql)){

		mysqli_stmt_bind_param($stmt, "sssssss", $animal_species_id, $person_id, $anim_name, $birthdate, $last_examination, $sex, $animal_id); 

		$person_id = $_POST["person_id"];
		$animal_species_id = $_POST["animal_species_id"];
		$sex = $_POST["sex"];
		$birthdate = $_POST["birthdate"];
		$anim_name = trim($_POST["anim_name"]);
		$last_examination = $_POST["last_examination"];
		$animal_id = $_POST["animal_id"];

		if(mysqli_stmt_execute($stmt)){}
		else {	
			$err_string = "Sorry. Operation update animal failed.";
			$query_success = FALSE;	
		}
		mysqli_stmt_close($stmt);
	}

	if ($query_success) {
		mysqli_commit($db);
		header("location: animal_profile.php?animal_id=$animal_id");
	} 
	else {
		mysqli_rollback($db);
		header("location: error.php?text=$err_string");
	}



}

?>