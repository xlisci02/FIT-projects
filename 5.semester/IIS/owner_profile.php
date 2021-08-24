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

$owner_info = array();
$err_string = "";

if($_SERVER["REQUEST_METHOD"] == "GET"){

	if(!isset($_GET["owner_id"])){
		header("location: error.php?data_missing");
		exit;
	}
	/* OWNER INFO */
	$sql = "SELECT `NAME`, ADDRESS, MAIL, PHONE_NUMBER, IDENTIFICATION_NUMBER, BIRTHDATE, SEX, ANIM_NAME, SPECIE, ANIMAL_ID, PERSON_ID
			FROM PERSON
			NATURAL JOIN  `OWNER` 
			NATURAL JOIN  `ANIMAL` 
			NATURAL JOIN ANIMAL_SPECIES 
			WHERE PERSON_ID = ?";
  
	if(($stmt = mysqli_prepare($db, $sql))){
		
		mysqli_stmt_bind_param($stmt, "s", $owner_id);
		
		$owner_id = (trim($_GET["owner_id"]))? trim($_GET["owner_id"]) : "";
		
		if(mysqli_stmt_execute($stmt)){
			// Store result
			mysqli_stmt_store_result($stmt);
			
			if(mysqli_stmt_num_rows($stmt) >= 1){				   
				// Bind result variables
				mysqli_stmt_bind_result($stmt, $name, $address, $mail, $phone_number, $identification_number, $birthdate, $sex, $anim_name, $specie, $animal_id, $person_id );
				while(mysqli_stmt_fetch($stmt)){
					array_push($owner_info, array($name, $address, $mail, $phone_number, $identification_number, $birthdate, $sex, $anim_name, $specie, $animal_id, $person_id));
				}
			}
			else $err_string = "Owner was not found!";
		}
		else $err_string = "Owner was not found!";
	}
	else $err_string = "Owner was not found!";

	if(strlen($err_string)){
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
			<li class="<?php echo ($_SESSION["user_type"] == "nurse")?'hidden':''; ?>"> <a href="add_treatment.php">Add new treatment</a></li>
			<li><a href="add_intervention.php">Add new intervention</a></li>
		</ul>
		<ul class="nav navbar-nav navbar-right">
			<li><a href="#"><span class="glyphicon glyphicon-user"></span> <?php echo htmlspecialchars($_SESSION["username"]); ?></a></li>
			<li><a href="logout.php"><span class="glyphicon glyphicon-log-in"></span> Logout</a></li>
		</ul>
	</div>
</nav>

<div class="container-fluid <?php echo (empty($owner_info))?'hidden':'' ?>" style="width: 70%">
	<div>
	  	<h3>Owner: <?php echo $owner_info[0][0]?></h3>
	</div>
			<ul class="nav nav-tabs">
				<li class="active"><a data-toggle="tab" href="#home">Home</a></li>
				<li><a data-toggle="tab" href="#animals">Animals</a></li>
			  </ul>

			  
		  <div class="tab-content">
			<div class="tab-pane fade in active" id="home">
				<hr>
				  <form id="form" class="form" action="edit_owner.php" method="post" >
				  	  <input type="hidden" name="person_id" value="<?php echo $owner_info[0][10]?>"> 
				  	  <input type="hidden" name="name" value="<?php echo $owner_info[0][0]?>">
				  	  
					  <div class="form-group">
						  
						  <div class="col-xs-6">
							  <label><h4>Address</h4></label>
							  <input readonly type="text" class="form-control" name="address" value="<?php echo $owner_info[0][1]?>" maxlength="32">
						  </div>
					  </div>
					  <div class="form-group">
						  
						  <div class="col-xs-6">
							<label><h4>Mail</h4></label>
							  <input readonly type="email" class="form-control" name="mail" value="<?php echo $owner_info[0][2]?>" maxlength="32" >
						  </div>
					  </div>
		  
					  <div class="form-group">
						  
						  <div class="col-xs-6">
							  <label><h4>Phone number</h4></label>
							  <input readonly type="text" class="form-control" name="phone" value="<?php echo $owner_info[0][3]?>" maxlength="13">
						  </div>
					  </div>
		  
					  <div class="form-group">
						  <div class="col-xs-6">
							 <label><h4>Identification number</h4></label>
							  <input readonly type="text" class="form-control" name="id_num" value="<?php echo $owner_info[0][4]?>" >
						  </div>
					  </div>
					  <div class="form-group">
						  
						  <div class="col-xs-6">
							  <label><h4>Birthdate</h4></label>
							  <input readonly type="date" class="form-control" name="birthdate" value="<?php echo $owner_info[0][5]?>" max="<?php echo date('Y-m-d'); ?>">
						  </div>
					  </div>
					  <div class="form-group">
						  
						  <div class="col-xs-6">
							  <label><h4>Sex</h4></label>
							  <input readonly  pattern="M|F" type="text" class="form-control" name="sex" value="<?php echo $owner_info[0][6]?>" pattern="M|F">
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
			 <div class="tab-pane fade" id="animals">

			   <hr>
				  
			  	<div class="table-responsive">
						  <table class="table table-hover">
							<thead>
						<tr>
							<th>Animal's name</th>
							<th>Specie</th>
						</tr>
						</thead>
						<tbody>
  						<?php 
  						if(!empty($owner_info))
  							foreach ($owner_info as $row) {
  							echo "<tr>";	
  							echo "<td><a href='animal_profile.php?animal_id=$row[9]'>$row[7]</a></td>";
  							echo "<td>$row[8]</td>";
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


<script>
	function makeEditable(){
		var form = document.getElementById('form');
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