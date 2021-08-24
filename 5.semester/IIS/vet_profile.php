<?php
// Initialize the session
session_start();
 
// Check if the user is logged in, if not then redirect him to login page
if(!isset($_SESSION["loggedin"]) || $_SESSION["loggedin"] !== true){
	header("location: index.php");
	exit;
} 
else if($_SESSION["user_type"] != "vet"){
	header("location: error.php?permissions");
	exit;
}

// Include config file
require_once "config.php";

$vet_info = array();
$treatments = array();
$interventions = array();
$animals = array();
$err_string = "";

if($_SERVER["REQUEST_METHOD"] == "GET"){
	if(!isset($_GET["vet_id"])){
		header("location: error.php?data_missing");
		exit;

	}

	/* VET INFO */
	$sql = "SELECT `DEGREE`, `NAME`, ADDRESS, MAIL, IDENTIFICATION_NUMBER, BIRTHDATE, SEX
			FROM PERSON
			NATURAL JOIN`VET` 
			NATURAL JOIN `USERS`
			WHERE PERSON_ID = ?";

	if(($stmt = mysqli_prepare($db, $sql))){
		
		mysqli_stmt_bind_param($stmt, "s", $vet_id);
		
		$vet_id = (trim($_GET["vet_id"]))? trim($_GET["vet_id"]) : "";
		
		if(mysqli_stmt_execute($stmt)){
			// Store result
			mysqli_stmt_store_result($stmt);
			
			if(mysqli_stmt_num_rows($stmt) >= 1){
				// Bind result variables
				mysqli_stmt_bind_result($stmt, $degree, $name, $address, $mail, $identification_number, $birthdate, $sex);
				while(mysqli_stmt_fetch($stmt)){
					array_push($vet_info, array($degree, $name, $address, $mail, $identification_number, $birthdate, $sex));
				}
			}
			else $err_string = "Vet was not found!";
		}
		else $err_string = "Vet was not found!";
	}
	else $err_string = "Vet was not found!";
	
	if(strlen($err_string)){
		header("location: error.php?text=$err_string");
		exit;
	}


	/* TREATMENTS */
	$sql = "SELECT A.ANIMAL_ID, A.ANIM_NAME, T.DIAGNOSIS, T.DATE_OF_START, T.PRICE, T.TREATMENT_ID
			FROM TREATMENT T
			NATURAL JOIN VET V
			INNER JOIN ANIMAL A ON T.ANIMAL_ID = A.ANIMAL_ID
			WHERE T.PERSON_ID = ?";

	if(($stmt = mysqli_prepare($db, $sql))){

		mysqli_stmt_bind_param($stmt, "s", $vet_id);

		$vet_id = (trim($_GET["vet_id"]))? trim($_GET["vet_id"]) : "";
		
		if(mysqli_stmt_execute($stmt)){
			// Store result
			mysqli_stmt_store_result($stmt);
			
			if(mysqli_stmt_num_rows($stmt) >= 1){
				// Bind result variables
				mysqli_stmt_bind_result($stmt, $animal_id, $anim_name, $diagnosis, $date_of_start, $price, $treatment_id);
				while(mysqli_stmt_fetch($stmt)){
					array_push($treatments, array($animal_id, $anim_name, $diagnosis, $date_of_start, $price, $treatment_id));
				}
			}

		}

	}


	/* INTERVENTIONS WITH VET */
	$sql = "SELECT A.ANIMAL_ID, A.ANIM_NAME, I.TYPE, I.DOSAGE, I.DATE, I.SIDE_EFFECTS, I.PRICE
			FROM ANIMAL A
			NATURAL JOIN INTERVENTION I
			INNER JOIN VET V ON I.VET_ID = V.PERSON_ID
			WHERE V.PERSON_ID = ?";
  
	if(($stmt = mysqli_prepare($db, $sql))){
		
		mysqli_stmt_bind_param($stmt, "s", $vet_id);
		
		$vet_id = (trim($_GET["vet_id"]))? trim($_GET["vet_id"]) : "";
		
		if(mysqli_stmt_execute($stmt)){
			// Store result
			mysqli_stmt_store_result($stmt);
			
			if(mysqli_stmt_num_rows($stmt) >= 1){				   
				// Bind result variables
				mysqli_stmt_bind_result($stmt, $animal_id, $anim_name, $type, $dosage, $date, $side_effects, $price) ;
				while(mysqli_stmt_fetch($stmt)){
				   	array_push($interventions, array($animal_id, $anim_name, $type, $dosage, $date, $side_effects, $price));
				}
			}
		}
	}
	

	/* VET'S ANIMALS */
	$sql = "SELECT ANIMAL_ID, ANIM_NAME, SPECIE
			FROM PERSON
			NATURAL JOIN  `OWNER` 
			NATURAL JOIN  `ANIMAL` 
			NATURAL JOIN ANIMAL_SPECIES 
			WHERE PERSON_ID = ?";
  
	if(($stmt = mysqli_prepare($db, $sql))){
		
		mysqli_stmt_bind_param($stmt, "s", $vet_id);
		
		$vet_id = (trim($_GET["vet_id"]))? trim($_GET["vet_id"]) : "";
		
		if(mysqli_stmt_execute($stmt)){
			// Store result
			mysqli_stmt_store_result($stmt);
			
			if(mysqli_stmt_num_rows($stmt) >= 1){				   
				// Bind result variables
				mysqli_stmt_bind_result($stmt, $animal_id, $anim_name, $specie);
				while(mysqli_stmt_fetch($stmt)){
				   	array_push($animals, array($animal_id, $anim_name, $specie));
				}
			}
		}
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
			<li class="<?php echo ($_SESSION["user_type"] == "nurse")?'hidden':''; ?>"> <a href="add_treatment.php">Add new treatment</a></li>
			<li><a href="add_intervention.php">Add new intervention</a></li>
		</ul>
		<ul class="nav navbar-nav navbar-right">
			<li><a href="#"><span class="glyphicon glyphicon-user"></span> <?php echo htmlspecialchars($_SESSION["username"]); ?></a></li>
			<li><a href="logout.php"><span class="glyphicon glyphicon-log-in"></span> Logout</a></li>
		</ul>
	</div>
</nav>

<div class="container-fluid <?php echo (empty($vet_info))?'hidden':'' ?>" style="width: 70%">
	<div>
		<h3>Vet: <?php echo $vet_info[0][0].' '.$vet_info[0][1]?></h3>
	</div>
	<ul class="nav nav-tabs">
		<li class="active"><a data-toggle="tab" href="#home">Home</a></li>
		<li><a data-toggle="tab" href="#treatments">Treatments</a></li>
		<li><a data-toggle="tab" href="#interventions">Interventions</a></li>
		<li><a data-toggle="tab" href="#animals">Animals</a></li>
	</ul>
	<div class="tab-content">
		<div class="tab-pane fade in active" id="home">
			<hr>
			<form class="form" action="##" method="post" id="registrationForm">
				<div class="form-group">
						
					<div class="col-xs-6">
						<label><h4>Address</h4></label>
							<input readonly type="text" class="form-control" name="address" value="<?php echo $vet_info[0][2]?>">
					</div>
				</div>

				<div class="form-group">
							
					<div class="col-xs-6">
						<label><h4>Mail</h4></label>
							<input readonly type="email" class="form-control" name="mail" value="<?php echo $vet_info[0][3]?>" >
					</div>
				</div>

				<div class="form-group">
					<div class="col-xs-6">
						<label><h4>Identification number</h4></label>
						<input readonly type="text" class="form-control" name="id_num" value="<?php echo $vet_info[0][4]?>" >
					</div>
				</div>
		
				<div class="form-group">
					<div class="col-xs-6">
						<label><h4>Birthdate</h4></label>
						<input readonly type="email" class="form-control" name="birthdate" value="<?php echo $vet_info[0][5]?>" >
					</div>
				</div>

				<div class="form-group">
					<div class="col-xs-6">
						<label><h4>Sex</h4></label>
						<input readonly type="text" class="form-control" name="sex" value="<?php echo $vet_info[0][6]?>">
					</div>
				</div>
			</form>
			
			<hr>
			
		</div><!--/tab-pane-->

		<div class="tab-pane fade" id="treatments">

			<hr>
					
			<div class="table-responsive">
				<table class="table table-hover">
				<thead>
					<tr>
						<th>Animal name</th> 
						<th>Diagnosis</th> 
						<th>Date of start</th> 
						<th>Price (&euro;)</th>
						<th> </th>
					</tr>
				</thead>
				<tbody>
					<?php 
					if(!empty($treatments))
					foreach ($treatments as $row) {
						echo "<tr>";
						echo "<td><a href='animal_profile.php?animal_id=$row[0]'>$row[1]</a></td>";
						echo "<td>$row[2]</td>";
						echo "<td>$row[3]</td>";
						echo "<td>$row[4]</td>";
						echo "<td>
						<a target='_blank' href='treatment.php?treatment_id=$row[5]' class='btn btn-default btn-sm'>
		  				<span class='glyphicon glyphicon-new-window'></span> View detail
						</a></td>";
						echo "</tr>";
					}
					 ?>
				</tbody>
				</table>
			</div>
			 
		</div><!--/tab-pane-->
		<div class="tab-pane fade" id="interventions">

			 <hr>
				
			<div class="table-responsive">
				<table class="table table-hover">
				<thead>
					<tr>
						<th>Animal</th>
						<th>Type</th>
						<th>Dosage</th>
						<th>Date</th>
						<th>Side effects</th>
						<th>Price (&euro;)</th>
					</tr>
				</thead>
				<tbody>
					<?php 
					if(!empty($interventions))
					foreach ($interventions as $row) {
					echo "<tr>";
					echo "<td><a href='animal_profile.php?animal_id=$row[0]'>$row[1] </a></td>";
					echo "<td>$row[2]</td>";
					echo "<td>$row[3]</td>";				
					echo "<td>$row[4]</td>";
					echo "<td>$row[5]</td>";
					echo "<td>$row[6]</td>";							
					echo "</tr>";
					}
					?>

				</tbody>
				</table>
			</div>
							 
		</div><!--/tab-pane-->

		<div class="tab-pane fade" id="animals">

			<hr>
				
			<div class="table-responsive">
				<table class="table table-hover">
				<thead>
					<tr>
						<th>ANIMAL'S NAME</th>
						<th>SPECIE</th>
					</tr>
					</thead>
					<tbody>
					<?php 
					if(!empty($animals))
						foreach ($animals as $row) {
						echo "<tr>";	
						echo "<td><a href='animal_profile.php?animal_id=$row[0]'>$row[1]</a></td>";
						echo "<td>$row[2]</td>";
						echo "</tr>";
						}
					 ?>
				</tbody>
				</table>
			</div>
			 
		</div><!--/tab-pane-->
	</div><!--/tab-content-->

</div><!--/col-9-->
</div><!--/row-->


</body>
</html>