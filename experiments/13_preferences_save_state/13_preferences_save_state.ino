#include <Preferences.h>

const int BUTTON_PIN = 0;
const int LED_PIN = 2;

Preferences preferences;

bool ledOn = false;
int lastButtonState = HIGH;

void setLed(bool state) {
    ledOn = state;

    digitalWrite(LED_PIN, ledOn ? HIGH : LOW);

    preferences.putBool("led_on", ledOn);

    Serial.print("LED Saved As ");
    Serial.println(ledOn ? "ON" : "OFF");
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);

    preferences.begin("lab", false);

    ledOn = preferences.getBool("led_on", false);
    digitalWrite(LED_PIN, ledOn ? HIGH : LOW);

    Serial.println("ESP32 Preferences save-state demo started");
    Serial.print("Restored LED state: ");
    Serial.println(ledOn ? "ON" : "OFF");
    Serial.println("Press BOOT to toggle and save LED state.");
}

void loop() {
    int buttonState = digitalRead(BUTTON_PIN);

    if (lastButtonState == HIGH && buttonState == LOW) {
        setLed(!ledOn);
        delay(200);
    }

    lastButtonState = buttonState;
}