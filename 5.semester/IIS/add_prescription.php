<?php
// Initialize the session
session_start();
 
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
$medicaments = array();
$err_string = "";

if($_SERVER["REQUEST_METHOD"] == "GET"){
	if(!isset($_GET["treatment_id"])){
		header("location: index.php");
		exit;		
	}
 	$treatment_id = trim($_GET["treatment_id"]);

 	$sql = "SELECT MEDICAMENT_ID, ATC_CLASIFICATION, MODE_OF_ACTION,  `NAME` , ACTIVE_SUBSTANCE
			FROM MEDICAMENT
			NATURAL JOIN MEDICAMENT_TYPE";
	if(($stmt = mysqli_prepare($db, $sql))){
								
		if(mysqli_stmt_execute($stmt)){
			// Store result
			mysqli_stmt_store_result($stmt);
			
			if(mysqli_stmt_num_rows($stmt) >= 1){
				// Bind result variables
				mysqli_stmt_bind_result($stmt, $medicament_id, $atc_clasification, $mode_of_action, $name, $active_substance);

				while(mysqli_stmt_fetch($stmt)){
					array_push($medicaments, array($medicament_id, $atc_clasification, $mode_of_action, $name, $active_substance));
				}
			}
			else $err_string = "No medicament was found!";
		}
		else $err_string = "No medicament was found!";
	}
	else $err_string = "No medicament was found!";
}

if(strlen($err_string)){
	header("location: error.php?text=$err_string");
	exit;
}

