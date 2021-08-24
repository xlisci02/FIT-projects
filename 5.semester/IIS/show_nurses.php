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
			<li class="active" ><a href="show_nurses.php">Nurses</a></li>
			<li><a href="show_medicaments.php">Medicaments</a></li>
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
				<th>Degree</th>
				<th>Name</th>
				<th>Username</th>
				<th>Mail</th>
				<th>Identification number</th>
				<th>Birthdate</th>
				<th>Address</th>
				<th>Sex</th>
				<th>Account number</th>
				<th>Hourly wage</th>
				<th>Edit</th>
				<th>Delete</th>
			</tr>
		</thead>
		<tbody>
<?php

require_once "config.php";
$users = array();

$sql = "SELECT USER_ID, USERNAME, MAIL
		FROM USERS";

if(($stmt = mysqli_prepare($db, $sql))){
	if(mysqli_stmt_execute($stmt)){
		mysqli_stmt_store_result($stmt);
		if(mysqli_stmt_num_rows($stmt) >= 1){				   
			mysqli_stmt_bind_result($stmt, $user_id, $username, $mail);
			while(mysqli_stmt_fetch($stmt)) {
				array_push($users, array($user_id, $username, $mail));
			}
		}
	}
	mysqli_stmt_close($stmt);
}

$sql = "SELECT DEGREE, `NAME`, IDENTIFICATION_NUMBER, BIRTHDATE, ADDRESS, SEX, ACCOUNT_NUMBER, HOURLY_WAGE, PERSON_ID, USER_ID
		FROM NURSE
		NATURAL JOIN PERSON";

if(($stmt = mysqli_prepare($db, $sql))){
	if(mysqli_stmt_execute($stmt)){
		// Store result
		mysqli_stmt_store_result($stmt);
		if(mysqli_stmt_num_rows($stmt) >= 1){				   

			mysqli_stmt_bind_result($stmt, $degree,  $name, $identification_number, $birthdate, $address, $sex, $account_number, $hourly_wage, $person_id, $user_id_2);
			while(mysqli_stmt_fetch($stmt)) {
				$contains = 0;
				foreach ($users as $user_info) {
					if($user_info[0] == $user_id_2){
						$contains = 1;
						$username1 = $user_info[1];
						$mail1 = $user_info[2];
						break;
					}
				}
				if($contains){
					echo '<tr class="text-left">';
					echo "<td>".$degree."</td>";
					echo "<td>".$name."</td>";
					echo "<td>".$username1."</td>";
					echo "<td>".$mail1."</td>";
					echo "<td>".$identification_number."</td>";
					echo "<td>".$birthdate."</td>";
					echo "<td>".$address."</td>";
					echo "<td>".$sex."</td>";
					echo "<td>".$account_number."</td>";
					echo "<td>".$hourly_wage."</td>";
					echo "<td>"."<button type='button' onclick='storeRow(\"$degree\", \"$name\", \"$username\", \"$mail\", \"$identification_number\", \"$birthdate\", \"$address\", \"$sex\", \"$account_number\", \"$hourly_wage\", \"$person_id\", \"$user_id\")' class='btn btn-info btn-xs' data-toggle='modal' data-target='#Edit' ><span class='glyphicon glyphicon-edit'  ></span></button></a></td>"."</td>"; 
					echo "<td>"."<button type='button' onclick='storeIDs(\"$person_id\", \"$user_id\")' class='btn btn-danger btn-xs' data-toggle='modal' data-target='#Delete'><span class='glyphicon glyphicon-trash'></span></button></td>"."</td>"; 
					echo "</tr>"; 
				}
				else{

				echo '<tr class="text-left">';
				echo "<td><del>".$degree."</del></td>";
				echo "<td><del>".$name."</del></td>";
				echo "<td><del>"."-"."</del></td>";
				echo "<td><del>"."-"."</del></td>";
				echo "<td><del>".$identification_number."</del></td>";
				echo "<td><del>".$birthdate."</del></td>";
				echo "<td><del>".$address."</del></td>";
				echo "<td><del>".$sex."</del></td>";
				echo "<td><del>".$account_number."</del></td>";
				echo "<td><del>".$hourly_wage."</del></td>";
				echo "</tr>"; 
				}
			}
		}
	} 
	else{
		echo "Oops! Something went wrong. Please try again later.";
	}
	// Close statement
	mysqli_stmt_close($stmt);
}
	// Close connection
	mysqli_close($db);
?>	  

		</tbody>
	</table>
