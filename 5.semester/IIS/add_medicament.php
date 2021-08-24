<?php

// Initialize the session
session_start();
 
// Check if the user is logged in, if not then redirect him to login page
if(!isset($_SESSION["loggedin"]) || $_SESSION["loggedin"] !== true || $_SESSION["username"] != "admin"){
	header("location: index.php");
	exit;
}
else if ($_SESSION["user_type"] != "admin"){
	header("location: error.php?permissions");
	exit;
}
// Include config file
require_once "config.php";
$medicament_types = array();
$animal_species = array();
$err_string = "";

$sql = "SELECT MEDICAMENT_TYPE_ID, ATC_CLASIFICATION, MODE_OF_ACTION
		FROM MEDICAMENT_TYPE";

if(($stmt = mysqli_prepare($db, $sql))){
								
	if(mysqli_stmt_execute($stmt)){
		// Store result
		mysqli_stmt_store_result($stmt);
		
		if(mysqli_stmt_num_rows($stmt) >= 1){
			// Bind result variables
			mysqli_stmt_bind_result($stmt, $medicament_type_id, $atc_clasification, $mode_of_action) ;

			while(mysqli_stmt_fetch($stmt)){
				array_push($medicament_types, array($medicament_type_id, $atc_clasification, $mode_of_action));
			}
		}
		else $err_string = "No medicament type was found!";
	}
	else $err_string = "No medicament type was found!";
}
else $err_string = "No medicament type was found!";


if(strlen($err_string)){
	header("location: error.php?text=$err_string");
	exit;
}

$sql = "SELECT ANIMAL_SPECIES_ID, SPECIE
		FROM ANIMAL_SPECIES";

if(($stmt = mysqli_prepare($db, $sql))){
							
	if(mysqli_stmt_execute($stmt)){
		// Store result
		mysqli_stmt_store_result($stmt);
		
		if(mysqli_stmt_num_rows($stmt) >= 1){
			// Bind result variables
			mysqli_stmt_bind_result($stmt, $animal_species_id, $specie)  ;

			while(mysqli_stmt_fetch($stmt)){
				array_push($animal_species, array($animal_species_id, $specie));
			}
		}
		else $err_string = "No animal specie was found!";
	}
	else $err_string = "No animal specie was found!";
}
else $err_string = "No animal specie was found!";


if(strlen($err_string)){
	header("location: error.php?text=$err_string");
	exit;
}

