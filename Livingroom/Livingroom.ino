#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <DHT.h>   
#include <Wire.h> 
#include <Servo.h>      

//#define FIREBASE_HOST "iot-control-2-e5398-default-rtdb.firebaseio.com"
//#define FIREBASE_AUTH "775BO6fUCY8PArfNzLjMj8zZvYemQWtGcvFT2EqP"

#define FIREBASE_HOST ""
#define FIREBASE_AUTH ""

#define WIFI_SSID ""
#define WIFI_PASSWORD ""

//#define WIFI_SSID "LAPTOP-P5FMVFSS 7890"
//#define WIFI_PASSWORD "%x8872M5"
String fireStatus = "";  
String fireStatus_fan = "";  
String fireStatus_door = "";  
String fireStatus_doorwarning = "";  
String door_outside = "";
String door_theft = "";
String speaker_controll = "";
String speaker_controll_guest = "";

bool checkDoor;
int pos = 0;      

int count = 0;
bool guest = false;
bool theft = false;
                                                                                             
int led = 0; 
int led_door = 15;

// gán chân cho còi báo động
int speaker = 5;

int fan_liv = 3;

int pir = 12;

//Đật tên cho biến Servo
Servo my_door;

#define DHTPIN 14                                                        // D5
#define DHTTYPE DHT11                                                     // select dht type as DHT 11 or DHT22
DHT dht(DHTPIN, DHTTYPE);     

void setup() {
  Serial.begin(9600);
  delay(1000);
  pinMode(LED_BUILTIN, OUTPUT);      
  pinMode(led, OUTPUT);  
  pinMode(led_door, OUTPUT);   

  pinMode(pir, INPUT);   

  pinMode(fan_liv, OUTPUT);

  // setup cho loa báo động
  pinMode(speaker, OUTPUT);

  my_door.attach(13);
             
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
                                         
  dht.begin();                                                               //Start reading dht sensor
}

void loop() {

 //  digitalWrite(led_door, HIGH);   

  // Xử lý đèn phòng khách
  fireStatus = Firebase.getString("/Livingroom/Light");                                      // get ld status input from firebase
  if (fireStatus == "Open") {                                                          // compare the input of led status received from firebase
    Serial.println("Led Turned ON");                         
    digitalWrite(LED_BUILTIN, LOW);                                                  // make bultin led ON
    digitalWrite(led, HIGH);                                                         // make external led ON
  } 
  else if (fireStatus == "Close") {                                                  // compare the input of led status received from firebase
    Serial.println("Led Turned OFF");
    digitalWrite(LED_BUILTIN, HIGH);                                               // make bultin led OFF
    digitalWrite(led, LOW);                                                         // make external led OFF
  }
  else {
    Serial.println("Wrong Credential! Please send ON/OFF");
  }

  // xử lý điều khiển Quạt phòng khách
   fireStatus_fan = Firebase.getString("/Livingroom/Fan");                                      // get ld status input from firebase
  if (fireStatus_fan == "Open") {                                                          // compare the input of led status received from firebase
    analogWrite(fan_liv, 254);//Gán tốc độ quay của motor                                                                                            
  } 
  else if (fireStatus_fan == "Close") {                                                  // compare the input of led status received from firebase
  analogWrite(fan_liv, 0);//Gán tốc độ quay của motor                                                    
  }
  else {
    Serial.println("Wrong Credential! Please send ON/OFF");
  }
  

// Xử lý nhiệt độ độ ẩm

float  doam = dht.readHumidity();                                              // Reading temperature or humidity takes about 250 milliseconds!
float nhietdo = dht.readTemperature();                                           // Read temperature as Celsius (the default)
    
  Serial.print(doam);
  Serial.print(nhietdo);
                                                    //convert integer temperature to string temperature
  Firebase.setFloat("/Livingroom/Humidity", doam);                                  //setup path and send readings
  Firebase.setFloat("/Livingroom/Temperature", nhietdo);                                //setup path and send readings
  delay(1000);

  // xử lý điều khiển cửa ra vào
fireStatus_door = Firebase.getString("/Security/Door_status"); 
                              
  if (fireStatus_door == "Open" && checkDoor == false) {  
    checkDoor = true;                                                       
    for(pos = 180; pos > 0; pos -= 1) {                           
        my_door.write(pos);
        delay(10);
    }                                                     
  } 
  else if (fireStatus_door == "Close" && checkDoor == true) { 
    checkDoor = false;                                                 
    for(pos = 0; pos <= 180; pos += 1) {                           
        my_door.write(pos);
        delay(10);
    }                                                  
  }
  else {
    Serial.println("Wrong Credential! Please send ON/OFF");
  }

  // Xử lý bật đèn của ngõ và phát hiện có người đứng trước cửa

  if (digitalRead(pir) == HIGH) {
    Serial.println("Door LED Turned ON");                         
    digitalWrite(LED_BUILTIN, LOW);                                                  // make bultin led ON
    digitalWrite(led_door, HIGH);   
    delay(5000);
    count ++ ;    

    // cảnh báo có thể có trộm
     door_theft = Firebase.getString("/Security/Warning");  
    if (door_theft == "Suspicious object"){
      Firebase.setString("/Security/Door", "Theft"); 
      Firebase.setString("/Security/Door_solve", "Abnormal");  
       digitalWrite(speaker, HIGH);
       theft = true;
    }

    
// cảnh báo có khách đến thăm hoặc có người muốn vào nhà
   if (count > 2){
    Firebase.setString("/Security/Door", "Guest");  
    Firebase.setString("/Security/Door_solve", "Abnormal");  
    digitalWrite(speaker, HIGH);
    guest = true;
     count = 0 ;
    } 
  }

   door_outside = Firebase.getString("/Security/Door");  

   // thiết lập lại bình thường khi đã phát hiện ra có thể có  trộm
   if (door_outside == "Normal" && theft == true){
    Serial.println("Door Led Turned OFF");
     digitalWrite(LED_BUILTIN, HIGH);                                             
     digitalWrite(led_door, LOW);   
     digitalWrite(speaker, LOW);
     theft = false;  
     }
   
   
    // thiết lập trở lại bình thường khi biết đã có khách đến nhà     
    if (door_outside == "Normal" && guest == true){
    Serial.println("Door Led Turned OFF");
     digitalWrite(LED_BUILTIN, HIGH);     
      digitalWrite(speaker, LOW);    
      delay(3000);                                    
    digitalWrite(led_door, LOW);   
    guest = false;  
     }

//xử lý còi báo động khi có hiện tượng bất thường như cháy, gas, đối tượng khả nghi
 speaker_controll = Firebase.getString("/Security/Warning");  
 speaker_controll_guest = Firebase.getString("/Security/Door");  
 if (speaker_controll == "Suspicious object" || speaker_controll == "Gas leakage" || speaker_controll == "There is fire"){
  digitalWrite(speaker, HIGH);
 } else if (speaker_controll == "Normal" && speaker_controll_guest != "Guest"  ){
  digitalWrite(speaker, LOW);
 }
 
}
