#include <Arduino.h>
#include <WiFi.h>
#include <DHT.h>
#include <Arduino_MQTT_Client.h>
#include <ThingsBoard.h>


const char* SSID = "A24";
const char* PASS = "amir0000";
const char*  TOKEN = "KAEp61Slwvs9IIve3UZj";
const char* TB_SERVER = "eu.thingsboard.cloud";
constexpr uint16_t TB_PORT = 1883U;
constexpr uint16_t MAX_MESSAGE_SIZE = 1024U;
constexpr uint32_t SERIAL_DEBUG_BAUD = 115200U;


const int LED_PIN1 = 25;
const int LED_PIN2 = 33;
const int LED_PIN3 = 32;
const int LED_PIN4 = 26;


#define DHTTYPE DHT11
#define DHTPIN 15

DHT dht(DHTPIN, DHTTYPE);

WiFiClient wifiClient;
Arduino_MQTT_Client mqttClient(wifiClient);
ThingsBoard tb(mqttClient);


void init_WiFi(){
  Serial.println("Connecting to AP..");
  WiFi.begin(SSID, PASS);

  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
    
  }

  Serial.println("Connected to Wifi");

  Serial.print("Local IP Address: ");

  Serial.println(WiFi.localIP());



}

bool reconnect(){
  if (WiFi.status() == WL_CONNECTED){
    return true;
  }
  
  init_WiFi();
  return true;
  
}


void setup(){
  Serial.begin(115200);
  dht.begin();
  init_WiFi();

}

void loop(){
  delay(10);
  if (!reconnect()) {
    return;
  }

  if(!tb.connected()){
    if(!tb.connect(TB_SERVER, TOKEN,TB_PORT)){
      return;
    }
    Serial.println("Connected to ThingsBoard server.");

  }

  
  float temp = dht.readTemperature();
  float humid = dht.readHumidity();


  
  if(!isnan(temp) && !isnan(humid)){
    tb.sendTelemetryData("temp", temp);
    tb.sendTelemetryData("humid", humid);

    Serial.println("Temprature:");
    Serial.println(temp);

    Serial.println("Humidity");
    Serial.println(humid);


  }

  else{
    Serial.println("Failed to get data from sensors.");

  }
  tb.loop();
}