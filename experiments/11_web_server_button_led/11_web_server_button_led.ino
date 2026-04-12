#include <WiFi.h>
#include <WebServer.h>

const char* WIFI_SSID = "simonisdev 🐶";
const char* WIFI_PASSWORD = "khoanvn2";

const int LED_PIN = 2;
const int BUTTON_PIN = 0; // BOOT Button

WebServer server(80);

bool ledOn = false;
int lastButtonState = HIGH;

void setLed(bool state) {
    ledOn = state;
    digitalWrite(LED_PIN, ledOn ? HIGH : LOW);

    Serial.print("LED is now ");
    Serial.println(ledOn ? "ON" : "OFF");
}

void toggleLed() {
    setLed(!ledOn);
}

void handleHome() {
  String html = "";

  html += "<!DOCTYPE html>";
  html += "<html>";
  html += "<head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<title>ESP32 LED Control</title>";
  html += "</head>";
  html += "<body style='font-family: Arial; text-align: center; margin-top: 40px;'>";
  html += "<h1>ESP32 LED Control</h1>";
  html += "<p>LED status: ";
  html += ledOn ? "ON" : "OFF";
  html += "</p>";
  html += "<p><a href='/on'><button style='font-size: 24px; padding: 12px 24px;'>ON</button></a></p>";
  html += "<p><a href='/off'><button style='font-size: 24px; padding: 12px 24px;'>OFF</button></a></p>";
  html += "<p><a href='/toggle'><button style='font-size: 24px; padding: 12px 24px;'>TOGGLE</button></a></p>";
  html += "<p>Or press the BOOT button on the ESP32.</p>";
  html += "</body>";
  html += "</html>";

  server.send(200, "text/html", html);
}

void handleLedOn() {
  setLed(true);
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleLedOff() {
  setLed(false);
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleToggle() {
  toggleLed();
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleStatus() {
  String json = "";

  json += "{";
  json += "\"led\":\"";
  json += ledOn ? "on" : "off";
  json += "\",";
  json += "\"ip\":\"";
  json += WiFi.localIP().toString();
  json += "\"";
  json += "}";

  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  setLed(false);

  Serial.println("ESP32 Web Server + Button demo started");
  Serial.print("Connecting to: ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println("Wi-Fi connected!");
  Serial.print("Open this URL: http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleHome);
  server.on("/on", handleLedOn);
  server.on("/off", handleLedOff);
  server.on("/toggle", handleToggle);
  server.on("/status", handleStatus);

  server.begin();

  Serial.println("Web server started");
}

void loop() {
    server.handleClient();

    int buttonState = digitalRead(BUTTON_PIN);

    if (lastButtonState == HIGH && buttonState == LOW) {
        Serial.println("BOOT button pressed");
        toggleLed();
        delay(200);
    }

    lastButtonState = buttonState;
}