if($_SERVER["REQUEST_METHOD"] == "POST"){

	if( !isset($_POST["medicament_type_id"]) ||
		!isset($_POST["name"]) ||
		!isset($_POST["contraindication"]) ||
		!isset($_POST["active_substance"]) ||
		!isset($_POST["side_effects"]) ||
		!isset($_POST["animal_species_id"]) ||
		!isset($_POST["dose"]) ||
		!isset($_POST["disease"]) ){

		header("location: error.php?data_missing");
		exit;
	}

	$medicament_type_id = $_POST["medicament_type_id"];
	$name = trim($_POST["name"]);
	$contraindication = trim($_POST["contraindication"]);
	$active_substance = trim($_POST["active_substance"]);
	$side_effects = trim($_POST["side_effects"]);

	/* Switch off auto commit to allow transactions*/

	mysqli_autocommit($db, FALSE);
	$query_success = TRUE;
	
	if (mysqli_query($db, "SET foreign_key_checks = 0") === FALSE) {
		$query_success = FALSE;
	}

	$sql = "INSERT INTO MEDICAMENT( MEDICAMENT_TYPE_ID, `NAME`, CONTRAINDICATION, ACTIVE_SUBSTANCE, SIDE_EFFECTS ) VALUES 
			(?, ?, ?, ?, ?)";

	if(($stmt = mysqli_prepare($db, $sql))){
		// Bind variables to the prepared statement as parameters
		mysqli_stmt_bind_param($stmt, "sssss", $medicament_type_id, $name, $contraindication, $active_substance, $side_effects);
		
		// Attempt to execute the prepared statement
		if(mysqli_stmt_execute($stmt)){
			$medicament_id = mysqli_insert_id($db);
		}
		else {
			$err_string = "Sorry. Operation add medicament failed.";
			$query_success = FALSE;
		}
		mysqli_stmt_close($stmt);
	}
	if($query_success){

		$animal_species_id = $_POST["animal_species_id"];
		$dose = $_POST["dose"];
		$disease = $_POST["disease"];

		for($i = 0; $i < count($animal_species_id); $i++) {
			$sql = "INSERT INTO SUITABLE( MEDICAMENT_ID, ANIMAL_SPECIES_ID, DOSE, DISSEASE) 
					VALUES ( ?,?,?,? )";
			if(($stmt = mysqli_prepare($db, $sql))){
				// Bind variables to the prepared statement as parameters
				mysqli_stmt_bind_param($stmt, "ssss", $medicament_id, $animal_species_id[$i],$dose[$i],$disease[$i]);
				
				// Attempt to execute the prepared statement
				if(mysqli_stmt_execute($stmt)){}
				else {
					$err_string = "Sorry. Operation add suitable info to medicament failed.";
					$query_success = FALSE;
				}
				mysqli_stmt_close($stmt);
			}
		}
	}
	
	if (mysqli_query($db, "SET foreign_key_checks = 1") === FALSE) {
		$query_success = FALSE;
	}
	

	if ($query_success) {
		mysqli_commit($db);
		header("location: medicament_profile.php?medicament_id=$medicament_id");
	} 
	else {
		
		mysqli_rollback($db);
		header("location: error.php?text=$err_string");
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
			<li><a href="show_vets.php">Vets</a></li>
			<li><a href="show_nurses.php">Nurses</a></li>
			<li class="active" ><a href="show_medicaments.php">Medicaments</a></li>
		</ul>
		<ul class="nav navbar-nav navbar-right">
			<li><a href="#"><span class="glyphicon glyphicon-user"></span> <?php echo htmlspecialchars($_SESSION["username"]); ?></a></li>
			<li><a href="logout.php"><span class="glyphicon glyphicon-log-in"></span> Logout</a></li>
		</ul>
	</div>
</nav>

<div class="container-fluid text-center <?php echo ($query_success)?'':'hidden';?>">
	<p class= "lead">
		<span class="glyphicon glyphicon-ok-circle"></span> User was successfully added.
	</p>
</div>
<div class="container wrapper <?php echo (!$query_success)?'':'hidden';?> ">
	<form id="form_suit" onsubmit="return validateForm()" class="form-horizontal" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]); ?>" method="post">
		<div class="form-group ">
			<h2>Add medicament</h2>
		</div>

		<div class="form-group">
	      	<label for="sel1">Select medicament_type :</label>
	    	<select class="form-control" name="medicament_type_id">
			<?php
			foreach ($medicament_types as $value) {
				echo "<option value='$value[0]'>$value[1]</option>";
			}
			?>
	    	</select>
		</div>


		<div class="form-group ">
			<label>Name</label>
			<input type="text" name="name" class="form-control" maxlength="32">
		</div>

		<div class="form-group ">
			<label>Contraindication</label>
			<input type="text" name="contraindication" class="form-control" maxlength="50">
		</div>
		<div class="form-group ">
			<label>Active substance</label>
			<input type="text" name="active_substance" class="form-control" maxlength="50">
		</div>
		<div class="form-group ">
			<label>Side effects</label>
			<input type="text" name="side_effects" class="form-control" maxlength="50">
		</div>
		<div class="form-group">
			<div class=""> 
				<button type='button' class='btn btn-success btn-sm' data-toggle='modal' data-target='#Medicament'><span class='glyphicon glyphicon-plus'></span> Add suitable info</button>
			</div>
		</div>
		<hr/>
	<!--	
		<div class="form-group ">
			<label>Animal_specie</label>
			<input type="text" class="form-control" value="doga">
			<input type="hidden" name="animal_species_id[]" class="form-control" value="3">
		</div>
		
		<div class="form-group ">
			<label>Dose</label>
			<input type="text" name="dose[]" class="form-control" value="2-4mg/kg">
		</div>

		<div class="form-group ">
			<label>Disease</label>
			<input type="text" name="disease[]" class="form-control" value="Chorodba">
		</div>

-->
		<div class="form-group">
			<input type="submit" class="btn btn-primary" value="Submit">
			<input type="reset" class="btn btn-default" value="Reset">
		</div>
		<p><a href="show_medicaments.php">Back to medicaments.</a></p>
	</form>
</div>


