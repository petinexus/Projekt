<!DOCTYPE html>
<html><body>
<?php

$server = "localhost";
$dbname = "esp_adat";
$username = "root";
$password = "";


$conn = new mysqli($server, $username, $password, $dbname);

if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} 

$sql = "SELECT * FROM adatok ORDER BY id ASC";

echo '<table cellspacing="5" cellpadding="5">
      <tr> 
        <td>ID</td> 
        <td>Hőmérséklet</td> 
        <td>Páratartalom</td> 
        <td>Dátum</td> 
      </tr>';
 
if ($result = $conn->query($sql)) {
    while ($row = $result->fetch_assoc()) {
        $row_id = $row["id"];
        $row_temperature = $row["Hőmérséklet"];
        $row_humidity = $row["Páratartalom"];
        $row_idopont = $row["Időpont"];
      
        echo '<tr> 
                <td>' . $row_id . '</td> 
                <td>' . $row_temperature . '</td> 
                <td>' . $row_humidity . '</td> 
                <td>' . $row_idopont . '</td> 
              </tr>';
    }
    $result->free();
}

$conn->close();
?> 
</table>
</body>
</html>