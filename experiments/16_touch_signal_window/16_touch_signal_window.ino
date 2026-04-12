const int TOUCH_PIN = 4; // T0 on classic ESP32
const int LED_PIN = 2;

const int WINDOW_SIZE = 50;
const unsigned long SAMPLE_INTERVAL_MS = 20;

int samples[WINDOW_SIZE];
int sampleIndex = 0;
int sampleCount = 0;

unsigned long previousSampleTime = 0;

int baseline = 0;
int touchThreshold = 0;

void calibrateTouch() {
    Serial.println("Calibrating touch sensor...");
    Serial.println("Do not touch GPIO 4.");

    delay(1000);

    long total = 0;

    for (int i = 0; i < 100; i++) {
        total += touchRead(TOUCH_PIN);
        delay(10);
    }

    baseline = total / 100;
    touchThreshold = baseline * 70 / 100;

    Serial.print("Baseline: ");
    Serial.println(baseline);

    Serial.print("Touch threshold: ");
    Serial.println(touchThreshold);

    Serial.println("Now touch GPIO 4.");
}


void addSample(int value) {
    samples[sampleIndex] = value;
    sampleIndex = (sampleIndex + 1) % WINDOW_SIZE;

    if (sampleCount < WINDOW_SIZE) {
        sampleCount++;
    }
}

void analyzeWindow() {
    if (sampleCount < WINDOW_SIZE) {
        return;
    }

    unsigned long startTime = micros();

    long total = 0;
    int minValue = samples[0];
    int maxValue = samples[0];

    for (int i = 0; i < WINDOW_SIZE; i++) {
        int value = samples[i];

        total += value;

        if (value < minValue) {
            minValue = value;
        }

        if (value > maxValue) {
            maxValue = value;
        }
    }

    int average = total / WINDOW_SIZE;
    bool isTouched = average < touchThreshold;

    unsigned long analysisTime = micros() - startTime;

    digitalWrite(LED_PIN, isTouched ? HIGH : LOW);

    Serial.print("avg=");
    Serial.print(average);

    Serial.print(" min=");
    Serial.print(minValue);

    Serial.print(" max=");
    Serial.print(maxValue);

    Serial.print(" threshold=");
    Serial.print(touchThreshold);

    Serial.print(" analysis=");
    Serial.print(analysisTime);
    Serial.print(" us");

    if (isTouched) {
        Serial.println(" -> TOUCH DETECTED");
    } else {
        Serial.println(" -> no touch");
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.println("ESP32 Touch Signal Window Demo Started");

    calibrateTouch();
}

void loop() {
    unsigned long currentTime = millis();

    if (currentTime - previousSampleTime >= SAMPLE_INTERVAL_MS) {
        previousSampleTime = currentTime;

        int touchValue = touchRead(TOUCH_PIN);

        addSample(touchValue);
        analyzeWindow();
    }
}