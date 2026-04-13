const int TOUCH_PIN = 4;
const int LED_PIN = 2;

const int WINDOW_SIZE = 50;
const unsigned long SAMPLE_INTERVAL_MS = 20;

int samples[WINDOW_SIZE];
int sampleIndex = 0;
int sampleCount = 0;

unsigned long previousSampleTime = 0;

int baseline = 0;
int threshold = 0;

void calibrateSensor() {
    long total = 0;

    Serial.println("Calibrating sensor. Do not touch GPIO 4.");
    delay(1000);

    for (int i = 0; i < 100; i++) {
        total += touchRead(TOUCH_PIN);
        delay(10);
    }

    baseline = total / 100;
    threshold = baseline * 70 / 100;

    Serial.print("baseline=");
    Serial.println(baseline);

    Serial.print("threshold=");
    Serial.println(threshold);
}

void addSample(int value) {
    samples[sampleIndex] = value;
    sampleIndex = (sampleIndex + 1) % WINDOW_SIZE;

    if (sampleCount < WINDOW_SIZE) {
        sampleCount++;
    }
}

void extractFeatures(int* averageValue, int* minValue, int* maxValue) {
    long total = 0;

    *minValue = samples[0];
    *maxValue = samples[0];

    for (int i = 0; i < WINDOW_SIZE; i++) {
        int value = samples[i];

        total += value;

        if (value < *minValue) {
            *minValue = value;
        }

        if (value > *maxValue) {
            *maxValue = value;
        }
    }

    *averageValue = total / WINDOW_SIZE;
}

float fakeModelPredictTouch(int averageValue) {
    if (averageValue >= baseline) {
        return 0.0;
    }

    if (averageValue <= threshold) {
        return 1.0;
    }

    return float(baseline - averageValue) / float(baseline - threshold);
}

void runInference() {
    if (sampleCount < WINDOW_SIZE) {
        return;
    }

    unsigned long startTime = micros();

    int averageValue = 0;
    int minValue = 0;
    int maxValue = 0;

    extractFeatures(&averageValue, &minValue, &maxValue);

    float confidence = fakeModelPredictTouch(averageValue);
    bool predictedTouch = confidence >= 0.80;

    unsigned long inferenceTime = micros() - startTime;

    digitalWrite(LED_PIN, predictedTouch ? HIGH : LOW);

    Serial.print("avg=");
    Serial.print(averageValue);

    Serial.print(" min=");
    Serial.print(minValue);

    Serial.print(" max=");
    Serial.print(maxValue);

    Serial.print(" confidence=");
    Serial.print(confidence, 2);

    Serial.print(" label=");
    Serial.print(predictedTouch ? "touch" : "not_touch");

    Serial.print(" inference_time=");
    Serial.print(inferenceTime);
    Serial.println("us");
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.println("ESP32 TinyML Inference Simulator started");

    calibrateSensor();
}

void loop() {
    unsigned long currentTime = millis();

    if (currentTime - previousSampleTime >= SAMPLE_INTERVAL_MS) {
        previousSampleTime = currentTime;

        int rawValue = touchRead(TOUCH_PIN);

        addSample(rawValue);
        runInference();
    }
}