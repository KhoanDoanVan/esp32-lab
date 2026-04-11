#include <WiFi.h>

const char* WIFI_SSID = "simonisdev";
const char* WIFI_PASSWORD = "khoanvn2";

const int LED_PIN = 2;

void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.println("ESP32 WIFI connect demo started");
    Serial.print("Connecting to: ");
    Serial.println(WIFI_SSID);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    unsigned long startTime = millis();
    const unsigned long WIFI_TIMEOUT = 15000;

    while(WiFi.status() != WL_CONNECTED && millis() - startTime < WIFI_TIMEOUT) {
        Serial.print(".");
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        delay(500);
    }

    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        digitalWrite(LED_PIN, HIGH);

        Serial.println("Wi-Fi connected!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());

        Serial.print("Signal strength RSSI: ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");
    } else {
        digitalWrite(LED_PIN, LOW);

        Serial.println("WiFi connection failed.");
        Serial.println("Check SSID, password, and make sure the network is 2.4 GHz.");
    }
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Still connected.");
    } else {
        Serial.println("Disconnected.");
    }

    delay(5000);
}