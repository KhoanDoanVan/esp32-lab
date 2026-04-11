const int BUTTON_PIN = 0;  // BOOT button
const int LED_PIN = 2;     // Onboard LED on many ESP32 DevKit V1 boards

int lastButtonState = HIGH;
bool ledOn = false;

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);

  Serial.println("ESP32 Button Toggle started");
}

void loop() {
  int buttonState = digitalRead(BUTTON_PIN);

  if (lastButtonState == HIGH && buttonState == LOW) {
    ledOn = !ledOn;

    digitalWrite(LED_PIN, ledOn ? HIGH : LOW);

    if (ledOn) {
      Serial.println("LED toggled ON");
    } else {
      Serial.println("LED toggled OFF");
    }

    delay(200);
  }

  lastButtonState = buttonState;
}
