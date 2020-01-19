<?php
$server = "localhost";   //ip cim
$username = "root";          //felhasznalo
$password = "";			 //jelszo
$dbname = "esp_adat";			 //adatbazis neve, amibe irunk
	
$temperature="0";
$humidity="0";

$foabiztonsag="";
$foabiztonsag_v="3r05v3d3l3m";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $foabiztonsag = input($_POST["foabiztonsag"]);
    if($foabiztonsag == $foabiztonsag_v) {
        $temperature = input($_POST["temperature"]);
        $humidity = input($_POST["humidity"]);
	
		$conn = new mysqli($server, $username, $password, $dbname);			//csatlakozas a szerverhez
        // csatlakozas ellenorzese
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        } 
		
		
		$sql="INSERT INTO adatok (Hőmérséklet, Páratartalom)
		VALUES ('" .$temperature."','".$humidity."')";
		
		$conn->query($sql);
		$conn->close();		
	}
}



function input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}
?>