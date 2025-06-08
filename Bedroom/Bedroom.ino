#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <DHT.h>         
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
String fireStatus_kit_light = "";
String fireStatus_check = "";

bool checkLine;
int pos = 0;   
                                                                                             
int led = 0;   
int led_kit = 15;                                                                
int fan_bed = 13;

//Đật tên cho biến Servo
Servo my_line;

// đặt tên cho cảm biến PIR

int pir = 4;


// Đạt tên cho cảm biến mưa

int rain = 1;

String data;
String data_10;
int count = 0;

String send_data;
String send_data_10;

#define DHTPIN 14                                                        // D5
#define DHTTYPE DHT11                                                     // select dht type as DHT 11 or DHT22
DHT dht(DHTPIN, DHTTYPE);    



void setup() {
  
  Serial.begin(9600);
  delay(1000);
  pinMode(LED_BUILTIN, OUTPUT);      
  pinMode(led, OUTPUT);  
  pinMode(led_kit, OUTPUT);  

  pinMode(pir, INPUT);  
  pinMode(rain, INPUT); 

  pinMode(fan_bed, OUTPUT);

  my_line.attach(5);
    
  
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
  // Đọc thông số nhiệt độ
float t = dht.readTemperature();                                           // Read temperature as Celsius (the defaut

  // Xử lý đèn phòng ngủ
  fireStatus = Firebase.getString("/Bedroom/Light");                                      // get ld status input from firebase
  if (fireStatus == "Open") {                                                          // compare the input of led status received from firebase
    Serial.println("Led Turned ON");                         
    digitalWrite(LED_BUILTIN, LOW);                                                  // make bultin led ON
    digitalWrite(led, HIGH); 
   
  } 
  else if (fireStatus == "Close") {                                                  // compare the input of led status received from firebase
    Serial.println("Led Turned OFF");
    digitalWrite(LED_BUILTIN, HIGH);                                               // make bultin led OFF
    digitalWrite(led, LOW);                                                         // make external led OFF
  }
  else {
    Serial.println("Wrong Credential! Please send ON/OFF");
  }


  // Xử lý đèn nhà bếp
  fireStatus_kit_light = Firebase.getString("/Kitchen/Light");                                      // get ld status input from firebase
  if (fireStatus_kit_light == "Open") {                                                          // compare the input of led status received from firebase
    Serial.println("Led kitchen Turned ON");      
     digitalWrite(LED_BUILTIN, LOW);                                                               
    digitalWrite(led_kit, HIGH);                                                         // make external led ON
  } 
  else if (fireStatus_kit_light == "Close") {                                                  // compare the input of led status received from firebase
    Serial.println("Led kitchen Turned OFF");   
     digitalWrite(LED_BUILTIN, HIGH);      // make bultin led OFF
    digitalWrite(led_kit, LOW);                                                         // make external led OFF
  }
  else {
    Serial.println("Wrong Credential! Please send ON/OFF");
  }

  // xử lý điều khiển Quạt phòng ngủ
   fireStatus_fan = Firebase.getString("/Bedroom/Fan");                                      // get ld status input from firebase
  if (fireStatus_fan == "Open" && count == 0) {                                                          // compare the input of led status received from firebase
      if ( t >= 17 && t <= 45 ) {
     // int motor_bed_speed = map(t,17,45,0,255);//chuyển thang đo của value từ 20 đến 45 độ C sang thang 255 đến 0
    //  analogWrite(fan_bed, 255- motor_bed_speed);//Gán tốc độ quay của motor

     // Firebase.setFloat("/Bedroom/Fan_controll",round(motor_bed_speed/25) ); 
      
     // Firebase.setFloat("/Bedroom/Fan_speed", round(motor_bed_speed/25)); 
      data = "";
      }else {
      analogWrite(fan_bed, 0);//Gán tốc độ quay của motor 
      Firebase.setFloat("/Bedroom/Fan_speed", 0); 
      data = "";
      }
    }

                                                             
  else if (fireStatus_fan == "Close") {                                                  // compare the input of led status received from firebase
   analogWrite(fan_bed, 0);//Gán tốc độ quay của motor       
   Firebase.setFloat("/Bedroom/Fan_speed", 0);                                           
  }
  else {
    Serial.println("Wrong Credential! Please send ON/OFF");
  }

   // Điều khiển tốc độ quay của quạt
    data = Firebase.getString("/Bedroom/Fan_controll");    
    data_10 = Firebase.getString("/Bedroom/Fan_controll");      

 if (fireStatus_fan == "Open"){
      if (data_10 == "10"){
      analogWrite(fan_bed, 255);//Gán tốc độ quay của motor 
      Firebase.setString("/Bedroom/Fan_speed", data_10); 
      data = "";
    }
 }
     int motor_bed_speed;
     
 if (fireStatus_fan == "Open"){
    if (data >= "1" && data <= "9"){
      count = 1;
      if (data == "1"){
         motor_bed_speed = 1 ;
      } else if  (data == "2"){
         motor_bed_speed = 2 ;
      } else if  (data == "3"){
         motor_bed_speed = 3 ;
      } else if  (data == "4"){
         motor_bed_speed = 4 ;
      } else if  (data == "5"){
         motor_bed_speed = 5 ;
      } else if  (data == "6"){
         motor_bed_speed = 6 ;
      } else if  (data == "7"){
         motor_bed_speed = 7 ;
      } else if  (data == "8"){
         motor_bed_speed = 8 ;
      } else if  (data == "9"){
         motor_bed_speed = 9 ;
      }
     
      analogWrite(fan_bed, 216 + 4 * motor_bed_speed);//Gán tốc độ quay của motor 
      Firebase.setString("/Bedroom/Fan_speed", data); 
      data = "";
      }   
 }       
 
// Xử lý nhiệt độ độ ẩm

float  doam = dht.readHumidity();                                              // Reading temperature or humidity takes about 250 milliseconds!
float nhietdo = dht.readTemperature();                                           // Read temperature as Celsius (the default)
    
  Serial.print(doam);
  Serial.print(nhietdo);
                                                    //convert integer temperature to string temperature
  Firebase.setFloat("/Bedroom/Humidity", doam);                                  //setup path and send readings
  Firebase.setFloat("/Bedroom/Temperature", nhietdo);                                //setup path and send readings
  delay(1000);

  //fireStatus_check = Firebase.getString("/Kitchen/Clothes_line");   
if (fireStatus_check == "Out"){
  checkLine = true;
} else if (fireStatus_check == "In"){
 checkLine = false;
} else if (fireStatus_check == "Rain_In"){
 checkLine = false;
}

// xử lý dây phơi quần áo kéo vào kéo ra nếu không có mưa
fireStatus = Firebase.getString("/Kitchen/Clothes_line"); 
                              
  if (fireStatus == "Out" && checkLine == false) {  
    checkLine = true;                                                       
         for(pos = 0; pos <= 100; pos += 1) {                           
        my_line.write(pos);
        delay(10);
    }                                                     
  } 
  else if (fireStatus == "In" && checkLine == true) { 
    checkLine = false;                                                 
    for(pos = 100; pos > 0; pos -= 1) {                           
        my_line.write(pos);
        delay(10);
    }                                                  
  }
  else {
    Serial.println("Wrong Credential! Please send ON/OFF");
  }


// xử lý cảm biến mưa
  if (digitalRead(rain) == LOW){
    // kéo dây phơi đồ vào bẳng servo
     Firebase.setString("/Security/Warning", "Rain");
     Firebase.setString("/Kitchen/Clothes_line", "Rain_In");

    if (checkLine == true) {
      for(pos = 100; pos > 0; pos -= 1) {                           
        my_line.write(pos);
        delay(10);

    } 
    checkLine = false; 
    }
   
   }  else {
     if (fireStatus == "Rain_In" && checkLine == false){
      Firebase.setString("/Kitchen/Clothes_line", "In");
     }
   }

   // xử lý với cảm biển PIR
  if (digitalRead(pir) == HIGH){
   Firebase.setString("/Security/Warning", "Suspicious object");
   delay(5000);
   } 
   
}
