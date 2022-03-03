#include <ESP32Servo.h>
#define BLYNK_PRINT Serial
#include<DHT.h>
#include<WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>



char auth[] = "HkWbsy9yXSMfyt828ewPcqoGUdAJU_wA";
String apiKey ="5EN9524FJWSTIY6D";
const char *ssid= "SaneVoid";
const char *pass= "BlindMM90";
const char *server= "api.thingspeak.com";

int pos = 0;
const int trigPin = 4 ;
const int echoPin = 2 ;
const int trigPin2 = 19 ;
const int echoPin2 = 18 ;
#define DHTPIN 23      // What digital pin we're connected to DHT11
DHT dht(DHTPIN, DHT11);
const int ledRed = 12 ; // temperature's led
const int ledGreen = 14 ; // Distance's led
const int ledBlue = 13 ; // humidity's led
WiFiClient client;
long duration,distance,duration2,distance2; 
int servoPin = 15;

Servo servol;

void setup(){
  
  servol.attach(servoPin);
  pinMode(trigPin,OUTPUT); // Sets the trigpin as an output
  pinMode(echoPin,INPUT); // Sets the echoPin as an Input
  pinMode(trigPin2,OUTPUT);
  pinMode(echoPin2,INPUT);
  pinMode(ledRed,OUTPUT);// temperature's LED
  pinMode(ledGreen,OUTPUT);// Distance's LED
   pinMode(ledBlue,OUTPUT);
  Serial.begin(115200);//Starts the serial communication
  Blynk.begin(auth, ssid, pass);
  delay(100);
  dht.begin();
  Serial.println("Connecting to");
  Serial.println(ssid);
  WiFi.begin(ssid,pass);
  while (WiFi.status()!=WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
    Serial.println("");
    Serial.println("WiFi connected");
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  servol.setPeriodHertz(50); // standard 50hz servo
  servol.attach(servoPin, 500, 2400);
 
  }
 void loop(){

  WidgetLCD lcd(V2);
  digitalWrite(trigPin, LOW);
  delayMicroseconds (2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds (10);
  digitalWrite(trigPin, LOW);// Reads the echopin, returns the sound wave travel time in microseconds
  duration=pulseIn(echoPin,HIGH);//calculating the distance
  distance=duration*0.034/2;//Prints the distance on the Serial Monitor
  
  digitalWrite(trigPin2, LOW);
  delayMicroseconds (2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds (10);
  digitalWrite(trigPin2, LOW);// Reads the echopin, returns the sound wave travel time in microseconds
  duration2=pulseIn(echoPin2,HIGH);//calculating the distance
  distance2=duration2*0.034/2;//Prints the distance on the Serial Monitor
  
  Serial.print("Distance:");
  Serial.print(distance);
  Serial.print("Distance2:");
  Serial.print(distance2);

  float h =dht.readHumidity();
  float t =dht.readTemperature();
      if (isnan(h) || isnan(t))
           {
              Serial.println("Failed to read from DHT sensor!");
               return;
           }
            if(t > 30) {
      Blynk.notify("The temperature is more than 30 degrees!");
      delay(100);
    }
           if(client.connect(server,80))
           {
                String postStr = apiKey;
                postStr +="&field1=";
                postStr += String(t);
                postStr +="&field2=";
                postStr += String(h);
                postStr +="&field3=";
                postStr += String(distance);                
                postStr +="\r\n\r\n";
               
                client.print("POST /update HTTP/1.1\n");
                client.print("Host: api.thingspeak.com\n");
                client.print("Connection: close\n");
                client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                client.print("Content-Type: application/x-www-form-urlencoded\n");
                client.print("Content-Length: ");
                client.print(postStr.length());
                client.print("\n\n");
                client.print(postStr);

                delay(100);

                Serial.print("Temperature: ");
                Serial.print(t);
                Serial.print(" degrees Celcius, Humidity: ");
                Serial.print(h);
                Serial.print("distance: ");
                Serial.print(distance);

                delay(100);

                Serial.print("Temperature: ");
                Serial.print(t);
                Serial.print(" degrees Celcius, Humidity: ");
                Serial.print(h);
                Serial.print("distance: ");
                Serial.print(distance);
                Serial.print("%. Send to Thingspeak.");

            }
 
  client.stop();

  Serial.println("Waiting...");

  // thingspeak needs minimum 15 sec delay between updates, i've set it to 30 seconds
  delay(500);
  
   if (distance2 <=10) { 
  servol.attach(servoPin);
  servol.write(0);
  delay(100);
 }

 else {
  servol.write(90);
  delay(100);
  
 }
  if (distance< 4){
    digitalWrite(ledGreen,HIGH);
    lcd.clear();
    lcd.print(0,0, "The Bin is Full");
    delay(500);
   }
   else if (distance <= 8){
    lcd.clear();
    lcd.print(0,0, "The Bin is Half Full");
    delay(500);
   }
   else{
    digitalWrite(ledGreen,LOW);
    lcd.clear();
    lcd.print(0,0, "The Bin is empty");
    delay(500);
   }
  
   if (t>20){  
  digitalWrite(ledRed,HIGH);
  lcd.clear();
  lcd.print(0,0, "The Bin is Overheated");
  delay(500);
   }
  else{
  digitalWrite(ledRed,LOW);
  lcd.clear();
    lcd.print(0,0, "The Bin is room temperature");
    delay(500);
    
    if (h>20){  
  digitalWrite(ledBlue,HIGH);
  lcd.clear();
  lcd.print(0,0, "High Humidity found, Change bag ");
  delay(500);
   }
  else{
  digitalWrite(ledBlue,LOW);
  lcd.clear();
    lcd.print(0,0, "Humidity is normal");
    delay(500);
  }
  Blynk.virtualWrite(V0,distance);
  Blynk.virtualWrite(V1,t);
  Blynk.run();
 }
 }
