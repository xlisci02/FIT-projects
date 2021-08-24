<?php

// Initialize the session
session_start();
 
// Check if the user is logged in, if not then redirect him to login page
if(!isset($_SESSION["loggedin"]) || $_SESSION["loggedin"] !== true){
	header("location: index.php");
	exit;
}
else if ($_SESSION["user_type"] != "vet"){
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
}

if(strlen($err_string)){
	header("location: error.php?text=$err_string");
	exit;
}

if($_SERVER["REQUEST_METHOD"] == "POST"){
 
 	if( !isset ($_POST["animal_id"]) ||
		!isset ($_POST["diagnosis"]) ||
		!isset ($_POST["date"]) ||
		!isset ($_POST["price"])){
		header("location: error.php?data_missing");
		exit;
	}

	$person_id = $_SESSION["person_id"];
	$animal_id = trim ($_POST["animal_id"]);
	$diagnosis = trim ($_POST["diagnosis"]);
	$date = trim ($_POST["date"]);
	$price = trim ($_POST["price"]);

	/* Switch off auto commit to allow transactions*/

	mysqli_autocommit($db, FALSE);
	$query_success = TRUE;
	
	// Prepare a select statement
	$sql = "INSERT INTO TREATMENT( PERSON_ID, ANIMAL_ID, DIAGNOSIS, DATE_OF_START, PRICE) VALUES 
			(?, ?, ?, STR_TO_DATE(?, '%Y-%m-%d'), ? )";

	if(($stmt = mysqli_prepare($db, $sql))){
		// Bind variables to the prepared statement as parameters
		mysqli_stmt_bind_param($stmt, "sssss", $person_id, $animal_id, $diagnosis, $date, $price);
		
		// Attempt to execute the prepared statement
		if(mysqli_stmt_execute($stmt)){}
		else {
			$err_string = "Sorry. Operation add treatment failed.";
			$query_success = FALSE;
		}
		mysqli_stmt_close($stmt);
	}
	if ($query_success) {
		$treatment_id = mysqli_insert_id($db);
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
		.wrapper{ width: 350px; padding: 20px; }
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
			<li class="active" > <a href="add_treatment.php">Add new treatment</a></li>
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
			<span class="glyphicon glyphicon-ok-circle"></span> Treatment was successfully added.
		</p>
		<p>
			<a href='treatment.php<?php echo (isset($treatment_id))?'?treatment_id='.$treatment_id:''?>' class='btn btn-default btn-sm'>
		  	<span class='glyphicon glyphicon-new-window'></span> View treatment detail</a>
		</p>
</div>
<div class="container-fluid <?php echo (!$query_success)?'':'hidden';?>" style='width:80%'>
	<h2>Add treatment</h2>
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

	<form id="Add" class="form-horizontal col-xs-11" action="add_treatment.php" method="post">

		<div class="panel-group form-group">
			<div class="panel panel-primary">
			<div class="panel-heading">
				<h4 class="panel-title">
				<a data-toggle="collapse" href="#collapse1">Choose animal (Click to hide/show the list).</a>
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

		<div class="form-group">
			<label class="control-label col-xs-2" for="diagnosis">Diagnosis:</label>
			<div class="col-xs-9"> 
				<input type="text" name="diagnosis" id="diagnosis" class="form-control" required maxlength="32">
			</div>
		</div>

		<div class="form-group">
			<label class="control-label col-xs-2" for="date">Date of start:</label>
			<div class="col-xs-3"> 
				<input type="date" name="date" class="form-control" required value="<?php echo date('Y-m-d'); ?>" max="<?php echo date('Y-m-d'); ?>"><br>
			</div>
		</div>

		<div class="form-group">
			<label class="control-label col-xs-2" for="price" >Price (&euro;):</label>
			<div class="col-xs-3"> 
				<input type="number" name="price" class="form-control" required min="0" step=0.01 ><br>
			</div>
		</div>

		<div class="form-group">
			<button type="submit" class="btn btn-success btn-sm pull-right "> CONTINUE </button>
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
		div = collapse1.getElementsByClassName("radio");
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