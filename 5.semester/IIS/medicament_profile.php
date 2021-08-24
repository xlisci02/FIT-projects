<?php
// Initialize the session
session_start();
 
// Check if the user is logged in, if not then redirect him to login page
if(!isset($_SESSION["loggedin"]) || $_SESSION["loggedin"] !== true){
	header("location: index.php");
	exit;

}

else if($_SESSION["username"] != "admin"){
	header("location: error.php?permissions");
	exit;
}


require_once "config.php";

$medicament_info = array();
$medicament_suitable_info = array();

if($_SERVER["REQUEST_METHOD"] == "GET"){

	if(!isset($_GET["medicament_id"])){
		header("location: error.php?data_missing");
		exit;
	}

	/* medicament INFO + PRESCRIPTIONS */
	$sql = "SELECT `NAME` , ATC_CLASIFICATION, MODE_OF_ACTION, CONTRAINDICATION, ACTIVE_SUBSTANCE, SIDE_EFFECTS, SPECIE, DOSE, DISSEASE
			FROM MEDICAMENT_TYPE
			NATURAL JOIN MEDICAMENT
			NATURAL JOIN ANIMAL_SPECIES
			NATURAL JOIN SUITABLE
			WHERE MEDICAMENT_ID = ? ";

	if(($stmt = mysqli_prepare($db, $sql))){
		
		mysqli_stmt_bind_param($stmt, "s", $medicament_id);
		
	
		$medicament_id = $_GET["medicament_id"];
		if(mysqli_stmt_execute($stmt)){
			// Store result
			mysqli_stmt_store_result($stmt);
			
			if(mysqli_stmt_num_rows($stmt) >= 1){
				// Bind result variables
				mysqli_stmt_bind_result($stmt, $name, $atc_clasification, $mode_of_action, $contraindication, $active_substance, $side_effects, $specie, $dose, $dissease );
				while(mysqli_stmt_fetch($stmt)){
					array_push($medicament_info, array($name, $atc_clasification, $mode_of_action, $contraindication, $active_substance, $side_effects));
					array_push($medicament_suitable_info, array($specie, $dose, $dissease ));
				}
			}
		}
		mysqli_stmt_close($stmt);
	}
}


?>


	
<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<title>VETclinIS Nurses</title>
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


<div class="container-fluid <?php echo (empty($medicament_info))?'hidden':'' ?>" style="width: 70%">
	
	<div>
		<h3>Medicament detail</h3>
	</div>
	<ul class="list-group">
		<li class="list-group-item"><label>Medicament: </label> <?php echo $medicament_info[0][0]?> </a></li>
		<li class="list-group-item"><label>ATC classification: </label> <?php echo $medicament_info[0][1]?> </a></li> 
		<li class="list-group-item"><label>Mode of action: </label> <?php echo $medicament_info[0][2]?></li> 
		<li class="list-group-item"><label>Contraindication: </label> <?php echo $medicament_info[0][3]?></li> 
		<li class="list-group-item"><label>Active substance: </label> <?php echo $medicament_info[0][4]?></li> 
		<li class="list-group-item"><label>Side effects: </label> <?php echo $medicament_info[0][5]?></li> 

	</ul>
<div>

<div>
<div class="panel-group" >
	<?php
		$it = 0;
		foreach ($medicament_suitable_info as $info) {
		$it ++;
		echo "	
		<div class='panel panel-info'>
			<div class='panel-heading'>
				<h4 class='panel-title'>
					<a data-toggle='collapse'  href='#collapse$it'>	#$it Suitable info (Click to hide/show the list).</a>
				</h4>
			</div>
			<div id='collapse$it' class='panel-collapse collapse "; echo ($it==1)?"in'>":"'>"; 
				echo "
				<div class='panel-body'>
					<ul class='list-group'>
						<li class='list-group-item'><label>Suitable for specie: $info[0]</label></li>
						<li class='list-group-item'><label>Dose: $info[1]</label></li>
						<li class='list-group-item'><label>Diseases: $info[2]</label></li>
						";
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