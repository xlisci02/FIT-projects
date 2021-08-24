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

// Define variables and initialize with empty values
$name = $identification_number = $address = $birthdate = $sex = $username = $password = $confirm_password = $degree = $account_number = $hourly_wage = $mail = "";
$name_err = $identification_number_err = $birthdate_err = $address_err = $sex_err = $username_err = $password_err = $confirm_password_err = $degree_err = $account_number_err = $hourly_wage_err = $mail_err = "";

// Processing form data when form is submitted
if($_SERVER["REQUEST_METHOD"] == "POST"){
 

	if(	
		!isset($_POST["user"]) ||
		!isset($_POST["name"]) ||
		!isset($_POST["identification_number"]) ||
		!isset($_POST["address"]) ||
		!isset($_POST["birthdate"]) ||
		!isset($_POST["sex"]) ||
		!isset($_POST["username"]) ||
		!isset($_POST["password"]) ||
		!isset($_POST["confirm_password"]) ||
		!isset($_POST["degree"]) ||
		!isset($_POST["account_number"]) ||
		!isset($_POST["hourly_wage"]) ||
		!isset($_POST["mail"]) ){
			header("location: error.php?data_missing");
			exit;
	}

	mysqli_autocommit($db, FALSE);
	$query_success = TRUE;
	
	$user = $_POST["user"];
	$name = trim($_POST["name"]);
	$identification_number = trim($_POST["identification_number"]);
	$address = trim($_POST["address"]);
	$birthdate = $_POST["birthdate"];
	$sex = $_POST["sex"];
	$username = trim($_POST["username"]);
	$password = trim($_POST["password"]);
	$confirm_password = trim($_POST["confirm_password"]);
	$degree = trim($_POST["degree"]);
	$account_number = trim($_POST["account_number"]);
	$hourly_wage = trim($_POST["hourly_wage"]);
	$mail = trim($_POST["mail"]);

	// Prepare a select statement
	$sql = "SELECT USER_ID FROM USERS WHERE USERNAME = ?";
		
	if($stmt = mysqli_prepare($db, $sql)){
		// Bind variables to the prepared statement as parameters
		mysqli_stmt_bind_param($stmt, "s", $username);
		
		// Attempt to execute the prepared statement
		if(mysqli_stmt_execute($stmt)){
			/* store result */
			mysqli_stmt_store_result($stmt);
			
			if(mysqli_stmt_num_rows($stmt) == 1){
				$username_err = "This username is already taken.";
			}
		} 
		else{
			echo "Oops! Something went wrong. Please try again later.";
		}
		mysqli_stmt_close($stmt);
	}
		 
	
	
	// Validate confirm password
	if($password != $confirm_password){
		$confirm_password_err = "Password did not match.";
	}
	
	// Check input errors before inserting in database
	if(empty($username_err) && empty($confirm_password_err)){
		
		mysqli_autocommit($db, FALSE);
		$query_success = TRUE;

		if (mysqli_query($db, "SET foreign_key_checks = 0") === FALSE) 
			$query_success = FALSE; /* TODO pridat na koniec set na 1 */
		

		// Prepare an insert statement
		$sql = "INSERT INTO USERS (USERNAME, `PASSWORD`, MAIL) VALUES (?, ?, ?)";
		 
		if($stmt = mysqli_prepare($db, $sql)){
			// Bind variables to the prepared statement as parameters
			mysqli_stmt_bind_param($stmt, "sss", $username, $password, $mail);

			//$param_password = crypt($password, 'PASSWORD_DEFAULT'); // Creates a password hash
			// Attempt to execute the prepared statement
			if(mysqli_stmt_execute($stmt))
				$user_id = mysqli_insert_id($db);
			else 
				$query_success = FALSE;
			mysqli_stmt_close($stmt);
		}

		$sql = "INSERT INTO PERSON (`NAME`, IDENTIFICATION_NUMBER, BIRTHDATE, ADDRESS, SEX) VALUES " . 
				"(?, ?, STR_TO_DATE(?, '%Y-%m-%d'), ?, ?)";
		if($stmt = mysqli_prepare($db, $sql)){
			// Bind variables to the prepared statement as parameters
			mysqli_stmt_bind_param($stmt, "sssss", $name, $identification_number, $birthdate, $address, $sex);
			$name = trim($_POST["name"]); 
			$identification_number = trim($_POST["identification_number"]); 
			$birthdate = $_POST["birthdate"];
			$address = trim($_POST["address"]);
			$sex = $_POST["sex"];

			if(mysqli_stmt_execute($stmt)){
				$person_id = mysqli_insert_id($db);
			}
			else 
				$query_success = FALSE;
			mysqli_stmt_close($stmt);
		}

		if($user == "vet")
			$sql = "INSERT INTO VET( PERSON_ID, `DEGREE`, ACCOUNT_NUMBER, HOURLY_WAGE, USER_ID) VALUES " . 
				"(?, ?, ?, ?, ?)";
		else
			$sql = "INSERT INTO NURSE( PERSON_ID, `DEGREE`, ACCOUNT_NUMBER, HOURLY_WAGE, USER_ID) VALUES " . 
				"(?, ?, ?, ?, ?)";
		if($stmt = mysqli_prepare($db, $sql)){
			// Bind variables to the prepared statement as parameters
			mysqli_stmt_bind_param($stmt, "sssss", $person_id, $degree, $account_number, $hourly_wage, $user_id);
			$degree = trim($_POST["degree"]); 
			$account_number = trim($_POST["account_number"]); 
			$hourly_wage = trim($_POST["hourly_wage"]);

			if(mysqli_stmt_execute($stmt)){}
			else
				$query_success = FALSE;
			mysqli_stmt_close($stmt);
		}

		if (mysqli_query($db, "SET foreign_key_checks = 0") === FALSE) 
			$query_success = FALSE; 

		if ($query_success) {
			mysqli_commit($db);
		} 
		else {
			mysqli_rollback($db);
		}
  		
	}
// Close connection
mysqli_close($db);
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
		.wrapper{ width: 600px; padding: 20px; }
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
			<li><a href="show_medicaments.php">Medicaments</a></li>
		</ul>
		<ul class="nav navbar-nav navbar-right">
			<li><a href="#"><span class="glyphicon glyphicon-user"></span> <?php echo htmlspecialchars($_SESSION["username"]); ?></a></li>
			<li><a href="logout.php"><span class="glyphicon glyphicon-log-in"></span> Logout</a></li>
		</ul>
	</div>
</nav>

	<div class="container-fluid text-center <?php echo ($query_success && empty($username_err) && empty($confirm_password_err))?'':'hidden';?>">
		<p class= "lead">
			<span class="glyphicon glyphicon-ok-circle"></span> User was successfully added.
		</p>
	</div>
	<div class="container wrapper <?php echo (!($query_success && empty($username_err) && empty($confirm_password_err)))?'':'hidden';?> ">
		<form class="form-horizontal" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]); ?>" method="post">
			<div class="form-group">
				<h2>Add user</h2>
			</div>
			<div class="form-group <?php echo (!empty($sex_err)) ? 'has-error' : ''; ?>">

				<label class="radio-inline">
					<input type="radio" name="user" value="vet" checked>Vet
				</label>
				<label class="radio-inline">
					<input type="radio" name="user" value="nurse">Nurse
				</label>
				<!--<input type="text" name="sex" class="form-control" value="<?php echo $sex; ?>"> -->
				<span class="help-block"><?php echo $sex_err; ?></span>
			</div>
			
				<div class="form-group <?php echo (!empty($degree_err)) ? 'has-error' : ''; ?>">
				<label>Degree</label>
				<input type="text" name="degree" class="form-control" value="<?php echo $degree; ?>" maxlength="10" >
				<span class="help-block"></span>
			</div>

			<div class="form-group <?php echo (!empty($name_err)) ? 'has-error' : ''; ?>">
				<label>Name and Surname</label>
				<input type="text" name="name" class="form-control" maxlength="32" value="<?php echo $name; ?>">
				<span class="help-block"><?php echo $name_err; ?></span>
			</div>

			<div class="form-group <?php echo (!empty($identification_number_err)) ? 'has-error' : ''; ?>">
				<label>Identification number</label>
				<input type="text" name="identification_number" class="form-control" value="<?php echo $identification_number; ?>">
				<span class="help-block"><?php echo $identification_number_err; ?></span>
			</div>
			
			<div class="form-group <?php echo (!empty($birthdate_err)) ? 'has-error' : ''; ?>">
				<label class="show">Birthdate</label>
				<input type="date" name="birthdate" class="form-control"  value="<?php echo date('Y-m-d'); ?>" max="<?php echo date('Y-m-d'); ?>">
				<span class="help-block"><?php echo $birthdate_err; ?></span>
			</div>

			<div class="form-group <?php echo (!empty($mail_err)) ? 'has-error' : ''; ?>">
				<label class="show">Mail</label>
				<input type="email" name="mail" class="form-control" maxlength="32" >
				<span class="help-block"><?php echo $mail_err; ?></span>
			</div>

			<div class="form-group <?php echo (!empty($address_err)) ? 'has-error' : ''; ?>">
				<label>Address</label>
				<input type="text" name="address" maxlength="32" class="form-control" value="<?php echo $address; ?>">
				<span class="help-block"><?php echo $address_err; ?></span>
			</div>

			<div class="form-group <?php echo (!empty($sex_err)) ? 'has-error' : ''; ?>">
				<label class="show">Sex</label>

				<label class="radio-inline">
					<input type="radio" name="sex" value="M" checked>Male
				</label>
				<label class="radio-inline">
					<input type="radio" name="sex" value="F">Female
				</label>
				<!--<input type="text" name="sex" class="form-control" value="<?php echo $sex; ?>"> -->
				<span class="help-block"><?php echo $sex_err; ?></span>
			</div>
			<div class="form-group <?php echo (!empty($account_number_err)) ? 'has-error' : ''; ?>">
				<label>Account number</label>
				<input type="text" name="account_number" class="form-control" value="<?php echo $account_number; ?>" maxlength="24">
				<span class="help-block"><?php echo $account_number_err; ?></span>
			</div>
			<div class="form-group <?php echo (!empty($hourly_wage_err)) ? 'has-error' : ''; ?>">
				<label>Hourly wage</label>
				<input type="text" name="hourly_wage" class="form-control" value="<?php echo $hourly_wage; ?>">
				<span class="help-block"><?php echo $hourly_wage_err; ?></span>
			</div>
			
			<div class="form-group <?php echo (!empty($username_err)) ? 'has-error' : ''; ?>">
				<label>Username</label>
				<input type="text" maxlength="50" name="username" class="form-control" value="<?php echo $username; ?>">
				<span class="help-block"><?php echo $username_err; ?></span>
			</div>	
			<div class="form-group <?php echo (!empty($password_err)) ? 'has-error' : ''; ?>">
				<label>Password</label>
				<input type="password" maxlength="255" name="password" class="form-control" value="<?php echo $password; ?>">
				<span class="help-block"><?php echo $password_err; ?></span>
			</div>
			<div class="form-group <?php echo (!empty($confirm_password_err)) ? 'has-error' : ''; ?>">
				<label>Confirm Password</label>
				<input type="password" name="confirm_password" maxlength="255" class="form-control" value="<?php echo $confirm_password; ?>">
				<span class="help-block"><?php echo $confirm_password_err; ?></span>
			</div>
			<div class="form-group">
				<input type="submit" class="btn btn-primary" value="Submit">
				<input type="reset" class="btn btn-default" value="Reset">
			</div>
			<p><a href="welcome.php">Back to main page</a></p>
		</form>
	</div>	
</body>
</html>