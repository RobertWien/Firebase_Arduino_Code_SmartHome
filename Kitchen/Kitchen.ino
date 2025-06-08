#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <DHT.h>         
#include <Wire.h> 
#include <Servo.h>   

#define FIREBASE_HOST ""
#define FIREBASE_AUTH ""

#define WIFI_SSID ""
#define WIFI_PASSWORD ""

//#define WIFI_SSID "LAPTOP-P5FMVFSS 7890"
//#define WIFI_PASSWORD "%x8872M5"

String statusLight = "";


// gán chân cho cảm biến phát hiện lửa
int fire = 5;

// gán chân cho cảm biến phát hiện khí gas
int gas = 1;

// gán chân cho cảm biến phát hiện khí gas
int light = 14;

// gán chân cho loa báo thức
int speaker = 4;

void setup() {
Serial.begin(9600);

// setup cảm biến phát hiện lửa
pinMode(fire, INPUT);

// setup cảm biến phát hiện khí gas
pinMode(gas, INPUT);

// setup cảm biến phát hiện ánh sáng
pinMode(light, INPUT);

// setup cho loa báo thức
pinMode(speaker, OUTPUT);

  delay(1000);             
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                      //try to connect with wifi
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP Address is : ");
  Serial.println(WiFi.localIP());                                                      //print local IP address
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);                                       // connect to firebase
                                                                                               
}

void loop() {


//fireStatus_check = Firebase.getString("/Kitchen/Clothes_line");   


     // xử lý với cảm biển gas

   if (digitalRead(gas) == LOW){
   Firebase.setString("/Security/Warning", "Gas leakage");
   delay(5000);
   } 
 
  

  //xủ lý với cảm biến lửa
  if (digitalRead(fire) == LOW){
  Firebase.setString("/Security/Warning", "There is fire");
  delay(5000);
   } 

// xử lý với cảm biến ánh sáng
statusLight = Firebase.getString("/Kitchen/Alarm"); 
if (digitalRead(light) == LOW && statusLight == "Open"){
  digitalWrite(speaker,HIGH);
}

if (statusLight == "Close" || digitalRead(light) == HIGH ){
  digitalWrite(speaker,LOW);
}
 
}
