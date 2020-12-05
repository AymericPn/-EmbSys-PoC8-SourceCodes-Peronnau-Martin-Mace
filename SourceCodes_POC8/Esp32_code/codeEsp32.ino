#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>

//traffic lights + distance sensor
const int greenLed1 = 15;
const int orangeLed1 = 2;
const int redLed1 = 4;
const int greenLed2 = 5;
const int orangeLed2 = 18;
const int redLed2 = 19;
const int triggerPin = 33;// pin used to send a 10 microseconds high voltage
const int echoPin = 23;// pin used to read the lenght response of the sensor
bool presence = false;// used to check if the light was already on
const float triggerDistance = 10;// minimum distance to trigger the light change
bool carApproaching;// check if a car is at a distance lower than the trigger distance from the sensor
//gas sensor + lcd screen
const int gasInPin = 34;
const float R0 = 1.7;//sensor resistance in the clean air
float pollutionRatio;
bool pollutionBool = false; //if the message is already being displayed, it's equal to true
const int lcdColumns = 16;//number of columns on the LCD
const int lcdRows = 2;//number of rows
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);
//sound sensor
const int soundAnaIn = 35;//analog pin of the sound sensor
int soundValue;
//timing variables
long timer;
long timeIntervals = 5000;
//wifi
const char* ssid = "iPhone";
const char* password =  "jarjarbiks";
const char* host = "172.20.10.2";
const int httpPort = 80;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {//wait while the board isn't connected to Wifi
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  lcd.init();
  lcd.backlight();
  pinMode(greenLed1, OUTPUT);
  pinMode(orangeLed1, OUTPUT);
  pinMode(redLed1, OUTPUT);
  pinMode(greenLed2, OUTPUT);
  pinMode(orangeLed2, OUTPUT);
  pinMode(redLed2, OUTPUT);
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(soundAnaIn, INPUT);
  pinMode(gasInPin, INPUT);
  digitalWrite(redLed1, HIGH);
  digitalWrite(greenLed2, HIGH);
  displayCleanAirMessage();// by default we display the message that the air is clean (carbon monoxyde concentration below the trigger rate)
  millis();// we start a chronometer
}

void loop() {
  carApproaching = checkDistance();// we get if a car is below the trigger Distance
  if (millis() - timer >= timeIntervals)// if the duration of the time Interval has passed we gaher sound and pollution data
  {
    timer = millis();
    pollutionRatio = ratioComputing();
    soundValue = soundMonitor();
    if (pollutionRatio > 6) {
      if (pollutionBool == false)// display message in case the pollution rate is bigger than the trigger
      {
        pollutionBool = true;
        displayPollutionMessage();
      }
    }
    else {
      if (pollutionBool == true) {
        pollutionBool = false;
        displayCleanAirMessage();
      }
    }
    sendToServer(soundValue, pollutionRatio);// send the data to the server to store it in the data base and dispplay it on a graph
  }
  if (carApproaching == true)
  {
    if (presence == false) {// if the traffic light on the main road was red
      presence = true;
      lightChangeMasterToGreen();// we change it to green and change the secondary one to red
    }
  }
  else
  {
    if (presence == true) //if the traffic light on the main road was green
    {
      presence = false;
      lightChangeMasterToRed();//we change it to red and change the secondary one to green
    }
  }
}

float distance_calculation() {
  // Clears the triggerPin
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  // Sets the triggerPin on HIGH state for 10 micro seconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  long timeDistance = pulseIn(echoPin, HIGH);//duration of the trigger-reponse time
  // Calculating the distance
  float carDistance = timeDistance * 0.034 / 2;// distance between the car and the sensor
  return carDistance;
}

bool checkDistance() {
  float dist = distance_calculation();
  if (dist <= triggerDistance) {
    return true;
  }
  return false;
}

float ratioComputing() {
  int sensorValue = analogRead(gasInPin);
  float sensor_volt = (float)sensorValue / 1024 * 5.0;// convert the analog value to a voltage
  float RS_gas = (5.0 - sensor_volt) / sensor_volt;// get the value of the resistance ( the resistance increases with the rate of carbon monoyxyde. 
  float ratio = RS_gas / R0; // ratio = RS/R0
  return ratio;
}

void displayPollutionMessage()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Polluted air ! ");
  lcd.setCursor(0, 1);
  lcd.print("Wear a mask ! ");
}

void displayCleanAirMessage()
{ lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("The air is ");
  lcd.setCursor(0, 1);
  lcd.print("good today ");
}

int soundMonitor()
{
  int soundAnaValue = analogRead(soundAnaIn);
  int dBvalue = map(soundAnaValue, 0, 4095, 10, 40); // this is not an accurate way of converting analog into decibel. unfortunately we couldn't manage to do it, so we made up this map to be able to display consistent values on the chart
  return dBvalue;
}

void lightChangeMasterToGreen() {
  digitalWrite(greenLed2, LOW);
  digitalWrite(orangeLed2, HIGH);
  delay(2000);
  digitalWrite(orangeLed2, LOW);
  digitalWrite(redLed2, HIGH);
  digitalWrite(redLed1, LOW);
  digitalWrite(greenLed1, HIGH);
}

void lightChangeMasterToRed() {
  digitalWrite(greenLed1, LOW);
  digitalWrite(orangeLed1, HIGH);
  delay(2000);
  digitalWrite(orangeLed1, LOW);
  digitalWrite(redLed2, LOW);
  digitalWrite(greenLed2, HIGH);
  digitalWrite(redLed1, HIGH);
}

void sendToServer(float soundLevel, float pollutionRate)
{
  WiFiClient client;// define a wifi client to communicate with the server
  if (!client.connect(host, httpPort)) {// if we cannot connect to the host IP using http port, then there's a problem
    Serial.println("connection failed");
    return;
  }
  client.print(String("GET http://172.20.10.2/code/POC.php?") + // Get request to communicate the pollution and sound data to the server
               ("&sound=") + soundLevel +
               ("&pollution=") + pollutionRate + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {// if the data couldn't get to the server, the server would return 0 bytes
    if (millis() - timeout > 1000) {
      Serial.println(">>> Client Timeout !");
      client.stop();// stop the connection
      return;
    }
  }
}
