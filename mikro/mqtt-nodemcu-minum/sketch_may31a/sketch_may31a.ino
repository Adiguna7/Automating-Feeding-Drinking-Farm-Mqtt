#define pinSensor A0
#define pinLed 4

int sensorValue = 0; // variable untuk menampung nilai baca dari sensor dalam bentuk integer
float tinggiAir = 0; // variabel untuk menampung ketinggian air
float sensorVoltage = 0; // untuk menampung nilai ketinggian air


int nilaiMax = 1023; // nilai "sensorValue" saat sensor terendam penuh kedalam air, bisa dirubah sesuai sensor dan jenis air yang anda pakai
float panjangSensor = 4.0 ; // 4.0 cm, bisa dirubah, menyesuikan dengan panjang sensor yang kalian gunakan

//SETUP WIFI
const char* ssid = "ssidmu";
const char* password = "passwifimu";

//SETUP MQTT
const char *mqtt_broker = "13.94.35.89";
const char *topic = "esp8266/srf_minum";
const char *mqtt_username = "kelompokwsn";
const char *mqtt_password = "kelompokwsn123";
const int mqtt_port = 1883;

WiFiClient espclient;
PubSubClient client(espclient);

void setup() {
  Serial.begin(115200); // mengatur baudrate komunikasi serial antara arduino dengan PC
  pinMode(pinLed, OUTPUT);   
  digitalWrite(pinLed, LOW); 

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
   client.subscribe("esp8266/servo_minum");
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

   if(messageTemp == "active"){
     digitalWrite(pinLed, HIGH);  
     delay(500);
   }
}

void loop() {
  digitalWrite(pinLed, LOW);
  sensorValue = analogRead(pinSensor); // membaca tengan dari sensor dalam bentuk integer
  tinggiAir = sensorValue * panjangSensor / nilaiMax;
  sensorVoltage = sensorValue * 5.0 / 1023;  

  StaticJsonDocument<200> data_json;
  data_json["device_type"] = "nodemcu";
  data_json["data_sensor"] = tinggiAir;

  char data_json_send[100];
  size_t len = serializeJson(data_json, data_json_send);
  
  client.publish(topic, data_json_send);
  client.loop();  

  Serial.print("Sensor Value = ");
  Serial.println(sensorValue);
  Serial.print("Sensor Voltage = ");
  Serial.println(sensorVoltage);
  Serial.print("Tinggi Air = ");
  Serial.println(tinggiAir);
  Serial.println();

  delay(500);  
}
