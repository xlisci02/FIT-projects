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

$treatment_prescrips = array();
$prescriptions_meds = array();
$treatment_info = array();
$err_string = "";



if($_SERVER["REQUEST_METHOD"] == "GET"){

	if(!isset($_GET["treatment_id"])){
		header("location: error.php?data_missing");
		exit;
	}

	/* TREATMENT INFO + PRESCRIPTIONS */
	$sql = "SELECT T.PERSON_ID, V.DEGREE, PE.`NAME` , T.ANIMAL_ID, A.ANIM_NAME, T.DIAGNOSIS, T.DATE_OF_START, T.PRICE, ADMINISTRATION_TIME,  `DATE` , PRESCRIPTION_ID
		FROM TREATMENT T
		NATURAL JOIN PRESCRIPTION P
		NATURAL JOIN PERSON PE
		INNER JOIN VET V ON T.PERSON_ID = V.PERSON_ID
		INNER JOIN ANIMAL A ON T.ANIMAL_ID = A.ANIMAL_ID
		WHERE TREATMENT_ID = ?
		ORDER BY PRESCRIPTION_ID ";

	if(($stmt = mysqli_prepare($db, $sql))){
		
		mysqli_stmt_bind_param($stmt, "s", $treatment_id);
		
		$treatment_id = (trim($_GET["treatment_id"]))? trim($_GET["treatment_id"]) : "";
		
		if(mysqli_stmt_execute($stmt)){
			// Store result
			mysqli_stmt_store_result($stmt);
			
			if(mysqli_stmt_num_rows($stmt) >= 1){
				// Bind result variables
				mysqli_stmt_bind_result($stmt, $person_id, $degree, $name, $animal_id, $anim_name, $diagnosis, $date_of_start, $price, $administration_time, $date, $prescription_id);
				while(mysqli_stmt_fetch($stmt)){
					array_push($treatment_info, array($person_id, $degree, $name, $animal_id, $anim_name, $diagnosis, $date_of_start, $price));
					array_push($treatment_prescrips, array($administration_time, $date, $prescription_id));
				}
			}
			else {
			/* NO PRESCRIPTIONS */
			$sql = "SELECT T.PERSON_ID, V.DEGREE, PE.`NAME` , T.ANIMAL_ID, A.ANIM_NAME, T.DIAGNOSIS, T.DATE_OF_START, T.PRICE
				FROM TREATMENT T
				NATURAL JOIN PERSON PE
				INNER JOIN VET V ON T.PERSON_ID = V.PERSON_ID
				INNER JOIN ANIMAL A ON T.ANIMAL_ID = A.ANIMAL_ID
				WHERE TREATMENT_ID = ?";
				if(($stmt = mysqli_prepare($db, $sql))){
		
					mysqli_stmt_bind_param($stmt, "s", $treatment_id);
					
					$treatment_id = (trim($_GET["treatment_id"]))? trim($_GET["treatment_id"]) : "";
					
					if(mysqli_stmt_execute($stmt)){
						// Store result
						mysqli_stmt_store_result($stmt);
						
						if(mysqli_stmt_num_rows($stmt) >= 1){
							// Bind result variables
							mysqli_stmt_bind_result($stmt, $person_id, $degree, $name, $animal_id, $anim_name, $diagnosis, $date_of_start, $price);
							while(mysqli_stmt_fetch($stmt)){
								array_push($treatment_info, array($person_id, $degree, $name, $animal_id, $anim_name, $diagnosis, $date_of_start, $price));
							}
						}
						else 
						/* NO TREATMENT */
						$err_string = "Treatment was not found!"; 				
					}
				}
			}
		}
		else $err_string = "Treatment was not found!"; 
	}
	else $err_string = "Treatment was not found!"; 


	if(!empty($treatment_prescrips)){
		foreach ($treatment_prescrips as $value) {
			$presc_id = end($value);
			/* PRESCRIPTION INFO */
			$sql = "SELECT ATC_CLASIFICATION, MODE_OF_ACTION,  `NAME` , SIDE_EFFECTS, CONTRAINDICATION, ACTIVE_SUBSTANCE, `DOSAGE`
					FROM INCLUDES I
					INNER JOIN MEDICAMENT M ON I.MEDICAMENT_ID = M.MEDICAMENT_ID
					NATURAL JOIN MEDICAMENT_TYPE
					WHERE PRESCRIPTION_ID = ?";

			if(($stmt = mysqli_prepare($db, $sql))){
				
				mysqli_stmt_bind_param($stmt, "s", $presc_id);
						
				if(mysqli_stmt_execute($stmt)){
					// Store result
					mysqli_stmt_store_result($stmt);
					
					if(mysqli_stmt_num_rows($stmt) >= 1){
						// Bind result variables
						mysqli_stmt_bind_result($stmt, $atc_clasification, $mode_of_action, $name, $side_effects, $contraindication, $active_substance, $dosage);
						$prescriptions_meds[$presc_id] = array();
						while(mysqli_stmt_fetch($stmt)){
							array_push($prescriptions_meds[$presc_id], array(
							$atc_clasification, $mode_of_action, $name, $side_effects, $contraindication, $active_substance, $dosage));
						}
					}
					else $err_string = "No pre was found!";
				}
				else $err_string = "No treatment was found!";
			}
			else $err_string = "No treatment was found!";
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

<div class="container-fluid <?php echo (empty($treatment_info))?'hidden':'' ?>" style="width: 70%">
	
	<div>
		<h3>Treatment detail</h3>
	</div>
	<ul class="list-group">
		<li class="list-group-item"><label>Determined by Vet: </label><a href="vet_profile.php?vet_id=<?php echo $treatment_info[0][0]?>" > <?php echo $treatment_info[0][1]." ".$treatment_info[0][2]?> </a></li>
		<li class="list-group-item"><label>Undegone by animal: </label><a href="animal_profile.php?animal_id=<?php echo $treatment_info[0][3]?>"> <?php echo $treatment_info[0][4]?> </a></li> 
		<li class="list-group-item"><label>Diagnosis: </label> <?php echo $treatment_info[0][5]?></li> 
		<li class="list-group-item"><label>Date of start: </label> <?php echo $treatment_info[0][6]?></li> 
		<li class="list-group-item"><label>Price (&euro;): </label> <?php echo $treatment_info[0][7]?></li> 
	</ul>
<div>
	<p>
		<a href='add_prescription.php?treatment_id=<?php echo $treatment_id ?>' class='btn btn-default btn-lg'>
		<span class='glyphicon glyphicon-plus'></span> Add new prescription</a>
	</p>
<div>
<div class="panel-group" >
	<?php
		$it = 0;
		foreach ($treatment_prescrips as $presc) {
		$presc_id = end($presc);
		$it ++;
		echo "	
		<div class='panel panel-info'>
			<div class='panel-heading'>
				<h4 class='panel-title'>
					<a data-toggle='collapse'  href='#collapse$it'>	#$it Prescription (Click to hide/show the list).</a>
				</h4>
			</div>
			<div id='collapse$it' class='panel-collapse collapse "; echo ($it==1)?"in'>":"'>"; 
				echo "
				<div class='panel-body'>
					<ul class='list-group'>
						<li class='list-group-item'><label>Administration time: $presc[0]</label></li>
						<li class='list-group-item'><label>Date: $presc[1]</label></li>";
						$med_i = 0;
						foreach ($prescriptions_meds[$presc_id] as $medicament) {
							$med_i ++;
							echo "<li class='list-group-item'><label>#${med_i} Medicament: $medicament[2] </label>";
							echo "<div>
								<button type='button' class='btn btn-xs' data-toggle='collapse' data-target='#collapse${it}med$med_i'> See more</button>
									<div id='collapse${it}med$med_i' class='collapse' style='margin-top:10px;'>
										<label>ATC Clasification: </label> $medicament[0] <br/>
										<label>Mode of action: </label> $medicament[1] <br/>
										<label>Side effects: </label> $medicament[3] <br/>
										<label>Contraindication: </label> $medicament[4] <br/>
										<label>Active substance: </label> $medicament[5] <br/>
										<label>Dosage: </label> $medicament[6] <br/>
									</div>
								</div</li>";
						}
						echo"						
					</ul>
				</div>
			</div>
		</div>";
		}
	?>

  </div>

</div>

</div>




</body>
</html>