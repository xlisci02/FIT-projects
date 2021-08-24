<?php

require_once "config.php";

$sql = "SELECT PERSON_ID
		FROM USERS 
		NATURAL JOIN PERSON
		NATURAL JOIN VET
		WHERE USER_ID = ?";

if(($stmt = mysqli_prepare($db, $sql))){
			
	mysqli_stmt_bind_param($stmt, "s", $id);
	
	if(mysqli_stmt_execute($stmt)){
		// Store result
		mysqli_stmt_store_result($stmt);
		
		if(mysqli_stmt_num_rows($stmt) == 1){				   
			mysqli_stmt_bind_result($stmt, $person_id);
			if(mysqli_stmt_fetch($stmt)){
				$_SESSION["person_id"] = $person_id;
				$_SESSION["user_type"] = "vet";
			}
		}
		/* NOT VET */ 
		else{
			$sql = "SELECT  PERSON_ID
					FROM USERS 
					NATURAL JOIN PERSON
					NATURAL JOIN NURSE
					WHERE USER_ID = ?";
			if(($stmt = mysqli_prepare($db, $sql))){
				mysqli_stmt_bind_param($stmt, "s", $id);

				if(mysqli_stmt_execute($stmt)){
					// Store result
					mysqli_stmt_store_result($stmt);
					if(mysqli_stmt_num_rows($stmt) == 1){
						mysqli_stmt_bind_result($stmt, $person_id);
						if(mysqli_stmt_fetch($stmt)){
							$_SESSION["person_id"] = $person_id;
							$_SESSION["user_type"] = "nurse";
						}				   
						
	   				}
	   				/* neither VET nor  NURSE*/
	   				else $_SESSION["user_type"] = "admin";
	   			}
	   		}
		}
	}
}

?>