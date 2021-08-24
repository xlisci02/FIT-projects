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
$animals = array();
$err_string = "";

if($_SERVER["REQUEST_METHOD"] == "GET"){
	$sql = "SELECT	ANIMAL_ID, ANIM_NAME, SPECIE, PERSON_ID, `NAME`
			FROM PERSON
			NATURAL JOIN	`OWNER` 
			NATURAL JOIN	`ANIMAL` 
			NATURAL JOIN ANIMAL_SPECIES ";
	
	if(isset($_GET["selected"])){
		$selected = $_GET["selected"];
		$sql .= "WHERE ANIMAL_ID = ?";
	}
	if(($stmt = mysqli_prepare($db, $sql))){
		
		if(isset($_GET["selected"]))
			mysqli_stmt_bind_param($stmt, "s", $selected);
		
		if(mysqli_stmt_execute($stmt)){
			// Store result
			mysqli_stmt_store_result($stmt);
			
			if(mysqli_stmt_num_rows($stmt) >= 1){					 
				// Bind result variables
				mysqli_stmt_bind_result($stmt, $animal_id, $anim_name, $specie, $person_id, $name) ;
				while(mysqli_stmt_fetch($stmt)){
					array_push($animals, array($animal_id, $anim_name, $specie, $person_id, $name));
				}
			}
			else $err_string = "No animal was found!";
		}
		else $err_string = "No animal was found!";
	}
	else $err_string = "No animal was found!";

	$collaborator_type = ($_SESSION["user_type"] == "vet")? "NURSE" : "VET";
	$collaborators = array();

	$sql = "SELECT	PERSON_ID, `DEGREE`, `NAME`
			FROM PERSON
			NATURAL JOIN USERS
			NATURAL JOIN ";
	$sql .= $collaborator_type;

	if(($stmt = mysqli_prepare($db, $sql))){

		if(mysqli_stmt_execute($stmt)){
			// Store result
			mysqli_stmt_store_result($stmt);
			
			if(mysqli_stmt_num_rows($stmt) >= 1){					 
				// Bind result variables
				mysqli_stmt_bind_result($stmt, $person_id, $degree, $name) ;
				while(mysqli_stmt_fetch($stmt)){
					array_push($collaborators, array($person_id, $degree, $name));
				}
			}
			
		}
	}
}

if(strlen($err_string)){
	header("location: error.php?text=$err_string");
	exit;
}

