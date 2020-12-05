<?php
//setting header to json
header('Content-Type: application/json');// this script will return data in json type

//database
define('DB_HOST', '127.0.0.1:3307');
define('DB_USERNAME', 'root');
define('DB_PASSWORD', '');
define('DB_NAME', 'POC');

//get connection to the POC database
$mysqli = new mysqli(DB_HOST, DB_USERNAME, DB_PASSWORD, DB_NAME);

if(!$mysqli){
  die("Connection failed: " . $mysqli->error);
}

//query to get data from the table

$query = sprintf("SELECT s.time, s.value, p.pollution_value FROM sound_value AS s LEFT JOIN pollution_ratio AS p ON s.time=p.time");
//execute query
$result = $mysqli->query($query);

//loop through the returned data
$data = array();

foreach ($result as $row){
  $data[] = $row;
}

//free memory associated with result
$result->close();
//close connection
$mysqli->close();
//now print the data
print json_encode($data);
?>