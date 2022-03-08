<?php
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "maxserver";
// Create connection
$conn = new mysqli($servername, $username, $password,$dbname);

// Check connection
if ($conn->connect_error) {
  die("Connection failed: " . $conn->connect_error);
}
echo "Connected successfully<br>";

$sql ="SELECT * FROM `devices` WHERE `Name` = '".$_POST["ID"]."'";
$result = $conn->query($sql);
$devname = $result->fetch_array()[0] ?? '';
if ($devname!='') {
    echo "Hello ".$_POST["ID"]."<br>" ;
  } else {
    echo "Error: Unauthorized access<br>".$conn->error ;
    exit;
  }
$sql ="UPDATE `devices` SET `Last Connect` = NOW() WHERE `devices`.`Name` = '".$_POST["ID"]."'";
if($conn->query($sql)=== TRUE){
    echo "Process complete<br>";
}
else {
    echo "Error updating record<br>".$conn->error;
  }
$conn->close();
?>