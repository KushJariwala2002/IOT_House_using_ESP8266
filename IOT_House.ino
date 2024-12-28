#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT_U.h>
#include <DHT.h>
#include <Servo.h>

// Wi-Fi credentials
const char* ssid = "VictorMADMAX";  // Replace with your Wi-Fi SSID
const char* password = "123456789";           // Replace with your Wi-Fi password

// Pins for sensors and actuators
#define LDRPIN D0        
#define GATEPIN D1      
#define FANPIN D2        // Fan connected to relay K2 (D2)
#define PIRPIN D3        // PIR sensor connected to D3
#define IRPIN D4         // IR sensor connected to D4
#define WATERPUMP D5     // Water Pump connected to D5 (Relay module)
#define MQ2PIN D6        // MQ2 gas sensor connected to D6
#define SOILPIN A0       // Soil Moisture sensor connected to A0
#define DHTPIN D7        // DHT11 sensor connected to D7

DHT_Unified dht(DHTPIN, DHT11);  // DHT11 sensor object

// Define relays for actuators
int relayPin = D5;   // Relay for water pump
Servo gateServo;
int gatePos = 0; // Initial position of the gate (closed)

ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);

  // Initialize pins
  pinMode(LDRPIN, INPUT);
  pinMode(GATEPIN, OUTPUT);
  pinMode(FANPIN, OUTPUT);    // Fan relay pin initialized
  pinMode(PIRPIN, INPUT);
  pinMode(IRPIN, INPUT);
  pinMode(MQ2PIN, INPUT);
  pinMode(SOILPIN, INPUT);
  pinMode(DHTPIN, INPUT);

  // Initialize DHT sensor
  dht.begin();

  // Attach servo to pin
  gateServo.attach(GATEPIN); // Attach the servo to the specified pin
  gateServo.write(gatePos);  // Set gate to closed initially

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Set up web server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/style.css", HTTP_GET, handleCSS);
  server.on("/script.js", HTTP_GET, handleJS);
  server.on("/getStatus", HTTP_GET, handleGetStatus);
  server.on("/toggleRelay", HTTP_GET, handleToggleRelay);
  server.on("/toggleFan", HTTP_GET, handleToggleFan);
  server.on("/toggleGate", HTTP_GET, handleToggleGate);

  server.begin();
  Serial.println("HTTP server started.");
}

void loop() {
  server.handleClient();
}
// HTML content
const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Smart IoT Dashboard</title>
    <link rel="stylesheet" href="style.css">
</head>
<body>
    <div class="container">
        <h1 class="title">Smart IoT Dashboard</h1>
        <p class="subtitle">Monitor and control your smart devices effortlessly</p>

        <!-- Sensor Data Cards -->
        <div class="status-sensors">
            <div class="card" id="temp-card">
                <p class="label">Temperature:</p>
                <p class="value" id="temperature">27.4°C</p>
            </div>
            <div class="card" id="humidity-card">
                <p class="label">Humidity:</p>
                <p class="value" id="humidity">47.3%</p>
            </div>
            <div class="card" id="light-card">
                <p class="label">Light:</p>
                <p class="value" id="light">Day</p>
            </div>
            <div class="card" id="motion-card">
                <p class="label">Motion:</p>
                <p class="value" id="motion">No Motion</p>
            </div>
            <div class="card" id="soil-card">
                <p class="label">Soil Moisture:</p>
                <p class="value" id="soil-moisture">--%</p>
            </div>
            <div class="card" id="gas-card">
                <p class="label">Gas:</p>
                <p class="value" id="gas">Inactive</p>
            </div>
        </div>

        <!-- Control Toggles -->
        <div class="controls">
            <label class="switch">
                <span>Water Pump</span>
                <input type="checkbox" onchange="toggleWaterPump()">
                <span class="slider"></span>
            </label>
            <label class="switch">
                <span>Fan</span>
                <input type="checkbox" onchange="toggleFan()">
                <span class="slider"></span>
            </label>
            <label class="switch">
                <span>Gate</span>
                <input type="checkbox" onchange="toggleGate()">
                <span class="slider"></span>
            </label>
        </div>
    </div>

    <div class="footer">
        <p>IoT Dashboard created by <strong>8th Grade and Kush Jariwala<strong></p>
    </div>
    <script src="script.js"></script>
</body>
</html>

)rawliteral";

