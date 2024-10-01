#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

// WiFi credentials
const char* ssid = "akshaya";
const char* password = "shay369x";

// Pin definitions
#define TRIG_PIN D1
#define ECHO_PIN D2
#define LDR_PIN A0
#define DHT_PIN D4
#define LIGHT_RELAY_PIN D5
#define FAN_RELAY_PIN D6

// DHT sensor type
#define DHT_TYPE DHT11

// Create DHT sensor instance
DHT dht(DHT_PIN, DHT_TYPE);

// Create web server instance on port 80
ESP8266WebServer server(80);

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP()); // This line prints the IP address


  // Initialize sensor and relay pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LIGHT_RELAY_PIN, OUTPUT);
  pinMode(FAN_RELAY_PIN, OUTPUT);

  // Initialize DHT sensor
  dht.begin();

  // Set initial states of relays
  digitalWrite(LIGHT_RELAY_PIN, HIGH); // Relay is off
  digitalWrite(FAN_RELAY_PIN, HIGH);   // Relay is off

  // Define web server routes
  server.on("/", handleRoot);
  server.on("/light/on", handleLightOn);
  server.on("/light/off", handleLightOff);
  server.on("/fan/on", handleFanOn);
  server.on("/fan/off", handleFanOff);

  // Start web server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Handle web server
  server.handleClient();

  // Automatic mode functionality
  checkEnvironment();
}

// Function to handle root web page
void handleRoot() {
  String html = "<h1>Home Automation</h1><p><a href=\"/light/on\">Light ON</a></p><p><a href=\"/light/off\">Light OFF</a></p><p><a href=\"/fan/on\">Fan ON</a></p><p><a href=\"/fan/off\">Fan OFF</a></p>";
  server.send(200, "text/html", html);
}

// Function to turn on the light
void handleLightOn() {
  digitalWrite(LIGHT_RELAY_PIN, LOW); // Active LOW
  server.send(200, "text/html", "<h1>Light ON</h1><p><a href=\"/\">Back</a></p>");
}

// Function to turn off the light
void handleLightOff() {
  digitalWrite(LIGHT_RELAY_PIN, HIGH);
  server.send(200, "text/html", "<h1>Light OFF</h1><p><a href=\"/\">Back</a></p>");
}

// Function to turn on the fan
void handleFanOn() {
  digitalWrite(FAN_RELAY_PIN, LOW); // Active LOW
  server.send(200, "text/html", "<h1>Fan ON</h1><p><a href=\"/\">Back</a></p>");
}

// Function to turn off the fan
void handleFanOff() {
  digitalWrite(FAN_RELAY_PIN, HIGH);
  server.send(200, "text/html", "<h1>Fan OFF</h1><p><a href=\"/\">Back</a></p>");
}

// Function to check the environment and control devices automatically
void checkEnvironment() {
  // Measure distance using HC-SR04
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  long distance = (duration / 2) / 29.1;

  // Measure light level using LDR
  int lightLevel = analogRead(LDR_PIN);

  // Measure temperature using DHT11
  float temperature = dht.readTemperature();

  // Check if person is detected
  if (distance < 100) { // Assuming person is detected within 100 cm
    if (lightLevel < 500) { // Assuming low light level threshold
      digitalWrite(LIGHT_RELAY_PIN, LOW); // Turn on light
    } else {
      digitalWrite(LIGHT_RELAY_PIN, HIGH); // Turn off light
    }

    if (temperature > 20) {
      digitalWrite(FAN_RELAY_PIN, LOW); // Turn on fan
    } else {
      digitalWrite(FAN_RELAY_PIN, HIGH); // Turn off fan
    }
  } else {
    digitalWrite(LIGHT_RELAY_PIN, HIGH); // Turn off light
    digitalWrite(FAN_RELAY_PIN, HIGH);   // Turn off fan
  }
}
