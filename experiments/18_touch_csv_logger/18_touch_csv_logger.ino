#include <TFLIteMicro.h>
#include <utility.h>

const int TOUCH_PIN = 4;
const int LED_PIN = 2;

const int WINDOW_SIZE = 20;
const unsigned long SAMPLE_INTERVAL_MS = 50;

int samples[WINDOW_SIZE];
int sampleIndex = 0;
int sampleCount = 0;

unsigned long previousSampleTime = 0;

int baseline = 0;
int touchThreshold = 0;

void calibrateTouch() {
    long total = 0;

    delay(1000);

    for (int i = 0; i < 100; i++) {
        total += touchRead(TOUCH_PIN);
        delay(10);
    }

    baseline = total / 100;
    touchThreshold = baseline * 70 / 100;
}

void addSample(int value) {
    samples[sampleIndex] = value;
    sampleIndex = (sampleIndex + 1) % WINDOW_SIZE;

    if (sampleCount < WINDOW_SIZE) {
        sampleCount++;
    }
}

void calculateFeatures(int* averageValue, int* minValue, int* maxValue) {
    long total = 0;

    *minValue = samples[0];
    *maxValue = samples[0];

    for (int i = 0; i < sampleCount; i++) {
        int value = samples[i];

        total += value;

        if (value < *minValue) {
            *minValue = value;
        }

        if (value > *maxValue) {
            *maxValue = value;
        }
    }

    *averageValue = total / sampleCount;
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    calibrateTouch();

    Serial.println("time_ms,raw,avg,min,max,threshold,label");
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - previousSampleTime >= SAMPLE_INTERVAL_MS) {
    previousSampleTime = currentTime;

    int rawValue = touchRead(TOUCH_PIN);

    addSample(rawValue);

    if (sampleCount < WINDOW_SIZE) {
      return;
    }

    int averageValue = 0;
    int minValue = 0;
    int maxValue = 0;

    calculateFeatures(&averageValue, &minValue, &maxValue);

    int label = averageValue < touchThreshold ? 1 : 0;

    digitalWrite(LED_PIN, label == 1 ? HIGH : LOW);

    Serial.print(currentTime);
    Serial.print(",");
    Serial.print(rawValue);
    Serial.print(",");
    Serial.print(averageValue);
    Serial.print(",");
    Serial.print(minValue);
    Serial.print(",");
    Serial.print(maxValue);
    Serial.print(",");
    Serial.print(touchThreshold);
    Serial.print(",");
    Serial.println(label);
  }
}