</div>	
<div class="container">
  <!-- Modal -->
  <div class="modal fade" id="Edit" role="dialog">
	<div class="modal-dialog">
	
	  <!-- Modal content-->
	  <div class="modal-content">
		<div class="modal-header">
		  <button type="button" class="close" data-dismiss="modal">&times;</button>
		  <h4 class="modal-title">Edit nurse</h4>
		</div>
		<div class="modal-body wrapper ">
			<form action="edit_user.php" method="post">
				<div class="form-group">
					<label>Degree</label>
					<input type="text" name="degree" class="form-control" autofocus maxlength="10">
				</div>

				<div class="form-group">
					<label>Name and Surname</label>
					<input type="text" name="name" class="form-control" required maxlength="32">
					<span class="help-block"></span>
				</div>

				 <div class="form-group">
					<label>Mail</label>
					<input type="email" name="mail" class="form-control " required maxlength="32"">
					<span class="help-block"></span>
				</div>

				<div class="form-group">
					<label>Identification number</label>
					<input type="text" name="identification_number" class="form-control" >
					<span class="help-block"></span>
				</div>
				<div class="form-group">
					<label class="show">Birthdate</label>
					<div class="row">
						<div class="col-md-4">
						<label class="small">Day</label>
						<input type="text" name="birthdate_day" class="form-control" >
						</div>
						<div class="col-md-4">
						<label class="small">Month</label>
						<input type="text" name="birthdate_month" class="form-control" >
						</div>
						<div class="col-md-4">
						<label class="small">Year</label>
						<input type="text" name="birthdate_year" class="form-control" >
						</div>

					</div>
					<span class="help-block"></span>
				</div>
				<div class="form-group">
					<label>Address</label>
					<input type="text" name="address" class="form-control" maxlength="32">
					<span class="help-block"></span>
				</div>

				<div class="form-group">
					<label class="show">Sex</label>

					<label class="radio-inline">
						<input type="radio" name="sex" value="M" checked>Male
					</label>
					<label class="radio-inline">
						<input type="radio" name="sex" value="F">Female
					</label>
					<span class="help-block"></span>
				</div>
				<div class="form-group">
					<label>Account number</label>
					<input type="text" name="account_number" class="form-control" maxlength="24">
					<span class="help-block"></span>
				</div>
				<div class="form-group">
					<label>Hourly wage</label>
					<input type="text" name="hourly_wage" class="form-control" >
					<span class="help-block"></span>
				</div>
				
				<div class="form-group">
					<label>Username</label>
					<input type="text" name="username" class="form-control" disabled>
					<span class="help-block"></span>
				</div>	
				
				<div class="form-group">
					<input type="hidden" name="user" value="nurse">						
					<input type="hidden" name="person_id">
					<input type="hidden" name="user_id">
					<input type="submit" class="btn btn-primary" value="Submit">
					<input type="reset" class="btn btn-default" value="Reset">
				</div>
			</form>
		</div>
		<div class="modal-footer">
		  <button type="button" class="btn btn-default" data-dismiss="modal">Close</button>
		</div>	
		</div>
	  </div>
	</div>
  </div>


<div class="container">
	<!-- Modal -->
	<div class="modal fade" id="Delete" role="dialog">
		<div class="modal-dialog">
			

			<!-- Modal content-->
			<div class="modal-content">
				<div class="modal-header">
					<button type="button" class="close" data-dismiss="modal">&times;</button>
					<h4 class="modal-title">Delete nurse</h4>
				</div>
				<div class="modal-body">
					<div class="alert alert-danger">
						<p><span class="glyphicon glyphicon-exclamation-sign"></span> Are you sure you want to delete this record ?</p>
					</div>
				</div>
				<div class="modal-footer">
					<form action="remove_user.php" method="post">
					<input type="hidden" name="user" value="nurse">						
					<input type="hidden" name="person_id">
					<input type="hidden" name="user_id">
					<button type="submit" class="btn btn-default btn-success"><span class="glyphicon glyphicon glyphicon-ok-sign"> </span> Yes</button>
					<button type="button" class="btn btn-default btn-danger" data-dismiss="modal"><span class="glyphicon glyphicon glyphicon-remove-circle"> </span> No</button>
				</div>	
			</div>
		

		</div>
	</div>
</div>

	<div class="container-fluid">
		<a href="add_user.php" class="btn btn-success btn-lg"><span class="glyphicon glyphicon-plus"> </span> Add nurse</a>
		
	</div>
<!-- scripts -->

<script type="text/javascript">
  function storeRow(degree, name, username, mail, identification_number, birthdate, address, sex, account_number, hourly_wage, person_id, user_id){
	  document.getElementsByName("degree")[0].value = degree;
	  document.getElementsByName("name")[0].value = name;
	  document.getElementsByName("username")[0].value = username;
	  document.getElementsByName("mail")[0].value = mail;
	  document.getElementsByName("identification_number")[0].value = identification_number;
	  var year_month_day = birthdate.split("-");
	  document.getElementsByName("birthdate_year")[0].value = year_month_day[0];
	  document.getElementsByName("birthdate_month")[0].value = year_month_day[1];
	  document.getElementsByName("birthdate_day")[0].value = year_month_day[2];
	  document.getElementsByName("address")[0].value = address;
	  if(sex == "M"){
		document.getElementsByName("sex")[0].checked = true;
		document.getElementsByName("sex")[1].checked = false;
	  }
	  else{
	  document.getElementsByName("sex")[0].checked = false;
	  document.getElementsByName("sex")[1].checked = true;
	  }
	document.getElementsByName("account_number")[0].value = account_number;
	document.getElementsByName("hourly_wage")[0].value = hourly_wage;

	document.getElementsByName("person_id")[0].value = person_id;
	document.getElementsByName("user_id")[0].value = user_id;

  }
  function storeIDs(person_id, user_id){
	document.getElementsByName("person_id")[1].value = person_id;
	document.getElementsByName("user_id")[1].value = user_id;
  }
</script>

</body>
</html>