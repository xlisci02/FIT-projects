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

<div class="table-responsive">
	<table class="table table-hover">
		<thead>
			<tr>
				<th>Medicament</th>
				<th>ATC Clasification</th>
				<th>Mode of action</th>
				<th>Contraindication</th>
				<th>Active substance</th>
				<th>Side effects</th>
				<th>View detail</th>
			</tr>
		</thead>
		<tbody>
<?php

require_once "config.php";
$medicaments = array();

$sql = "SELECT MEDICAMENT_ID,  `NAME` , ATC_CLASIFICATION, MODE_OF_ACTION, CONTRAINDICATION, ACTIVE_SUBSTANCE, SIDE_EFFECTS
		FROM MEDICAMENT_TYPE
		NATURAL JOIN MEDICAMENT";
if(($stmt = mysqli_prepare($db, $sql))){
	if(mysqli_stmt_execute($stmt)){
		mysqli_stmt_store_result($stmt);
		if(mysqli_stmt_num_rows($stmt) >= 1){				   
			mysqli_stmt_bind_result($stmt, $medicament_id, $name, $atc_clasification, $mode_of_action, $contraindication, $active_substance, $side_effects);
			while(mysqli_stmt_fetch($stmt)) {
					array_push($medicaments, array($medicament_id, $name, $atc_clasification, $mode_of_action, $contraindication, $active_substance, $side_effects) );

				}
			}
		}
		mysqli_stmt_close($stmt);
	}


foreach ($medicaments as $medicament) {
	echo '<tr class="text-left">';
	echo "<td>".$medicament[1]."</td>";
	echo "<td>".$medicament[2]."</td>";
	echo "<td>".$medicament[3]."</td>";
	echo "<td>".$medicament[4]."</td>";
	echo "<td>".$medicament[5]."</td>";
	echo "<td>".$medicament[6]."</td>";
	echo "<td>"."<a  href='medicament_profile.php?medicament_id=$medicament[0]' type='button' class='btn btn-primary btn-xs'><span class='glyphicon glyphicon-new-window'></span></a></td>";
	echo "</tr>"; 
	}
// Close connection
mysqli_close($db);
?>	  

		</tbody>
	</table>
</div>	


	<div class="container-fluid">
		<a href="add_medicament.php" class="btn btn-success btn-lg"><span class="glyphicon glyphicon-plus"> </span> Add medicament</a>
		
	</div>

</body>
</html>