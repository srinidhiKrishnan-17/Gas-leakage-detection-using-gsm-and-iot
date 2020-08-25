#include <SoftwareSerial.h>   //Header file to create RX & Tx 
#include<stdlib.h>
float field1;
String apiKey = "9E0THDDMCKVVMMRF";
SoftwareSerial serial1(2, 3); // RX, TX - connect Rx of Esp8266 - 3, Tx - 2 
SoftwareSerial serial2(0, 1);
// this runs once
int redLed = 12;
int greenLed = 11;
int buzzer = 10;
int smokeA0 = A5;
int servopin = 1500;
// Your threshold value
int sensorThres = 300;
void setup() 
{
  // put your setup code here, to run once:
pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(smokeA0, INPUT);
   pinMode(servopin, OUTPUT);
   Serial.begin(115200);       //Baud rate for serial communication
  // enable software serial
  serial1.begin(115200);    // Baud rate for ESP8266
  serial1.println("AT"); 
  delay(500);
  serial1.println("AT+RST");    //RESET
  delay(500);
  serial1.println("AT+CWMODE=1");   //Station
  delay(500);
  // ("AT+CWJAP=\"ssid\",\"password\"");
  serial1.println("AT+CWJAP=\"OPPO A37fw\",\"12345678\"");
  delay(500);
}

void loop() {
int analogSensor = analogRead(smokeA0);
field1 = analogSensor;

  Serial.print("Pin A0: ");
  Serial.println(analogSensor);
  // Checks if it has reached the threshold value
  if (analogSensor > sensorThres)
  {
    digitalWrite(redLed, HIGH);
    digitalWrite(greenLed, LOW);
    tone(buzzer, 1000, 200);
    digitalWrite(servopin, HIGH);
    serial2.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
    delay(1000);  // Delay of 1000 milli seconds or 1 second
    serial2.println("AT+CMGS=\"+8248041948\"\r"); // Replace x with mobile number
    delay(1000);
    serial2.println("GAS LEAKING!!!");// The SMS text you want to send
    delay(100);
    serial2.println((char)26);// ASCII code of CTRL+Z
    delay(1000);
    serial2.println("AT");
    serial2.print("ATD");
    serial2.print("+918248041948");
    serial2.println(";");
    delay(45000);
    serial2.println("ATH");
  }
  else
  {
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, HIGH);
    digitalWrite(servopin, LOW);
    noTone(buzzer);
    delay(1000); 
    serial2.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
    delay(1000);  // Delay of 1000 milli seconds or 1 second
    serial2.println("AT+CMGS=\"+918248041948\"\r"); // Replace x with mobile number
    delay(3000);
    serial2.println("I am SMS from GSM Module NO LEAKAGE");// The SMS text you want to send
    delay(3000);
    serial2.println((char)26);// ASCII code of CTRL+Z
    delay(1000);
    serial2.println("AT");
    serial2.print("ATD");
    serial2.print("+918248041948");
    serial2.println(";");
    delay(45000);
    serial2.println("ATH");
  }
   delay(500);
  // TCP connection
  // AT+CIPSTART = <type>,<addr>,<port>
  String cmd = "AT+CIPSTART=\"TCP\",\""; 
  cmd += "184.106.153.149"; // api.thingspeak.com
  cmd += "\",80";
  serial1.println(cmd);
  Serial.println(cmd);
  if(serial1.find("error"))
  {
    Serial.println("AT+CIPSTART error");
    return;
  }
  
  //GET https://api.thingspeak.com/update?api_key=TMWR47CL9G5PTZHM&field1=0&field2=0
  //GET /update?api_key=TMWR47CL9G5PTZHM&field1=31&field2=73
  String getStr = "GET /update?api_key=";
  getStr += apiKey;
  getStr +="&field1=";
  getStr += String(field1);
  getStr += "\r\n\r\n";
  // send data length
  cmd = "AT+CIPSEND=";        // Sending data
  cmd += String(getStr.length());
  serial1.println(cmd);
  Serial.println(cmd);

  if(serial1.find(">")){
    serial1.print(getStr);
    Serial.print(getStr);
  }
  else{
    serial1.println("AT+CIPCLOSE");  // Closing TCP Connection
    // alert user
    Serial.println("AT+CIPCLOSE");
  }
  // thingspeak needs 5-10 sec delay between updates
  delay(1000); 
}
