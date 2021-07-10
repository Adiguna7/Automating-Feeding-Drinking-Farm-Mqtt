#include "ESP8266WiFi.h"
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <Servo.h>
#include <DHT.h>

#define triggerPin  12
#define echoPin     13
#define ledPin 4
#define pinServo 16
#define DHTPIN 5
#define DHTTYPE DHT11

const char* ssid = "TS_A37F";
const char* password = "qwertyuiop12345678";
//int port = 8080;

// MQTT Broker
const char *mqtt_broker = "13.94.35.89";
const char *topic_makan = "esp8266/srf_makan";
const char *topic_suhu = "esp8266/dht_suhu";
const char *mqtt_username = "kelompokwsn";
const char *mqtt_password = "kelompokwsn123";
const int mqtt_port = 1883;

WiFiClient espclient;
PubSubClient client(espclient);

DHT dht(DHTPIN, DHTTYPE);

//servo
Servo servo;

long duration, jarak;

void setup(){
  Serial.begin(115200);
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);

  dht.begin();

  //  led
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);  

  servo.attach(pinServo);
  servo.write(0);
  
  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print("disconnect");
  }
  Serial.println("");
  Serial.println("Wifi Connected");
  Serial.println(WiFi.localIP());
  delay(5000);

  //connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
      String client_id = "esp8266-client-";
      client_id += String(WiFi.macAddress());
      Serial.printf("The client %s connects to the %s mqtt broker\n", client_id.c_str(), mqtt_broker);
      if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
          Serial.println("mqtt broker connected");
      }
      else {
          Serial.print("failed with state ");
          Serial.print(client.state());
          delay(2000);
      }
   }
   client.subscribe("esp8266/servo_makan");
   client.subscribe("esp8266/led_suhu");
}

void callback(char *topic, byte *payload, unsigned int length) {
   Serial.print("Message arrived in topic: ");
   Serial.println(topic);
   Serial.print("Message:");
   String messageTemp;
   for (int i = 0; i < length; i++) {
       Serial.print((char) payload[i]);
       messageTemp += (char) payload[i];
   }
   Serial.println();
   Serial.println("-----------------------");
             
   if(messageTemp == "servo-active"){        
      servo.write(120);
      delay(5000);
      servo.write(0);
   }            
   if(messageTemp == "led-deactive"){
      Serial.println("led-deactive");
      digitalWrite(ledPin, LOW);  
   }
   if(messageTemp == "led-active"){
      Serial.println("led-active");
      digitalWrite(ledPin, HIGH);
   }         
}

void loop(){  
  // SENSOR DHT 
  float h = dht.readHumidity();
  float temperature = dht.readTemperature();
  // SENSOR ULTRASONIK   
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2); 
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10); 
  digitalWrite(triggerPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  jarak = (duration/2) / 29.4;
  Serial.println("jarak :");
  Serial.print(jarak);
  Serial.println(" cm");
  Serial.print(temperature);
  Serial.println(" celcius");

//  if(jarak > 50 && jarak < 80){
//    digitalWrite(ledPin, HIGH);  
//  }
//  else{
//    digitalWrite(ledPin, LOW);  
//  }
  //  delay(200);
  // WIFI DAN WEBSOCKET  
  //  String data_json = "{\"device_type\":\"nodemcu\",\"data_sensor\":\"123\"}";
  StaticJsonDocument<200> data_json_makan;
  StaticJsonDocument<200> data_json_suhu;
  
  data_json_makan["device_type"] = "srf";
  data_json_makan["data_sensor"] = jarak;

  data_json_suhu["device_type"] = "dht11";
  data_json_suhu["data_sensor"] = temperature;
  
  char data_json_send_makan[100];
  size_t len_makan = serializeJson(data_json_makan, data_json_send_makan);

  char data_json_send_suhu[100];
  size_t len_suhu = serializeJson(data_json_suhu, data_json_send_suhu);
  
  client.publish(topic_makan, data_json_send_makan);
  client.publish(topic_suhu, data_json_send_suhu);
  client.loop();
  delay(500);
}
