const int TOUCH_PIN = 4; // T0 on classic ESP32
const int LED_PIN = 2;

int untouchedValue = 0;
int touchThreshold = 0;


void setup() {
    Serial.begin(115200);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.println("ESP32 Touch Input started");
    Serial.println("Do not touch GPIO 4 durning calibration...");

    delay(1000);

    long total = 0;

    for (int i = 0; i < 50; i++) {
        total += touchRead(TOUCH_PIN);
        delay(20);
    }

    untouchedValue = total / 50;
    touchThreshold = untouchedValue * 70 / 100;

    Serial.print("Untouched value: ");
    Serial.println(untouchedValue);

    Serial.print("Touch threshold: ");
    Serial.println(touchThreshold);

    Serial.println("Now touch GPIO 4.");
}

void loop() {
    int touchValue = touchRead(TOUCH_PIN);
    bool isTouched = touchValue < touchThreshold;

    digitalWrite(LED_PIN, isTouched ? HIGH : LOW);

    Serial.print("touchValue = ");
    Serial.print(touchValue);

    if (isTouched) {
        Serial.println("-> touched");
    } else {
        Serial.println("-> not touched");
    }

    delay(100);
}