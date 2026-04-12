const int LED_PIN = 2;

unsigned long loopCount = 0;
unsigned long previousReportTime = 0;

void fakeWork() {
    long total = 0;

    for (int i = 0; i < 10000; i++) {
        total += 1;
    }

    if (total == -1) {
        Serial.println("This will never happen");
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(LED_PIN, OUTPUT);

    Serial.println("ESP32 runtime timer demo started");
}

void loop() {
    unsigned long startTime = micros();

    fakeWork();

    unsigned long endTime = micros();
    unsigned long elapsedTime = endTime - startTime;

    loopCount++;

    if (millis() - previousReportTime >= 1000) {
        previousReportTime = millis();

        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        
        Serial.print("fakeWork time: ");
        Serial.print(elapsedTime);
        Serial.println(" us");

        Serial.print("loop count: ");
        Serial.print(loopCount);
        Serial.println(" loops since boot");

        Serial.println();
    }
}


// Future will use this code for measure inferences time 
// unsigned long startTime = micros();

// interpreter->Invoke();

// unsigned long inferenceTime = micros() - startTime;

// Serial.print("Inference time: ");
// Serial.print(inferenceTime);
// Serial.println(" us");