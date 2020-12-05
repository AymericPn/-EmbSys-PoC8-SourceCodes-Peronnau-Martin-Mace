
<?php // this program is called in the arduino code when the 5 seconds timer is finished
class data{// php class for connecting to the database and storing in the database
	public $link='';
	function __construct($sound,$pollution){// constructor of the class
		$this->connect(); //calls the functions connect and storeInDB when an instance of the data class is called
		$this->storeInDB($sound,$pollution);
}

function connect(){
	$this->link = mysqli_connect('localhost:3307','root','') or die('Cannot connect to the DB'); //try to connect to the db or display an error message on the webpage
	mysqli_select_db($this->link,'POC') or die('Cannot select the Database');//POC is the name of the database
}

function storeInDB($sound,$pollution){
	$query = "insert into sound_value set value = '".$sound."'";// insert query  
	$query2 = "insert into pollution_ratio set pollution_value = '".$pollution."'";
	$result = mysqli_query($this->link,$query) or die('Errant query: '.$query);
	$result2 = mysqli_query($this->link,$query2) or die ('Errant query: '.$query2);
}

}

if ($_GET['sound'] != '' and $_GET['pollution'] != ''){// Php GET to read the value associated to sound and pollution in the url 
	$data = new data($_GET['sound'],$_GET['pollution']);
}
?>