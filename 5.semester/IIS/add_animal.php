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

$animal_species = array();
$owners = array();
$err_string = "";

$sql = "SELECT ANIMAL_SPECIES_ID, SPECIE
		FROM ANIMAL_SPECIES";

if(($stmt = mysqli_prepare($db, $sql))){
	if(mysqli_stmt_execute($stmt)){
		// Store result
		mysqli_stmt_store_result($stmt);
		
		if(mysqli_stmt_num_rows($stmt) >= 1){				   
			// Bind result variables
			mysqli_stmt_bind_result($stmt, $animal_species_id, $specie);
			while(mysqli_stmt_fetch($stmt)){
				array_push($animal_species, array($animal_species_id, $specie));
			}
		}
		else $err_string = "No animal specie was found!";
	}
	else $err_string = "No animal specie was found!";
}
else $err_string = "No animal specie was found!";

mysqli_stmt_close($stmt);

if(strlen($err_string)){
	header("location: error.php?text=$err_string");
	exit;
}

$sql = "SELECT PERSON_ID,  `NAME` 
		FROM  `OWNER` 
		NATURAL JOIN PERSON";

if(($stmt = mysqli_prepare($db, $sql))){
			
	if(mysqli_stmt_execute($stmt)){
		// Store result
		mysqli_stmt_store_result($stmt);
		
		if(mysqli_stmt_num_rows($stmt) >= 1){					 
			// Bind result variables
			mysqli_stmt_bind_result($stmt, $person_id, $name);
			while(mysqli_stmt_fetch($stmt)){
				array_push($owners, array($person_id, $name));
			}
		}
		else $err_string = "No owner was found!";
	}
	else $err_string = "No owner was found!";
}
else $err_string = "No owner was found!";

mysqli_stmt_close($stmt);

if(strlen($err_string)){
	header("location: error.php?text=$err_string");
	exit;
}

if($_SERVER["REQUEST_METHOD"] == "GET"){
	if(isset($_GET["selected"])){
		$owner_id = $_GET["selected"];
	}
}
if($_SERVER["REQUEST_METHOD"] == "POST"){
	if(	!isset($_POST["animal_species_id"]) ||
		!isset($_POST["person_id"]) ||
		!isset($_POST["anim_name"]) ||
		!isset($_POST["anim_birthdate"]) ||
		!isset($_POST["last_examination"]) ||
		!isset($_POST["sex"]) ){
		header("location: error.php?data_missing");			
		exit;
	}

	mysqli_autocommit($db, FALSE);
	$query_success = TRUE;
	
	$sql = "INSERT INTO ANIMAL (ANIMAL_SPECIES_ID, PERSON_ID, ANIM_NAME, ANIM_BIRTHDATE, LAST_EXAMINATION, ANIM_SEX) VALUES
			(?,?,?, STR_TO_DATE(?,'%Y-%m-%d'), STR_TO_DATE(?,'%Y-%m-%d'), ?)";
	

	if(($stmt = mysqli_prepare($db, $sql))){
		mysqli_stmt_bind_param($stmt, "ssssss", $animal_species_id ,$person_id ,$anim_name ,$anim_birthdate ,$last_examination ,$anim_sex);
		
		$animal_species_id =  $_POST["animal_species_id"];
		$person_id = $_POST["person_id"];
		$anim_name = trim($_POST["anim_name"]);
		$anim_birthdate = $_POST["anim_birthdate"];
		$last_examination = $_POST["last_examination"];
		$anim_sex = $_POST["sex"];
		// Attempt to execute the prepared statement
		if(mysqli_stmt_execute($stmt)){}
		else {
			$err_string = "Sorry. Operation add animal failed.";
			$query_success = FALSE;
		}
		mysqli_stmt_close($stmt);
	}
	

	if ($query_success) {
		$animal_id = mysqli_insert_id($db);
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
			<li class="active" ><a href="welcome.php">Search for animal</a></li>
			<li><a href="search_owner.php">Search for owner</a></li>
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
			<span class="glyphicon glyphicon-ok-circle"></span> Animal was successfully added.
		</p>
		<p>
			<a href='add_treatment.php<?php echo (isset($animal_id))?'?selected='.$animal_id:''?>' class='btn btn-default btn-sm'>
		  	<span class='glyphicon glyphicon-new-window'></span> Add treatment to this animal</a>
		</p>
</div>

<div class="container-fluid wrapper <?php echo (!$query_success)?'':'hidden';?> ">
	<form action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]); ?>" method="post">
	   <h2>Add animal </h2>
	   <div class="form-group">
	      	<label for="sel1">Select animal specie:</label>
	    	<select class="form-control" name="animal_species_id">
			<?php
			foreach ($animal_species as $value) {
				echo "<option value='$value[0]'>$value[1]</option>";
			}
			?>
	    	</select>
	    	<br>
		</div>

		<div class="form-group">
	    	<label for="sel1">Select owner:</label> (Owner not listed ? <a href="add_owner.php">Add new owner.</a>)
	    	<select class="form-control" name="person_id">
			<?php
			foreach ($owners as $value) {
				if($value[0] == $owner_id)
					echo "<option value='$value[0]' selected >$value[1]</option>";
				else
					echo "<option value='$value[0]'>$value[1]</option>";
			}
			?>
	    	</select>
	    	<br>
		</div>

		<div class="form-group">
			<label>Animal name</label>
			<input type="text" name="anim_name" class="form-control"  maxlength="32" >
		</div>

		<div class="form-group">
			<label>Birthdate</label>
			<input type="date" name="anim_birthdate" class="form-control" value="<?php echo date('Y-m-d'); ?>" max="<?php echo date('Y-m-d'); ?>">
		</div>

		<div class="form-group">
			<label>Last examination</label>
			<input type="date" name="last_examination" class="form-control" value="<?php echo date('Y-m-d'); ?>" max="<?php echo date('Y-m-d'); ?>">
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