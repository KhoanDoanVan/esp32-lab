const int TOUCH_PIN = 4;  // T0 on classic ESP32
const int LED_PIN = 2;

const int WINDOW_SIZE = 20;
const unsigned long SAMPLE_INTERVAL_MS = 50;

int samples[WINDOW_SIZE];
int sampleIndex = 0;
int sampleCount = 0;
long sampleTotal = 0;

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
  if (sampleCount < WINDOW_SIZE) {
    samples[sampleIndex] = value;
    sampleTotal += value;
    sampleCount++;
  } else {
    sampleTotal -= samples[sampleIndex];
    samples[sampleIndex] = value;
    sampleTotal += value;
  }

  sampleIndex = (sampleIndex + 1) % WINDOW_SIZE;
}

int getAverage() {
  if (sampleCount == 0) {
    return 0;
  }

  return sampleTotal / sampleCount;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.println("ESP32 Touch Serial Plotter demo started");
  Serial.println("Do not touch GPIO 4 during calibration.");

  calibrateTouch();

  Serial.print("baseline=");
  Serial.println(baseline);

  Serial.print("threshold=");
  Serial.println(touchThreshold);

  delay(1000);
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - previousSampleTime >= SAMPLE_INTERVAL_MS) {
    previousSampleTime = currentTime;

    int rawValue = touchRead(TOUCH_PIN);
    addSample(rawValue);

    int averageValue = getAverage();
    bool isTouched = averageValue < touchThreshold;

    digitalWrite(LED_PIN, isTouched ? HIGH : LOW);

    Serial.print("raw:");
    Serial.print(rawValue);

    Serial.print("\tavg:");
    Serial.print(averageValue);

    Serial.print("\tthreshold:");
    Serial.print(touchThreshold);

    Serial.print("\ttouched:");
    Serial.println(isTouched ? 100 : 0);
  }
}