<div class="container">
	<!-- Modal -->
	<div class="modal fade" id="Medicament" role="dialog">
		<div class="modal-dialog modal-lg">
			<!-- Modal content-->
			<div class="modal-content">
				<div class="modal-header">
					<button type="button" class="close" data-dismiss="modal">&times;</button>
					<h4 class="modal-title">Add suitable info about medicament</h4>
				</div>
				<div class="modal-body">
				
					<form action="#?>" method="get">
						<div class="form-group">
					      	<label for="sel1">Select medicament type :</label>
					    	<select class="form-control" name="animal_species_id" id="select">
							<?php
							foreach ($animal_species as $value) {
								echo "<option value='$value[0]'>$value[1]</option>";
							}
							?>
					    	</select>
						</div>

						
						<div class="form-group form-inline">
							<div> 
								<label class="control-label" for="dose">Dose:</label>
								<input type="text" name="dose" id="dose" class="form-control" value="2-4mg/kg" required maxlength="32">
							</div>	
						</div>
						<div class="form-group form-inline">
							<div> 
								<label class="control-label" for="disease">Disease:</label>
								<input type="text" name="disease" id="disease" class="form-control" maxlength="128" >
							</div>	
						</div>
					</form>
				</div>
				<div class="modal-footer">
					<button type="button" onclick="generateMed()" class="btn btn-default btn-sm" data-dismiss="modal">
         				<span class="glyphicon glyphicon-ok-sign"></span> Add info
        			</button>
				</div>	
			</div>
		</div>
	</div>
</div>	

<!--
<div class="form-group ">
	<label>Animal specie</label>
	<input type="text" class="form-control" value="doga">
	<input type="hidden" name="animal_species_id[]" class="form-control" value="3">
</div>

<div class="form-group ">
	<label>Dose</label>
	<input type="text" name="dose[]" class="form-control" value="2-4mg/kg">
</div>

<div class="form-group ">
	<label>Disease</label>
	<input type="text" name="disease[]" class="form-control" value="Chorodba">
</div>-->

<script>
	function validateForm(){
	var x = document.forms["form_suit"]["animal_species_id[]"];
	if(x == undefined){
		alert("Add at least one suitable info.");
		return false;
	}
	return true;

}
</script>
<script>
	function generateMed(){

		var i, specie, specie_name, medicament_name, medicament_id;
		var species = document.getElementById("select");
		for(i = 0 ; i < species.length; i++){
			if(species[i].selected){
				specie = species[i].value;
				specie_name = species[i].innerHTML;
				break;
			}
		}
		var dose = document.getElementById("dose").value;
		var disease = document.getElementById("disease").value;



		var div1 = document.createElement('div');
		div1.className = "form-group";
			var label1 = document.createElement('label');
			label1.innerHTML = "Animal specie";
				var new_input_specie = document.createElement('input');
				new_input_specie.type = "text";
				new_input_specie.className = "form-control";
				var new_input_specie_id = document.createElement('input');
				new_input_specie_id.type = "hidden";
				new_input_specie_id.name = "animal_species_id[]";
				new_input_specie_id.className = "form-control";				
		div1.appendChild(label1);
		div1.appendChild(new_input_specie_id);			
		div1.appendChild(new_input_specie);		
		

		var div2 = document.createElement('div');
		div2.className = "form-group";
			var label2 = document.createElement('label');
			label2.innerHTML = "Dose";
			var new_input_dose = document.createElement('input');
			new_input_dose.type = "text";
			new_input_dose.name = "dose[]";
			new_input_dose.className = "form-control";
		div2.appendChild(label2);
		div2.appendChild(new_input_dose);		
		

		var div3 = document.createElement('div');
		div3.className = "form-group";
			var label3 = document.createElement('label');
			label3.innerHTML = "Disease";
			var new_input_disease = document.createElement('input');
			new_input_disease.type = "text";
			new_input_disease.name = "disease[]";
			new_input_disease.className = "form-control";
		div3.appendChild(label3);
		div3.appendChild(new_input_disease);		
		
		var parent_div = document.getElementById("form_suit");
		
		/* set values */
		new_input_specie.value = specie_name;
		new_input_specie_id.value = specie;
		new_input_dose.value = dose;
		new_input_disease.value = disease;

		var link = parent_div.lastElementChild;
		parent_div.removeChild(link);

		var submit_btn = parent_div.lastElementChild;
	  	parent_div.removeChild(submit_btn);


		parent_div.appendChild(div1);
		parent_div.appendChild(div2);
		parent_div.appendChild(div3);
		
		parent_div.appendChild(document.createElement('br'));
		parent_div.appendChild(submit_btn);
		parent_div.appendChild(link);
		
			
	}
</script>

</body>
</html>