// Processing form data when form is submitted
if($_SERVER["REQUEST_METHOD"] == "POST"){

	if(	!isset($_POST["admin_time"]) ||
		!isset($_POST["date"]) ||
		!isset($_POST["treatment_id"]) ||
	    !(isset($_POST["medicament"])) ||
		!(isset($_POST["dosage"])) ) {

		header("location: error.php?data_missing");
		exit;
	}

	$admin_time = trim ($_POST["admin_time"]);
	$date = trim ($_POST["date"]);
	$treatment_id = trim ($_POST["treatment_id"]);
	
	/* Switch off auto commit to allow transactions*/

	mysqli_autocommit($db, FALSE);
	$query_success = TRUE;
	
	if (mysqli_query($db, "SET foreign_key_checks = 0") === FALSE) {
		$query_success = FALSE;
	}

	$sql = "INSERT INTO PRESCRIPTION( TREATMENT_ID, ADMINISTRATION_TIME, `DATE`) VALUES 
		(?,?, STR_TO_DATE(?, '%Y-%m-%d'))";

	if(($stmt = mysqli_prepare($db, $sql))){
		// Bind variables to the prepared statement as parameters
		mysqli_stmt_bind_param($stmt, "sss", $treatment_id, $admin_time, $date);
		
		// Attempt to execute the prepared statement
		if(mysqli_stmt_execute($stmt)){
			$prescription_id = mysqli_insert_id($db);
		}
		else {
			$err_string = "Sorry. Operation add prescription failed.";
			$query_success = FALSE;
		}
		mysqli_stmt_close($stmt);
	}

	if($query_success){

		$medicaments = $_POST["medicament"];
		$dosages = $_POST["dosage"];
		
		for($i = 0; $i < count($medicaments); $i++) {
			$sql = "INSERT INTO INCLUDES( MEDICAMENT_ID, PRESCRIPTION_ID, DOSAGE ) VALUES
			( ?,?,? )";
			if(($stmt = mysqli_prepare($db, $sql))){
				// Bind variables to the prepared statement as parameters
				mysqli_stmt_bind_param($stmt, "sss", $medicaments[$i], $prescription_id ,$dosages[$i]);
				
				// Attempt to execute the prepared statement
				if(mysqli_stmt_execute($stmt)){}
				else {
					$err_string = "Sorry. Operation add medicament to prescription failed.";
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
		header("location: treatment.php?treatment_id=$treatment_id");
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

<div class="container-fluid <?php echo (!$query_success)?'':'hidden';?>" style='width:80%'>
	<h2>Add prescription to treatment</h2>
	
	<form id="form_presc" name="form_presc" onsubmit="return validateForm()" class="form-horizontal col-xs-11" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]); ?>" method="post">
		<input class="hidden" name="treatment_id" value="<?php echo $treatment_id ?>">

		<div class="form-group">
			<label class="control-label col-xs-3" for="diagnosis">Administration time:</label>
			<div class="col-xs-9"> 
				<input type="text" name="admin_time" class="form-control" maxlength="32">
			</div>
		</div>
		<div class="form-group">
			<label class="control-label col-xs-3" for="diagnosis">Date:</label>
			<div class="col-xs-9"> 
				<input type="date" name="date" class="form-control" value="<?php echo date('Y-m-d'); ?>" max="<?php echo date('Y-m-d'); ?>"><br>
			</div>
		</div>
		<div class="form-group">
			<div class="col-xs-offset-3 col-xs-3"> 
				<button type='button' class='btn btn-primary btn-sm' data-toggle='modal' data-target='#Medicament'><span class='glyphicon glyphicon-plus'></span> Add medicament</button>
			</div>
		</div>
		<hr>
		<div class="form-group">
			<div class="col-xs-offset-3 col-xs-3"> 
				<button type='submit' class='btn btn-success btn-lg' ><span class='glyphicon glyphicon-ok-sign'></span> Submit</button>
			</div>
		</div>
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
					<h4 class="modal-title">Add medicament to prescription</h4>
				</div>
				<div class="modal-body">
				
					<form action="#?>" method="get">
						<div class="form-group form-inline">
							<input style="width:210px;" id='search_atc_name' onkeyup="searchMedicament()" type="text" class="form-control" placeholder="Search by ATC classification" autofocus>
							<input style="width:210px;" id='search_mode' onkeyup="searchMedicament()"type="text" class="form-control" placeholder="Search by mode of action">
							<input style="width:210px;" id='search_med_name' onkeyup="searchMedicament()"type="text" class="form-control" placeholder="Search by medicament name">
							<input style="width:210px;" id='search_act_subs' onkeyup="searchMedicament()"type="text" class="form-control" placeholder="Search by active substance">
						</div>

						<div class="panel-group form-group">
							<div class="panel panel-primary">
							<div class="panel-heading">
								<h4 class="panel-title">
								<a data-toggle="collapse" href="#collapse1">Choose medicament (Click to hide/show the list).</a>
								</h4>
							</div>
							<div id="collapse1" class="panel-collapse collapse in">
							<?php
								foreach ($medicaments as $value) {
									echo "<div class='panel-body radio'><label><input type='radio' name='medicament_id' checked value='$value[0]'>
									$value[1] / $value[2] / $value[3] / $value[4]<span class='hidden'>$value[1]/$value[2]/$value[3]/$value[4]</span></label></div>
									";
								}
							?>
							</div>
							</div>
						</div>
						<div class="form-group form-inline">
							<div> 
								<label class="control-label" for="dosage">Dosage:</label>
								<input type="text" name="dosage" id="dosage" class="form-control" value="1davka/8hod" maxlength="32">
							</div>	
						</div>
					</form>
				</div>
				<div class="modal-footer">
					<button type="button" onclick="generateMed()" class="btn btn-default btn-sm" data-dismiss="modal">
         				<span class="glyphicon glyphicon-ok-sign"></span> Add medicament
        			</button>
				</div>	
			</div>
		</div>
	</div>
</div>


<script>
	function validateForm(){
		var x = document.forms["form_presc"]["medicament[]"];
		if(x == undefined){
			alert("Prescription has to contain at least one medicament.\nPlease add medicament.");
			return false;
		}
		return true;

	}
</script>

<script>
	function generateMed(){
		var i, medicament_name, medicament_id;
		var medics = document.getElementsByClassName("radio");
		for(i = 0; i < medics.length; i++){
//			alert(medics[i].getElementsByTagName("input")[0].checked);
			if(medics[i].getElementsByTagName("input")[0].checked){
				medicament_name = medics[i].getElementsByTagName("span")[0].innerHTML.split("/")[2];
				medicament_id = medics[i].getElementsByTagName("input")[0].value;
				break;
			}
		}

		var div1 = document.createElement('div');
		div1.className = "form-group";
			var label1 = document.createElement('label');
			label1.className = "control-label col-xs-3";
			label1.innerHTML = "Medicament:";
				var div11 = document.createElement('div');
				div11.className = "col-xs-3";
					var new_input_med = document.createElement('input');
					new_input_med.type = "text";
					new_input_med.className = "form-control";
					var new_input_med_id = document.createElement('input');
					new_input_med_id.className = "hidden";
					new_input_med_id.name = "medicament[]";
				div11.appendChild(new_input_med_id);			
				div11.appendChild(new_input_med);		
		div1.appendChild(label1);
		div1.appendChild(div11);

		var div2 = document.createElement('div');
		div2.className = "form-group";
			var label2 = document.createElement('label');
			label2.className = "control-label col-xs-3";
			label2.innerHTML = "Dosage:";
			var div22 = document.createElement('div');
			div22.className = "col-xs-3";
					var new_input_dosage = document.createElement('input');
					new_input_dosage.type = "text";
					new_input_dosage.name = "dosage[]";
					new_input_dosage.className = "form-control";
				div22.appendChild(new_input_dosage);		
		div2.appendChild(label2);
		div2.appendChild(div22);

		var parent_div = document.getElementById("form_presc");
		var dosage = document.getElementById("dosage").value;
		
		/* set values */
		new_input_med.value = medicament_name;
		new_input_med_id.value = medicament_id;
		new_input_dosage.value = dosage;
		

		var submit_btn = parent_div.lastElementChild;
	  	parent_div.removeChild(submit_btn);

		parent_div.appendChild(div1);
		parent_div.appendChild(div2);
		parent_div.appendChild(document.createElement('br'));
		parent_div.appendChild(submit_btn);
			
	}

	function searchMedicament(){
		var  collapse, i, div, row, atc, mode;
		input_atc = document.getElementById("search_atc_name");
		input_mode = document.getElementById("search_mode");
		input_med_name = document.getElementById("search_med_name");
		input_act_subs = document.getElementById("search_act_subs");

		filter_atc = input_atc.value.toUpperCase();
		filter_mode = input_mode.value.toUpperCase();
		filter_med_name = input_med_name.value.toUpperCase();
		filter_act_subs = input_act_subs.value.toUpperCase();
		
		collapse = document.getElementById("collapse1");
		div = collapse1.getElementsByClassName("radio");

		for (i = 0; i < div.length; i++) {
			
			row = div[i].getElementsByTagName("span")[0];
			atc = (row.innerHTML.split("/")[0]);
			mode = (row.innerHTML.split("/")[1]);
			med_name = (row.innerHTML.split("/")[2]);
			act_subs = (row.innerHTML.split("/")[3]);
			if(atc && mode && med_name && act_subs){
				if((atc.toUpperCase().indexOf(filter_atc) > -1) &&
					(mode.toUpperCase().indexOf(filter_mode) > -1) &&
					(med_name.toUpperCase().indexOf(filter_med_name) > -1) &&
					(act_subs.toUpperCase().indexOf(filter_act_subs) > -1)){
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