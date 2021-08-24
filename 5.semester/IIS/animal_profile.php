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

$animal_info = array();
$treatments = array();
$interventions = array();
$owners = array();
$animal_species = array();
$animal_id;
$err_string = "";


	
if($_SERVER["REQUEST_METHOD"] == "GET"){
	if(!isset($_GET["animal_id"])){
		header("location: error.php?data_missing");
		exit;
	}

	/* OWNERS */
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

	/* ANIMAL SPECIES */ 
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

	/* ANIMAL INFO */
	$sql = "SELECT ANIM_NAME,  `NAME` , ADDRESS, SPECIE, ANIM_SEX, LAST_EXAMINATION, ANIM_BIRTHDATE, PERSON_ID
			FROM ANIMAL
			NATURAL JOIN PERSON
			NATURAL JOIN ANIMAL_SPECIES
			WHERE ANIMAL_ID = ?";
  
	if(($stmt = mysqli_prepare($db, $sql))){
		
		mysqli_stmt_bind_param($stmt, "s", $animal_id);
		
		$animal_id = (trim($_GET["animal_id"]))? trim($_GET["animal_id"]) : "";
		
		if(mysqli_stmt_execute($stmt)){
			// Store result
			mysqli_stmt_store_result($stmt);
			
			if(mysqli_stmt_num_rows($stmt) == 1){				   
				// Bind result variables
				mysqli_stmt_bind_result($stmt, $anim_name, $name, $address, $specie, $anim_sex, $last_examination, $anim_birthdate, $person_id);
				if(mysqli_stmt_fetch($stmt)){
					array_push($animal_info, $anim_name, $name, $address, $specie, $anim_sex, $last_examination, $anim_birthdate, $person_id);
				}
			}
			else $err_string = "Animal doesn't exist!";
		}
		else $err_string = "Animal doesn't exist!";
	}
	else $err_string = "Animal doesn't exist!";
	
	if(strlen($err_string)){
		header("location: error.php?text=$err_string");
		exit;
	}

	/* TREATMENTS */
	$sql = "SELECT DEGREE, `NAME` , DIAGNOSIS, DATE_OF_START, PRICE, PERSON_ID, TREATMENT_ID
			FROM PERSON
			NATURAL JOIN VET
			NATURAL JOIN TREATMENT
			WHERE ANIMAL_ID = ?";
  
	if(($stmt = mysqli_prepare($db, $sql))){
		
		mysqli_stmt_bind_param($stmt, "s", $animal_id);
		
		$animal_id = (trim($_GET["animal_id"]))? trim($_GET["animal_id"]) : "";
		
		if(mysqli_stmt_execute($stmt)){
			// Store result
			mysqli_stmt_store_result($stmt);
			
			if(mysqli_stmt_num_rows($stmt) >= 1){				   
				// Bind result variables
				mysqli_stmt_bind_result($stmt, $degree, $name, $diagnosis, $date_of_start, $price, $person_id, $treatment_id);
				while(mysqli_stmt_fetch($stmt)){
					array_push($treatments, array($degree, $name, $diagnosis, $date_of_start, $price, $person_id, $treatment_id));
				}
			}
		}
	}
	
   

   	/* INTERVENTIONS WITH NURSES */
	$sql = "SELECT INTERVENTION_ID, DEGREE, `NAME`, `TYPE`, DOSAGE, `DATE`, SIDE_EFFECTS, PRICE, NURSE_ID
			FROM PERSON P INNER JOIN NURSE N on P.PERSON_ID=N.PERSON_ID 
			INNER JOIN INTERVENTION I ON P.PERSON_ID=I.NURSE_ID
			WHERE ANIMAL_ID = ?";
  
	if(($stmt = mysqli_prepare($db, $sql))){
		
		mysqli_stmt_bind_param($stmt, "s", $animal_id);
		
		$animal_id = (trim($_GET["animal_id"]))? trim($_GET["animal_id"]) : "";
		
		if(mysqli_stmt_execute($stmt)){
			// Store result
			mysqli_stmt_store_result($stmt);
			
			if(mysqli_stmt_num_rows($stmt) >= 1){				   
				// Bind result variables
				mysqli_stmt_bind_result($stmt, $intervention_id, $nurse_degree, $nurse_name, $type, $dosage, $date, $side_effects, $price, $nurse_id);
				while(mysqli_stmt_fetch($stmt)){
					$vet_degree = "";
					$vet_name = "";
					$vet_id = 0;
					$interventions[$intervention_id] = array($vet_id, $vet_degree, $vet_name, $nurse_id, $nurse_degree, $nurse_name, $type, $dosage, $date, $side_effects, $price);
				}
			}
		}
	}

   	/* INTERVENTIONS WITH VETS */
	$sql = "SELECT INTERVENTION_ID, DEGREE, `NAME`, `TYPE`, DOSAGE, `DATE`, SIDE_EFFECTS, PRICE, VET_ID
			FROM PERSON P INNER JOIN VET N on P.PERSON_ID=N.PERSON_ID 
			INNER JOIN INTERVENTION I ON P.PERSON_ID=I.VET_ID
			WHERE ANIMAL_ID = ?";
  
	if(($stmt = mysqli_prepare($db, $sql))){
		
		mysqli_stmt_bind_param($stmt, "s", $animal_id);
		
		$animal_id = (trim($_GET["animal_id"]))? trim($_GET["animal_id"]) : "";
		
		if(mysqli_stmt_execute($stmt)){
			// Store result
			mysqli_stmt_store_result($stmt);
			
			if(mysqli_stmt_num_rows($stmt) >= 1){				   
				// Bind result variables
				mysqli_stmt_bind_result($stmt, $intervention_id, $vet_degree, $vet_name, $type, $dosage, $date, $side_effects, $price, $vet_id);
				while(mysqli_stmt_fetch($stmt)){
					$nurse_degree = "";
					$nurse_name = "";
					$nurse_id = 0;
					if(isset($interventions[$intervention_id])){
						$interventions[$intervention_id][0] = $vet_id;;
						$interventions[$intervention_id][1] = $vet_degree;
						$interventions[$intervention_id][2] = $vet_name.",";
					}
					else
						$interventions[$intervention_id] = array($vet_id, $vet_degree, $vet_name, $nurse_id, $nurse_degree, $nurse_name, $type, $dosage, $date, $side_effects, $price);
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

<div class="container-fluid <?php echo (empty($animal_info))?'hidden':'' ?>" style="width: 70%">
			<div>
				<h3>Animal: <?php echo $animal_info[0]?></h3>
			</div>
			<ul class="nav nav-tabs">
				<li class="active"><a data-toggle="tab" href="#home">Home</a></li>
				<li><a data-toggle="tab" href="#treatments">Treatments</a></li>
				<li><a data-toggle="tab" href="#interventions">Interventions</a></li>
			</ul>

			  
		  <div class="tab-content">
			<div class="tab-pane fade in active" id="home">
				<hr>
				  <form id="form" class="form" action="edit_animal.php" method="post">
					  <input type="hidden" name="anim_name" value="<?php echo $animal_info[0]?>"> 
				  	  <input type="hidden" name="animal_id" value="<?php echo $animal_id?>">

					  <div class="form-group">
						  <div class="col-xs-6">
							<label><h4>Owner:</h4></label>
					    	<select disabled class="form-control" name="person_id">
							<?php
							foreach ($owners as $value) {
								if($value[0] == $animal_info[7])
									echo "<option value='$value[0]' selected >$value[1]</option>";
								else
									echo "<option value='$value[0]'>$value[1]</option>";
							}
							?>
					    	</select>
						  </div>
					  </div>
	  
					  <div class="form-group">
						  
						  <div class="col-xs-6">
						  		<label><h4>Specie</h4></label>
						    	<select disabled class="form-control" name="animal_species_id">
								<?php
								foreach ($animal_species as $value) {
									if($value[1] == $animal_info[3])
										echo "<option value='$value[0]' selected >$value[1]</option>";
									else
										echo "<option value='$value[0]'>$value[1]</option>";
								}
								?>
						    	</select>
						  </div>
					  </div>
		  
					  <div class="form-group">
						  <div class="col-xs-6">
							 <label><h4>Sex</h4></label>
							  <input readonly type="text" class="form-control" name="sex" value="<?php echo $animal_info[4]?>" pattern="M|F" >
						  </div>
					  </div>
					  <div class="form-group">
						  
						  <div class="col-xs-6">
							  <label><h4>Last examination</h4></label>
							  <input readonly type="date" class="form-control" name="last_examination" value="<?php echo $animal_info[5]?>" max="<?php echo $animal_info[5]?>">
						  </div>
					  </div>
					  <div class="form-group">
						  
						  <div class="col-xs-6">
							  <label><h4>Birthdate</h4></label>
							  <input readonly type="date" name="birthdate" class="form-control" value="<?php echo $animal_info[6]?>" max="<?php echo $animal_info[6]?>">
						  </div>
					  </div>
					  <div class="form-group">
						   <div class="col-xs-12">
								<br>
							<button onclick="makeEditable()"class="btn btn-sm btn-primary" type="button"><i class="glyphicon glyphicon-edit"></i> Edit</button>
							<button id="save" class="btn btn-sm btn-success hidden" type="submit"><i class="glyphicon glyphicon-edit"></i> Save </button>
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
							<th>Vet</th>
							<th>Diagnosis</th>
							<th>Date of start</th>
							<th>Price (&euro;)</th>
						</tr>
					</thead>
					<tbody>
						<?php 
						if(!empty($treatments))
							foreach ($treatments as $row) {
							echo "<tr>";	
							echo "<td><a href='vet_profile.php?vet_id=$row[5]'>$row[0] $row[1]</a></td>";
							echo "<td>$row[2]</td>";
							echo "<td>$row[3]</td>";
							echo "<td>$row[4]</td>";
							echo "<td>
							<a target='_blank' href='treatment.php?treatment_id=$row[6]' class='btn btn-default btn-sm'>
			  				<span class='glyphicon glyphicon-new-window'></span> View detail
							</a></td>";
							echo "</tr>";
							}
						 ?>
					</tbody>
					</table>

				</div>
				<a href="add_treatment.php?selected=<?php echo $animal_id?>" class="btn btn-sm btn-success" type="button"><i class="glyphicon glyphicon-plus"></i> Add treatment </a>

			 </div><!--/tab-pane-->
			 <div class="tab-pane fade" id="interventions">

			   <hr>
				  
			  	<div class="table-responsive">
					<table class="table table-hover">
						<thead>
						<tr>
							<th>Executor(s)</th>
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
							echo "<td><a href='vet_profile.php?vet_id=$row[0]'>$row[1] $row[2]</a> <a href='nurse_profile.php?nurse_id=$row[3]'>$row[4] $row[5]</td>";
							echo "<td>$row[6]</td>";
							echo "<td>$row[7]</td>";							
							echo "<td>$row[8]</td>";
							echo "<td>$row[9]</td>";
							echo "<td>$row[10]</td>";
							echo "</tr>";
							}
						 ?>

					</tbody>
					</table>
				</div>
					<a href="add_intervention.php?selected=<?php echo $animal_id?>" class="btn btn-sm btn-success" type="button"><i class="glyphicon glyphicon-plus"></i> Add intervention </a>		 
			  </div><!--/tab-pane-->
		  </div><!--/tab-content-->

		</div><!--/col-9-->
	</div><!--/row-->

<script>
	function makeEditable(){
		var form = document.getElementById('form');
		var select = form.getElementsByTagName("select");
		for (var i = 0; i < select.length; i++) {
			if(select[i].disabled == true){
			select[i].disabled = false;
		}
		else
			select[i].disabled = true;
		}

		inputs = form.getElementsByTagName("input");
		for (var i = 0; i < inputs.length; i++) {
			if(inputs[i].readOnly == true)
				inputs[i].readOnly = false;
			else
				inputs[i].readOnly = true;
		}
		save_btn = document.getElementById("save");

		if(save_btn.classList.contains("hidden"))
			save_btn.classList.remove("hidden");
		else
			save_btn.classList.add("hidden");
	}
</script>
</body>
</html>