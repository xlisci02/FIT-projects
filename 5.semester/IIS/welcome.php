<?php
// Initialize the session
session_start();
 
// Check if the user is logged in, if not then redirect him to login page
if(!isset($_SESSION["loggedin"]) || $_SESSION["loggedin"] !== true){
	header("location: index.php");
	exit;
}

if($_SESSION["username"] === "admin"){
	header("location: show_vets.php");
	exit;
}

// Include config file
require_once "config.php";
$results = array();
$err_string = "";
$sql = "SELECT  ANIMAL_ID, ANIM_NAME, SPECIE, PERSON_ID, `NAME`
		FROM PERSON
		NATURAL JOIN  `OWNER` 
		NATURAL JOIN  `ANIMAL` 
		NATURAL JOIN ANIMAL_SPECIES ";
if(($stmt = mysqli_prepare($db, $sql))){
	if(mysqli_stmt_execute($stmt)){
		// Store result
		mysqli_stmt_store_result($stmt);
		
		if(mysqli_stmt_num_rows($stmt) >= 1){				   
			// Bind result variables
			mysqli_stmt_bind_result($stmt, $animal_id, $animal_name, $specie, $person_id, $name);
			while(mysqli_stmt_fetch($stmt)){
				array_push($results, array($animal_id, $animal_name, $specie, $person_id, $name));
			}
		}
		else $err_string = "No animal was found!";
	}
	else $err_string = "No animal was found!";
}
else $err_string = "No animal was found!";

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
			<li class="active"><a href="welcome.php">Search for animal</a></li>
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


<div class="container-fluid" style='width:80%'>
   <form class="navbar-form navbar-left" action="#">
	  <div class="form-group">
		<input style="width:200px;" id='search_anim_name' onkeyup="searchOwner()" type="text" class="form-control" placeholder="Search by animal's name" autofocus>
	  </div>
	  <div class="form-group">
		<input style="width:200px;" id='search_owner_name' onkeyup="searchOwner()"type="text" class="form-control" placeholder="Search by owner's name">
	  </div>
	  <div class="form-group">
        <a href="add_animal.php" class="btn btn-primary btn-sm">
          <span class="glyphicon glyphicon-plus-sign"></span> Add animal
        </a>
	  </div>
	</form>
</div>
	<hr/>
	<?php  
	if(empty($err_string)){
		echo "<div class='table-responsive container-fluid ' style='width:80%'	>
				<table class='table table-hover' id='owners'>
					<thead>
						<tr>
							<th>Animal's name</th>
							<th>Specie</th>
							<th>Owner</th>
						</tr>
					</thead>
					<tbody class='table-hover'>";
					foreach ($results as $row) {
						echo "<tr>";
						echo "<td><a href='animal_profile.php?animal_id=$row[0]'>$row[1]</a></td>";
						echo "<td class='hidden'>$row[1]</td>";
						echo "<td>$row[2]</td>";
						echo "<td><a href='owner_profile.php?owner_id=$row[3]'>$row[4]</a></td>";
						echo "<td class='hidden'>$row[4]</td>";
						echo "</tr>";
					}
		echo "</tbody>
			</table>
			</div>";	
	}
	else if($err_string) {
		echo "<div class='container-fluid'><h4 class='text-danger'><span class='glyphicon glyphicon-remove'> </span> $err_string</h4></div>";
	}
	?>

	<script>
	function searchOwner() {
		var input, filter, table, tr, td, i, input2, filter2, td2;
		input = document.getElementById("search_anim_name");
		input2 = document.getElementById("search_owner_name");

		filter = input.value.toUpperCase();
		filter2 = input2.value.toUpperCase();
		table = document.getElementById("owners");
		tr = table.getElementsByTagName("tr");
		for (i = 0; i < tr.length; i++) {

			td = tr[i].getElementsByTagName("td")[1];
			td2 = tr[i].getElementsByTagName("td")[4];
			
			if (td && td2) {
				if ((td.innerHTML.toUpperCase().indexOf(filter) > -1) && 
					(td2.innerHTML.toUpperCase().indexOf(filter2) > -1)) {
					tr[i].style.display = "";

				} else {
					tr[i].style.display = "none";
				}
			}		 
		}
	}
	</script>

	
</body>
</html>