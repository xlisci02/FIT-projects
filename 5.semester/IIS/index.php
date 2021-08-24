<?php
// Initialize the session
session_start();
 
// Check if the user is already logged in, if yes then redirect him to welcome page
if(isset($_SESSION["loggedin"]) && $_SESSION["loggedin"] === true){
	header("location: welcome.php");
	exit;
}

// Include config file
require_once "config.php";
 
// Define variables and initialize with empty values
$username = $password = "";
$username_err = $password_err = "";
 
// Processing form data when form is submitted
if($_SERVER["REQUEST_METHOD"] == "POST"){
 
	// Check if username is empty
	
	if(trim($_POST["username"]) == false){
		$username_err = "Please enter username.";
	} else{
		$username = trim($_POST["username"]);
	}
	
	// Check if password is empty
	if(trim($_POST["password"]) ==  false){
		$password_err = "Please enter your password.";
	} else{
		$password = trim($_POST["password"]);
	}
	
	// Validate credentials
	if(empty($username_err) && empty($password_err)){
		// Prepare a select statement
		$sql = "SELECT USER_ID, USERNAME, `PASSWORD` FROM USERS WHERE USERNAME = ?";

		if(($stmt = mysqli_prepare($db, $sql))){
			// Bind variables to the prepared statement as parameters
			mysqli_stmt_bind_param($stmt, "s", $param_username);
			// Set parameters
			$param_username = $username;
			
			// Attempt to execute the prepared statement
			if(mysqli_stmt_execute($stmt)){
				// Store result
				mysqli_stmt_store_result($stmt);
				
				// Check if username exists, if yes then verify password
				if(mysqli_stmt_num_rows($stmt) == 1){					
					// Bind result variables
					mysqli_stmt_bind_result($stmt, $id, $username, $hashed_password);
					if(mysqli_stmt_fetch($stmt)){
						if($password == $hashed_password){
							// Password is correct, so start a new session
							session_start();
							
							// Store data in session variables
							$_SESSION["loggedin"] = true;
							$_SESSION["id"] = $id;
							$_SESSION["username"] = $username;							
							require_once "set_user_type.php";
							// Redirect user to welcome page
							header("location: welcome.php");
						} else{
							// Display an error message if password is not valid
							$password_err = "Invalid username or password. Try again.";
						}
					}
				} else{
					// Display an error message if username doesn't exist
					$username_err = "Invalid username or password. Try again.";
				}
			} else{
				echo "Oops! Something went wrong. Please try again later.";
			}
			// Close statement
			mysqli_stmt_close($stmt);
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
	<meta charset="UTF-8">
	<title>VETclinIS</title>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css">
	<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js"></script>
	<script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js"></script>
	<style type="text/css">
		body{ font: 14px sans-serif;     background-image: url("kids-animal-pattern-1.jpg")}
		.wrapper{ width: 550px; padding: 20px;  box-shadow: 0 0 50px #505050; margin-top: 200px;
}
	</style>
</head>
<body>
	<div class="wrapper container" style="background-color: white;">
		<h2 class="text-center"><span class="glyphicon glyphicon-log-in"> </span> Login</h2>
		<form action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]); ?>" method="post">
			<label>Username</label>
			<div class="form-group input-group">
				<span class="input-group-addon"><i class="glyphicon glyphicon-user"></i></span>
    			<input type="text" class="form-control" name="username" placeholder="Username" required autofocus>
			</div>	
			<label>Password</label>
			<div class="form-group input-group">
				<span class="input-group-addon"><i class="glyphicon glyphicon-lock"></i></span>
				<input type="password" class="form-control" name="password" placeholder="Password" required>
			</div>
			<div class="alert alert-danger <?php echo (!empty($username_err) || !empty($username_err)) ? '' : 'hidden'; ?>">
  				<strong><?php echo $password_err; echo $username_err;?></strong> 
			</div>
			<div class="form-group ">
				<input type="submit" class="btn btn-primary" value="Login">
			</div>
		</form>
	</div>	
</body>
</html>