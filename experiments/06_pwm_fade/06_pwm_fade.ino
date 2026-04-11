const int LED_PIN = 2;

void setup() {
    Serial.begin(115200);

    pinMode(LED_PIN, OUTPUT);

    Serial.println("ESP32 PWM fade demo started");
}

void loop() {
    for (int brightness = 0; brightness <= 255; brightness++) {
        analogWrite(LED_PIN, brightness);
        Serial.println(brightness);
        delay(10);
    }

    for (int brightness = 255; brightness >= 0; brightness--) {
        analogWrite(LED_PIN, brightness);
        Serial.println(brightness);
        delay(10);
    }
}