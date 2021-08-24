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
$results = array();
$err_string = "";

$sql = "SELECT	PERSON_ID, `NAME`, ADDRESS
		FROM PERSON
		NATURAL JOIN `OWNER`
		NATURAL JOIN ANIMAL";
if(($stmt = mysqli_prepare($db, $sql))){
			
	if(mysqli_stmt_execute($stmt)){
		// Store result
		mysqli_stmt_store_result($stmt);
		
		if(mysqli_stmt_num_rows($stmt) >= 1){					 
			// Bind result variables
			mysqli_stmt_bind_result($stmt, $person_id, $name, $address);
			while(mysqli_stmt_fetch($stmt)){
				array_push($results, array($person_id, $name, $address));
			}
		}
		else $err_string = "No owner was found!";
	}
	else $err_string = "No owner was found!";
}
else $err_string = "No owner was found!";	

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
			<li class="active"><a href="search_owner.php">Search for owner</a></li>
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
   <form class="navbar-form navbar-left" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]); ?>" method="get">
	  <div class="form-group">
		<input style="width:200px;" id='search_name' onkeyup="searchOwner()" name="owner_name" type="text" class="form-control" placeholder="Search by owner's name" autofocus>
	  </div>
	  <div class="form-group">
		<input style="width:200px;" id='search_address' onkeyup="searchOwner()"name="animal_name" type="text" class="form-control" placeholder="Search by owner's address">
	  </div>
	  <div class="form-group">
        <a href="add_owner.php" class="btn btn-primary btn-sm">
          <span class="glyphicon glyphicon-plus-sign"></span> Add owner
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
							<th>Owner's name</th>
							<th>Address</th>
						</tr>
					</thead>
					<tbody class='table-hover'>";
					foreach ($results as $row) {
						echo "<tr>";
						echo "<td><a href='owner_profile.php?owner_id=$row[0]'>$row[1]</a></td>";
						echo "<td class='hidden'>$row[1]</td>";
						echo "<td>$row[2]</td>";
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
		input = document.getElementById("search_name");
		input2 = document.getElementById("search_address");

		filter = input.value.toUpperCase();
		filter2 = input2.value.toUpperCase();
		table = document.getElementById("owners");
		tr = table.getElementsByTagName("tr");
		for (i = 0; i < tr.length; i++) {

			td = tr[i].getElementsByTagName("td")[1];
			td2 = tr[i].getElementsByTagName("td")[2];
			
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