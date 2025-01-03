ESP8266 IoT-Based Telemetry System
Features
Connects to Wi-Fi to send telemetry data, including temperature, humidity, battery percentage, speed, and charging time. Operates in two modes:

Charging: Battery percentage increases.
Driving: Simulates speed and drains the battery.
Requirements
Hardware
ESP8266 (e.g., NodeMCU) and DHT11 sensor.

Software
Arduino IDE with PubSubClient and DHT Sensor libraries, and a ThingsBoard account.

Setup
Install the required libraries in Arduino IDE.
Update the Wi-Fi name (SSID), password, and ThingsBoard token in the code.
Upload the code to the ESP8266.
Monitor the data using the Serial Monitor or the ThingsBoard dashboard.
Outputs
Battery level (chargePercent), speed in km/h (speed), temperature and humidity from the DHT11 sensor, and estimated charging time.
