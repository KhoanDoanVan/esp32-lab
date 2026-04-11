#include <WiFi.h>
#include <WebServer.h>

const char* WIFI_SSID = "simonisdev";
const char* WIFI_PASSWORD = "khoanvn2";

const int LED_PIN = 2;

WebServer server(80);

bool ledOn = false;

void handleHome() {
    String html = "";

    html += "<!DOCTYPE html>";
    html += "<html>";
    html += "<head>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "<title>ESP32 LED Control</title>";
    html += "</head>";
    html += "<body style='font-family: Arial; text-align: center; margin-top: 40px;'>";
    html += "<h1>ESP32 Web Server</h1>";
    html += "<p>LED status: ";
    html += ledOn ? "ON" : "OFF";
    html += "</p>";
    html += "<p><a href='/on'><button style='font-size: 24px; padding: 12px 24px;'>Turn ON</button></a></p>";
    html += "<p><a href='/off'><button style='font-size: 24px; padding: 12px 24px;'>Turn OFF</button></a></p>";
    html += "</body>";
    html += "</html>";

    server.send(200, "text/html", html);
}

void handleLedOn() {
    ledOn = true;
    digitalWrite(LED_PIN, HIGH);

    Serial.println("LED turned ON from browser");

    server.sendHeader("Location", "/");
    server.send(303);
}

void handleLedOff() {
    ledOn = false;
    digitalWrite(LED_PIN, LOW);

    Serial.println("LED turned OFF from browser");

    server.sendHeader("Location", "/");
    server.send(303);
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.println("ESP32 Web Server LED demo started");
    Serial.print("Connecting to: ");
    Serial.println(WIFI_SSID);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while(WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }

    Serial.println();
    Serial.println("Wi-Fi connected!");
    Serial.print("Open this URL in your browser: http://");
    Serial.println(WiFi.localIP());

    server.on("/", handleHome);
    server.on("/on", handleLedOn);
    server.on("/off", handleLedOff);

    server.begin();

    Serial.println("Web Server Started!");
}

void loop() {
  server.handleClient();
}