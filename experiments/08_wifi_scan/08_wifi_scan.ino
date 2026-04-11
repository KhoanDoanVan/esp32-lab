#include <WiFi.h>

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("ESP32 Wi-Fi Scan demo started");

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    delay(100);
}

void loop() {
    Serial.println();
    Serial.println("Scanning Wi-Fi networks...");

    int networkCount = WiFi.scanNetworks();

    if (networkCount == 0) {
        Serial.println("No networks found");
    } else {
        Serial.print(networkCount);
        Serial.println(" networks found:");

        for (int i = 0; i < networkCount; i++) {
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" | RSSI: ");
            Serial.print(WiFi.RSSI(i));
            Serial.print(" dBm | ");

            if (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) {
                Serial.println("open");
            } else {
                Serial.println("secured");
            }

            delay(10);
        }
    }

    WiFi.scanDelete();

    Serial.println("Scan complete. Waiting 10 seconds...");
    delay(10000);
}