const int BUTTON_PIN = 0;
const int LED_PIN = 2;

const unsigned long BLINK_INTERVAL = 1000;

unsigned long previousBlinkTime = 0;
bool ledOn = false;

int lastButtonState = HIGH;

void setup() {
    Serial.begin(115200);

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);

    digitalWrite(LED_PIN, LOW);

    Serial.println("ESP32 millis non-blocking demo started");
}

void loop() {
    unsigned long currentTime = millis();

    if (currentTime - previousBlinkTime >= BLINK_INTERVAL) {
        previousBlinkTime = currentTime;

        ledOn = !ledOn;
        digitalWrite(LED_PIN, ledOn ? HIGH : LOW);

        if (ledOn) {
            Serial.println("Blink ON");
        } else {
            Serial.println("Blink OFF");
        }
    }

    int buttonState = digitalRead(BUTTON_PIN);

    if (lastButtonState == HIGH && buttonState == LOW) {
        Serial.println("BOOT pressed while blink is still running");
    }

    lastButtonState = buttonState;
}