if($_SERVER["REQUEST_METHOD"] == "POST"){
 
	if(	!isset($_POST["animal_id"]) ||
		!isset($_POST["collaborator"]) ||
		!isset($_POST["type"]) ||
		!isset($_POST["dosage"]) ||
		!isset($_POST["date"]) ||
		!isset($_POST["side_effects"]) ||
		!isset($_POST["price"]) ){
		header("location: error.php?data_missing");
		exit;
	}


	$animal_id = $_POST["animal_id"];
	$collaborator = $_POST["collaborator"];
	$type = trim($_POST["type"]);
	$dosage = trim($_POST["dosage"]);
	$date = $_POST["date"];
	$side_effects = trim($_POST["side_effects"]);
	$price = $_POST["price"];
	$current_person = $_SESSION["person_id"];
	/* Switch off auto commit to allow transactions*/

	mysqli_autocommit($db, FALSE);
	$query_success = TRUE;
	
	$sql = "";
	// Prepare a select statement
	if($collaborator != 0){
		$sql = "INSERT INTO INTERVENTION(ANIMAL_ID, VET_ID, NURSE_ID, `TYPE`, DOSAGE, `DATE`, SIDE_EFFECTS, PRICE) 
					VALUES (?, ?, ?, ?, ?, STR_TO_DATE(?, '%Y-%m-%d'),?,?)";
	}
	else{
		if($_SESSION["user_type"] == "vet")
			$sql = "INSERT INTO INTERVENTION(ANIMAL_ID, VET_ID, `TYPE`, DOSAGE, `DATE`, SIDE_EFFECTS, PRICE) 
					VALUES (?, ?, ?, ?, STR_TO_DATE(?, '%Y-%m-%d'),?,?)" ;	
		else
			$sql = "INSERT INTO INTERVENTION(ANIMAL_ID, NURSE_ID, `TYPE`, DOSAGE, `DATE`, SIDE_EFFECTS, PRICE) 
					VALUES (?, ?, ?, ?, STR_TO_DATE(?, '%Y-%m-%d'),?,?)" ;
	}

	if(($stmt = mysqli_prepare($db, $sql))){
		// Bind variables to the prepared statement as parameters
		if($collaborator != 0){
			if($_SESSION["user_type"] == "vet")
				mysqli_stmt_bind_param($stmt, "ssssssss", $animal_id, $current_person, $collaborator, $type, $dosage, $date, $side_effects, $price);
			else
				mysqli_stmt_bind_param($stmt, "ssssssss", $animal_id, $collaborator, $current_person, $type, $dosage, $date, $side_effects, $price);
		}
		else
			mysqli_stmt_bind_param($stmt, "sssssss", $animal_id, $current_person, $type, $dosage, $date, $side_effects, $price);
		// Attempt to execute the prepared statement
		if(mysqli_stmt_execute($stmt)){}
		else {
			echo mysqli_error($db);
				$err_string = "Sorry. Operation add intervention failed.";
			$query_success = FALSE;
		}
		mysqli_stmt_close($stmt);
	}
	if ($query_success) {
		mysqli_commit($db);
	} 
	else {
		mysqli_rollback($db);
		//header("location: error.php?text=$err_string");
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
			<li><a href="search_owner.php">Search for owner</a></li>
			<li class="<?php echo ($_SESSION["user_type"] == "nurse")?'hidden':''; ?>"> <a href="add_treatment.php">Add new treatment</a></li>
			<li class="active" ><a href="add_intervention.php">Add new intervention</a></li>
		</ul>
		<ul class="nav navbar-nav navbar-right">
			<li><a href="#"><span class="glyphicon glyphicon-user"></span> <?php echo htmlspecialchars($_SESSION["username"]); ?></a></li>
			<li><a href="logout.php"><span class="glyphicon glyphicon-log-in"></span> Logout</a></li>
		</ul>
	</div>
</nav>

<div class="container-fluid text-center <?php echo ($query_success)?'':'hidden';?>">
		<p class= "lead">
			<span class="glyphicon glyphicon-ok-circle"></span> Intervention was successfully added.
		</p>
		<p>
			<a href='animal_profile.php<?php echo (isset($animal_id))?'?animal_id='.$animal_id:''?>' class='btn btn-default btn-sm'>
		  	<span class='glyphicon glyphicon-new-window'></span> Go see the intervention in animal's profile </a>
		</p>
</div>
<div class="container-fluid <?php echo (!$query_success)?'':'hidden';?>" style='width:80%'>
	<h2>Add intervention</h2>
	<form class="navbar-form navbar-left" action="#" method="get">
		
		<div class="form-group">
			<input style="width:200px;" id='search_anim_name' onkeyup="searchAnimal()" type="text" class="form-control" placeholder="Search by animal's name" autofocus>
	  	</div>
	  	<div class="form-group">
			<input style="width:200px;" id='search_specie' onkeyup="searchAnimal()" type="text" class="form-control" placeholder="Search by specie">
	  	</div>
	  	<div class="form-group">
			<input style="width:200px;" id='search_owner_name' onkeyup="searchAnimal() "type="text" class="form-control" placeholder="Search by owner's name">
	  	</div>
	</form>

	<form id="Add" class="form-horizontal col-xs-11" action="add_intervention.php" method="post">

		<div class="panel-group form-group">
			<div class="panel panel-primary">
			<div class="panel-heading">
				<h4 class="panel-title">
				<a data-toggle="collapse" href="#collapse1">Choose animal. (Click to hide/show the list).</a>
				</h4>
			</div>
			<div id="collapse1" class="panel-collapse collapse in">
			<?php
				foreach ($animals as $value) {
					echo "<div class='panel-body radio'><label><input type='radio' name='animal_id' checked value='$value[0]'>
					$value[1] / $value[2] / $value[4]<span class='hidden'>$value[1]/$value[2]/$value[4]</span></label></div>
					";
				}
			?>
				<div class="panel-footer">Animal not listed ? <a href="add_animal.php">Add new one.</a></div>
			</div>
			</div>
		</div>

		<div class="panel-group form-group">
			<div class="panel panel-primary">
			<div class="panel-heading">
				<h4 class="panel-title">
				<a data-toggle="collapse" href="#collapse2">Choose collaborator (Click to hide/show the list).</a>
				</h4>
			</div>
			<div id="collapse2" class="panel-collapse collapse">
				<div class='panel-body radio'><label><input type='radio' name='collaborator' checked value='0'>
					No collaborator.<span class='hidden'>No collaborator.</span></label></div>
					
			<?php
				foreach ($collaborators as $value) {
					echo "<div class='panel-body radio'><label><input type='radio' name='collaborator' value='$value[0]'>
					$value[1] $value[2]<span class='hidden'>
					$value[1] $value[2]</span></label></div>
					";
				}
			?>
			</div>
			</div>
		</div>

		<div class="form-group">
			<label class="control-label col-xs-2" for="type">Type:</label>
			<div class="col-xs-9"> 
				<input type="text" name="type" id="type" class="form-control" required maxlength="32">
			</div>
		</div>

		<div class="form-group">
			<label class="control-label col-xs-2" for="dosage">Dosage:</label>
			<div class="col-xs-9"> 
				<input type="text" name="dosage" id="dosage" class="form-control" maxlength="32">
			</div>
		</div>

		<div class="form-group">
			<label class="control-label col-xs-2" for="date">Date:</label>
			<div class="col-xs-3"> 
				<input type="date" name="date" class="form-control" required value="<?php echo date('Y-m-d'); ?>" max="<?php echo date('Y-m-d'); ?>"><br>
			</div>
		</div>

		<div class="form-group">
			<label class="control-label col-xs-2" for="side_effects">Side effects:</label>
			<div class="col-xs-3"> 
				<input type="text" name="side_effects" class="form-control" maxlength="50" ><br>
			</div>
		</div>

		<div class="form-group">
			<label class="control-label col-xs-2" for="price" >Price (&euro;):</label>
			<div class="col-xs-3"> 
				<input type="number" name="price" class="form-control" min="0" step=0.01><br>
			</div>
		</div>

		<div class="form-group">
			<button type="submit" class="btn btn-success btn-sm pull-right "> Add intervention </button>
		</div>
	</form>	
  </div>

	
	<script>
	function searchAnimal() {
		var  collapse, i, div, row, animal, specie;
		input_anim = document.getElementById("search_anim_name");
		input_specie = document.getElementById("search_specie");
		input_owner = document.getElementById("search_owner_name");

		filter_anim = input_anim.value.toUpperCase();
		filter_specie = input_specie.value.toUpperCase();
		filter_owner = input_owner.value.toUpperCase();
		
		collapse = document.getElementById("collapse1");
		div = collapse.getElementsByClassName("radio");
		for (i = 0; i < div.length; i++) {
			row = div[i].getElementsByTagName("span")[0];
			animal = (row.innerHTML.split("/")[0]);
			specie = (row.innerHTML.split("/")[1]);
			owner = (row.innerHTML.split("/")[2]);
			if(animal && specie && owner){
				if((animal.toUpperCase().indexOf(filter_anim) > -1) &&
					(specie.toUpperCase().indexOf(filter_specie) > -1) &&
					(owner.toUpperCase().indexOf(filter_owner) > -1)){
					div[i].style.display = "";
				}
				else{
					div[i].style.display = "none";
				}
			}
		}
	}
	</script>
</body>
</html>