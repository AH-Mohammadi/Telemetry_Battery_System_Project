#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// Wi-Fi and ThingsBoard Information
const char* SSID = "**";
const char* PASS = "*****";
const char* TOKEN = "ipt7MDbpgbN8v6skRSLz";
const char* TB_SERVER = "eu.thingsboard.cloud";
constexpr uint16_t TB_PORT = 1883;

#define DHTTYPE DHT11
#define DHTPIN D5

// Battery and Speed Variables
#define BATTERY_CAPACITY 5000 // in mAh
int chargePercent = 100; // Start with 100%
bool chargeState = true;     // false: Not charging, true: Charging
float speed = 0;         // Speed in km/h
unsigned long lastUpdateTime = 0;
unsigned long chargingStartTime = 0; // To track charging start time

// DHT Sensor
DHT dht(DHTPIN, DHTTYPE);
WiFiClient wifiClient;
PubSubClient client(wifiClient);

void init_WiFi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(SSID, PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("Connected to Wi-Fi");
  Serial.print("Local IP Address: ");
  Serial.println(WiFi.localIP());
}
  
void reconnect() {
  while (!client.connected()) {
    Serial.println("Connecting to ThingsBoard...");
    if (client.connect("ESP8266", TOKEN, NULL)) {
      Serial.println("Connected to ThingsBoard server.");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void sendTelemetry(const char* key, float value) {
  String payload = "{\"";
  payload += key;
  payload += "\":";
  payload += value;
  payload += "}";
  client.publish("v1/devices/me/telemetry", payload.c_str());
}

void sendTelemetryChar(const char* key, const char* value) {
    String payload = "{\"";
    payload += key;
    payload += "\":\""; // Add opening quote for the value
    payload += value;
    payload += "\"}";   // Add closing quote for the value
    client.publish("v1/devices/me/telemetry", payload.c_str());
}

float estimateChargingTime() {
  int remainingCharge = 100 - chargePercent; // Remaining charge in percentage
  float chargeRate = 5; // Charging rate in % per 10 seconds
  return (remainingCharge / chargeRate) * 10; // Time in seconds
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  init_WiFi();
  client.setServer(TB_SERVER, TB_PORT);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long currentTime = millis();
  if (currentTime - lastUpdateTime >= 1000) {
    lastUpdateTime = currentTime;

    if (chargeState == true) { // Charging Mode
      speed = 0; // Speed is zero while charging
      chargingStartTime = chargingStartTime == 0 ? millis() : chargingStartTime;
      chargePercent = min(chargePercent + 5, 100);
    } else { // Driving Mode
      chargingStartTime = 0;
      speed = random(40, 120); // Simulate speed between 40-120 km/h
      chargePercent = max(chargePercent - 1, 0); // Discharge battery
    }

    // Send telemetry data
    sendTelemetry("chargePercent", chargePercent);
    String dataStr = chargeState ? "Charging" : "Not Charging"; 
    sendTelemetryChar("chargeState", dataStr.c_str());
    sendTelemetry("speed", speed);

    if (chargeState == true) {
      float remainingTime = estimateChargingTime();
      sendTelemetry("chargingTime", remainingTime);
      Serial.print("Estimated Charging Time: ");
      Serial.println(remainingTime);
    }

    Serial.print("Charge Percent: ");
    Serial.println(chargePercent);
    Serial.print("Charge State: ");
    Serial.println(chargeState);
    Serial.print("Speed: ");
    Serial.println(speed);

    // Read and send DHT sensor data
    float temp = dht.readTemperature() + 10 + random(1,3);
    float humid = dht.readHumidity() + random(1,10);

    if (!isnan(temp) && !isnan(humid)) {
      sendTelemetry("temp", temp);
      sendTelemetry("humid", humid);
      Serial.print("Temperature: ");
      Serial.println(temp);
      Serial.print("Humidity: ");
      Serial.println(humid);
    } else {
      Serial.println("Failed to read from DHT sensor!");
    }
  }
}
