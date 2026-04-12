#include <WiFi.h>
#include <time.h>

const char* WIFI_SSID = "simonisdev 🐶";
const char* WIFI_PASSWORD = "khoanvn2";

const long GMT_OFFSET_SECONDS = 7 * 60 * 60; // Vietnam timezone: UTC+7
const int DAYLIGHT_OFFSET_SECONDS = 0;

void printLocalTime() {
    struct tm timeInfo;

    if (!getLocalTime(&timeInfo)) {
        Serial.println("Failed to get time");
        return;
    }

    Serial.print("Current time: ");
    Serial.println(&timeInfo, "%Y-%m-%d %H:%M:%S");
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("ESP32 NTP Time Sync demo started");

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to Wi-Fi");

    while(WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }

    Serial.println();
    Serial.println("Wi-Fi Connected!");

    configTime(GMT_OFFSET_SECONDS, DAYLIGHT_OFFSET_SECONDS, "pool.ntp.org", "time.nist.gov");

    Serial.println("Waiting for NTP time...");
    printLocalTime();
}

void loop() {
    printLocalTime();
    delay(5000);
}