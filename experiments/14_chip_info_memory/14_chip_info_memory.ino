void printChipInfo() {
    Serial.println("=== Chip Info ===");

    Serial.print("Chip model: ");
    Serial.println(ESP.getChipModel());

    Serial.print("Chip revision: ");
    Serial.println(ESP.getChipRevision());

    Serial.print("CPU frequency: ");
    Serial.print(ESP.getCpuFreqMHz());
    Serial.println(" MHz");

    Serial.print("Flash size: ");
    Serial.print(ESP.getFlashChipSize() / 1024 / 1024);
    Serial.println(" MB");

    Serial.print("Sketch size: ");
    Serial.print(ESP.getSketchSize());
    Serial.println(" bytes");

    Serial.print("Free sketch space: ");
    Serial.print(ESP.getFreeSketchSpace());
    Serial.println(" bytes");
}

void printMemoryInfo() {
    Serial.println("=== Memory Info ===");

    Serial.print("Free heap: ");
    Serial.print(ESP.getFreeHeap());
    Serial.println(" bytes");

    Serial.print("Minimum free heap: ");
    Serial.print(ESP.getMinFreeHeap());
    Serial.println(" bytes");

    Serial.print("Max alloc heap: ");
    Serial.print(ESP.getMaxAllocHeap());
    Serial.println(" bytes");

    Serial.print("PSRAM size: ");
    Serial.print(ESP.getPsramSize());
    Serial.println(" bytes");

    Serial.print("Free PSRAM: ");
    Serial.print(ESP.getFreePsram());
    Serial.println(" bytes");
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("ESP32 Chip Info and Memory demo started");

    printChipInfo();
    printMemoryInfo();
}

void loop() {
    Serial.println();
    Serial.println("Memory snapshot: ");

    Serial.print("Free heap: ");
    Serial.print(ESP.getFreeHeap());
    Serial.println(" bytes");

    Serial.print("Minimum free heap: ");
    Serial.print(ESP.getMinFreeHeap());
    Serial.println(" bytes");

    printChipInfo();
    printMemoryInfo();

    delay(5000);
}