// CSS content
const char CSS_CONTENT[] PROGMEM = R"rawliteral(
/* General Body Styling */
body {
    font-family: 'Arial', sans-serif;
    background-color: #ffffff; /* White background */
    color: #333333; /* Dark text color */
    margin: 0;
    padding: 0;
    height: 100vh;
    display: flex;
    flex-direction: column;
    justify-content: space-between;
    text-align: center;
    overflow: hidden;
}

/* Container and Layout */
.container {
    flex: 1;
    padding: 20px;
    display: flex;
    flex-direction: column;
    justify-content: flex-start;
    align-items: center;
    overflow: hidden;
    height: 100%;
}

/* Title and Subtitle */
.title {
    font-size: 2rem; /* Simple, readable font size */
    font-weight: 600; /* Slightly bold for emphasis */
    color: #004c97; /* Dark blue color */
    margin-bottom: 15px;
    text-transform: uppercase; /* Uppercase for emphasis */
}

.subtitle {
    font-size: 1rem; /* Slightly smaller subtitle */
    color: #666666; /* Light gray */
    margin-bottom: 20px;
    font-style: italic;
}

/* Grid Layout for Sensor Cards */
.status-sensors {
    display: flex;
    flex-wrap: wrap;
    justify-content: center;
    gap: 15px;
    margin-top: 20px;
    width: 100%;
    max-width: 900px;
}

.card {
    background-color: #ffffff; /* White card background */
    border-radius: 8px;
    padding: 15px; /* Adequate padding */
    color: #004c97; /* Dark blue text */
    box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1); /* Soft shadow */
    transition: transform 0.2s ease, box-shadow 0.2s ease;
    border: 2px solid #00c853; /* Green border */
    width: 160px; /* Card width */
    height: 130px; /* Card height */
    display: flex;
    flex-direction: column;
    justify-content: space-between;
    align-items: center;
    text-align: center;
}

.card:hover {
    transform: translateY(-5px); /* Hover effect */
    box-shadow: 0 6px 15px rgba(0, 0, 0, 0.15); /* Slightly stronger shadow on hover */
}

.card .label {
    font-size: 0.9rem; /* Smaller label font size */
    font-weight: 600;
    color: #004c97; /* Dark blue label color */
}

.card .value {
    font-size: 1rem; /* Smaller value font size */
    margin-top: 5px;
    font-weight: 500;
    color: #00c853; /* Green value text */
}

/* Control Buttons */
.controls {
    display: flex;
    justify-content: space-evenly;
    margin-top: 20px;
    gap: 20px;
    width: 100%;
    max-width: 900px;
}

.switch {
    display: flex;
    align-items: center;
    justify-content: center;
    font-size: 1rem; /* Smaller font size */
    color: #004c97; /* Dark blue */
    cursor: pointer;
}

.switch span {
    margin-right: 10px;
}

.switch input {
    display: none;
}

.slider {
    width: 40px; /* Smaller width */
    height: 22px; /* Smaller height */
    background-color: #e0e0e0; /* Light gray background */
    border-radius: 30px;
    position: relative;
    transition: background-color 0.3s ease;
}

.slider:before {
    content: '';
    position: absolute;
    top: 3px;
    left: 3px;
    width: 16px; /* Smaller circle */
    height: 16px; /* Smaller circle */
    background-color: #ffffff;
    border-radius: 50%;
    transition: transform 0.3s ease;
}

.switch input:checked + .slider {
    background-color: #00c853; /* Green when toggled on */
}

.switch input:checked + .slider:before {
    transform: translateX(18px); /* Smaller translation */
}

/* Footer */
.footer {
    background-color: #004c97; /* Dark blue background */
    font-size: 0.9rem; /* Smaller font size */
    color: #ffffff; /* White text */
    padding: 10px;
    margin-top: 30px;
    font-weight: 400;
    text-align: center;
}

/* Make the layout responsive */
@media (max-width: 768px) {
    .status-sensors {
        justify-content: center; /* Center the content on smaller screens */
    }
    .card {
        width: 120px; /* Smaller width for smaller screens */
        height: 100px; /* Adjust height */
    }
}

