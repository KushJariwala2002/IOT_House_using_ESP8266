# IoT Smart Home Dashboard

This project allows you to control and monitor various smart devices in your home, including sensors (temperature, humidity, light, soil moisture, gas, and flame) and actuators (water pump, fan, gate/servo motor) through a simple web interface.

## Features

- **Temperature & Humidity Monitoring**: Displays live temperature and humidity readings from the **DHT11** sensor.
- **Light Detection**: Detects ambient light using the **LDR** sensor and toggles between "Day" and "Night".
- **Motion Detection**: Detects motion with the **PIR** sensor.
- **Soil Moisture Monitoring**: Displays the soil moisture level to automate watering plants using the **soil moisture sensor**.
- **Gas and Flame Detection**: Monitors gas leaks using the **MQ2** sensor and flame using the **flame sensor**.
- **Control Devices**:
  - **Water Pump**: Turns the water pump on/off for automatic irrigation.
  - **Fan**: Toggles a fan on/off for temperature control.
  - **Gate**: Controls a gate using a **servo motor** to open/close.

## Components Used

- **ESP8266**: Main microcontroller for Wi-Fi and server management.
- **DHT11**: Temperature and humidity sensor.
- **LDR**: Light-dependent resistor for light level detection.
- **PIR**: Passive infrared sensor for motion detection.
- **MQ2**: Gas sensor for detecting smoke and gases.
- **Flame Sensor**: Detects the presence of fire.
- **Soil Moisture Sensor**: Monitors soil moisture for automatic irrigation.
- **Relay Module**: Controls the **water pump** and **fan**.
- **Servo Motor**: Controls a gate (open/close).
- **DC Fan Motor**: Controls fan operation.

## Setup

### Hardware Connections

1. **LDR** - Connect to **D0**.
2. **Gate (Servo Motor)** - Connect to **D1**.
3. **Fan (Relay)** - Connect to **D2**.
4. **PIR Sensor** - Connect to **D3**.
5. **IR Sensor** - Connect to **D4**.
6. **Water Pump (Relay)** - Connect to **D5**.
7. **MQ2 Gas Sensor** - Connect to **D6**.
8. **Soil Moisture Sensor** - Connect to **A0**.
9. **DHT11 Sensor** - Connect to **D7**.

### Software Setup

1. Install the required libraries:
    - **ESP8266WiFi**: For connecting the ESP8266 to your Wi-Fi network.
    - **ESP8266WebServer**: For creating the web server.
    - **Adafruit_Sensor** & **DHT_U**: For DHT11 sensor support.
    - **Servo**: For controlling the servo motor.

2. Update the Wi-Fi credentials:
    ```cpp
    const char* ssid = "YourSSID";
    const char* password = "YourPassword";
    ```

3. Upload the code to your **ESP8266** board using the Arduino IDE.

### Web Interface

Once the ESP8266 is connected to your Wi-Fi, access the IoT Dashboard using the IP address provided in the Serial Monitor.

- **Dashboard**: Displays sensor data (Temperature, Humidity, Light, Motion, Soil Moisture, Gas, Flame).
- **Control Toggles**: Allows you to control:
  - **Water Pump**: Toggle the water pump relay.
  - **Fan**: Toggle the fan relay.
  - **Gate**: Open or close the gate using the servo motor.

## How It Works

1. The ESP8266 connects to the Wi-Fi network and starts a web server.
2. The web page is served, displaying real-time data from various sensors.
3. You can control the connected devices (water pump, fan, gate) using the web interface.
4. The **JavaScript** in the dashboard fetches sensor data every second and updates the page dynamically.

## Routes & Endpoints

- **`/`**: Displays the main dashboard page (HTML).
- **`/style.css`**: Serves the CSS file for the dashboard.
- **`/script.js`**: Serves the JavaScript file for controlling the devices and updating the sensor data.
- **`/getStatus`**: Returns a JSON response with the current sensor readings.
- **`/toggleRelay`**: Toggles the water pump relay.
- **`/toggleFan`**: Toggles the fan relay.
- **`/toggleGate`**: Toggles the gate (servo motor) open/close.

## Libraries Required

- **Adafruit DHT Unified Library**: `https://github.com/adafruit/DHT-sensor-library`
- **ESP8266WiFi**: `https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi`
- **ESP8266WebServer**: `https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer`
- **Servo Library**: Pre-installed with the Arduino IDE.

## Troubleshooting

- **Wi-Fi Connection Issues**: Ensure that your Wi-Fi credentials are correct.
- **No IP Address**: Double-check the connections and ensure the ESP8266 is powered correctly.
- **Web Interface Not Updating**: Ensure your device is connected to the same Wi-Fi network as the ESP8266.

## Conclusion

This project is a basic smart home automation system that integrates multiple sensors and actuators with the ESP8266. You can control and monitor your home environment from a simple web interface.