@media (max-width: 480px) {
    .status-sensors {
        justify-content: center; /* Center content */
    }
    .card {
        width: 100px; /* Smallest width for very small screens */
        height: 90px; /* Adjust height */
    }

    .title {
        font-size: 1.5rem; /* Smaller title */
    }

    .subtitle {
        font-size: 0.8rem; /* Smaller subtitle */
    }
}

)rawliteral";

// JavaScript content
const char JS_CONTENT[] PROGMEM = R"rawliteral(
// Function to toggle the water pump (or relay) on and off
function toggleWaterPump() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', '/toggleRelay', true);  // Request to toggle the relay
    xhr.send();
}

// Function to toggle the fan on and off
function toggleFan() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', '/toggleFan', true);  // Request to toggle the fan
    xhr.send();
}

// Function to toggle the gate (servo motor) open or close
function toggleGate() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', '/toggleGate', true);  // Request to toggle the gate
    xhr.send();
}

// Function to set the motor speed (for the fan or pump)
function setMotorSpeed(value) {
    document.getElementById('motorSpeedValue').innerText = value;  // Update the displayed speed
    var xhr = new XMLHttpRequest();
    xhr.open('GET', '/setMotorSpeed?speed=' + value, true);  // Send the new speed to the server
    xhr.send();
}

// Function to fetch the current status of all sensors and update the dashboard
setInterval(function() {
    fetch('/getStatus')
    .then(response => response.json())  // Get the response as JSON
    .then(data => {
        // Update the temperature, humidity, and other sensor data
        document.getElementById('temperature').innerText = data.temperature + " °C";
        document.getElementById('humidity').innerText = data.humidity + " %";
        document.getElementById('light').innerText = data.light;
        document.getElementById('motion').innerText = data.motion;
        document.getElementById('soil-moisture').innerText = data.soil + " %";
        document.getElementById('gas').innerText = data.gas;
        document.getElementById('flame').innerText = data.flame;
    })
    .catch(error => {
        console.error("Error fetching data: ", error);
    });
}, 1000);  // Update every second
)rawliteral";

// Handle root endpoint
void handleRoot() {
  server.send_P(200, "text/html", HTML_PAGE);
}

// Serve CSS file
void handleCSS() {
  server.send_P(200, "text/css", CSS_CONTENT);
}

// Serve JavaScript file
void handleJS() {
  server.send_P(200, "application/javascript", JS_CONTENT);
}

// Handle get status request
void handleGetStatus() {
  sensors_event_t event;

  // Get temperature
  dht.temperature().getEvent(&event);
  float temperature = (event.temperature != NAN) ? event.temperature : 0.0;

  // Get humidity
  dht.humidity().getEvent(&event);
  float humidity = (event.relative_humidity != NAN) ? event.relative_humidity : 0.0;

  int lightLevel = digitalRead(LDRPIN);
  String lightStatus = (lightLevel == HIGH) ? "Night" : "Day";

  String irStatus = (digitalRead(PIRPIN) == HIGH) ? "Motion Detected" : "No Motion";
  int soilMoisture = analogRead(SOILPIN);
  String gasStatus = (digitalRead(MQ2PIN) == HIGH) ? "Smoke Detected" : "No Smoke";
  String flameStatus = "No Flame"; // Default

  String jsonResponse = "{\"temperature\": " + String(temperature) +
                        ", \"humidity\": " + String(humidity) +
                        ", \"light\": \"" + lightStatus +
                        "\", \"motion\": \"" + irStatus +
                        "\", \"soil\": " + String(soilMoisture) +
                        ", \"gas\": \"" + gasStatus +
                        "\", \"flame\": \"" + flameStatus + "\"}";

  server.send(200, "application/json", jsonResponse);
}

// Handle toggle relay request (Water Pump)
void handleToggleRelay() {
  digitalWrite(WATERPUMP, !digitalRead(WATERPUMP));  // Toggle water pump state
  server.send(200, "text/plain", "Toggled water pump relay");
}

// Handle toggle fan request (Relay K2)
void handleToggleFan() {
  digitalWrite(FANPIN, !digitalRead(FANPIN));  // Toggle fan relay state (connected to K2)
  server.send(200, "text/plain", "Toggled fan relay");
}

// Handle toggle gate (servo motor) request
void handleToggleGate() {
  gatePos = (gatePos == 0) ? 180 : 0;  // Toggle between 0 and 180 degrees
  gateServo.write(gatePos);  // Move the gate servo
  server.send(200, "text/plain", "Toggled gate position");
}
