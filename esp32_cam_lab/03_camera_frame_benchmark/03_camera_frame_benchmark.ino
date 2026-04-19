#include "esp_camera.h"

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22


constexpr framesize_t FRAME_SIZE = FRAMESIZE_QVGA; // Try QQVGA, QVGA, VGA
constexpr int JPEG_QUALITY = 12;
constexpr int REPORT_EVERY_FRAMES = 30;

unsigned long frameCount = 0;
unsigned long totalCaptureTimeMs = 0;
unsigned long totalFrameBytes = 0;
unsigned long benchmarkStartMs = 0;

bool setupCamera() {
    camera_config_t config = {};

    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;

    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    config.frame_size = FRAME_SIZE;
    config.jpeg_quality = JPEG_QUALITY;
    config.fb_count = psramFound() ? 2 : 1;
    config.fb_location = psramFound() ? CAMERA_FB_IN_PSRAM : CAMERA_FB_IN_DRAM;
    config.grab_mode = psramFound() ? CAMERA_GRAB_LATEST : CAMERA_GRAB_WHEN_EMPTY;

    esp_err_t error = esp_camera_init(&config);

    if (error != ESP_OK) {
        Serial.print("Camera init failed with error 0x");
        Serial.println(error, HEX);
        return false;
    }

    sensor_t* sensor = esp_camera_sensor_get();

    if (sensor) {
        sensor->set_framesize(sensor, FRAME_SIZE);

        Serial.print("Camera sensor PID: 0x");
        Serial.println(sensor->id.PID, HEX);
    }

    return true;
}

void printMemory() {
    Serial.print("free_heap=");
    Serial.print(ESP.getFreeHeap());

    Serial.print(" min_free_heap=");
    Serial.print(ESP.getMinFreeHeap());

    Serial.print(" psram=");
    Serial.print(psramFound() ? "yes" : "no");

    Serial.print(" free_psram=");
    Serial.println(ESP.getFreePsram());
}

void captureOneFrame() {
    unsigned long startMs = millis();

    camera_fb_t* frame = esp_camera_fb_get();

    unsigned long captureTimeMs = millis() - startMs;

    if (!frame) {
        Serial.println("Capture failed.");
        return;
    }

    frameCount++;
    totalCaptureTimeMs += captureTimeMs;
    totalFrameBytes += frame->len;

    Serial.print("frame=");
    Serial.print(frameCount);

    Serial.print(" capture_ms=");
    Serial.print(captureTimeMs);

    Serial.print(" bytes=");
    Serial.print(frame->len);

    Serial.print(" width=");
    Serial.print(frame->width);

    Serial.print(" height=");
    Serial.println(frame->height);
    
    esp_camera_fb_return(frame);

    if (frameCount % REPORT_EVERY_FRAMES == 0) {
        unsigned long elapsedMs = millis() - benchmarkStartMs;

        float fps = frameCount * 1000.0 / elapsedMs;
        float avgCaptureMs = totalCaptureTimeMs * 1.0 / frameCount;
        float avgFrameKb = totalFrameBytes / 1024.0 / frameCount;

        Serial.println();
        Serial.println("=== Benchmark Report ===");

        Serial.print("frames=");
        Serial.println(frameCount);

        Serial.print("elapsed_ms=");
        Serial.println(elapsedMs);

        Serial.print("fps=");
        Serial.println(fps, 2);

        Serial.print("avg_capture_ms=");
        Serial.println(avgCaptureMs, 2);

        Serial.print("avg_frame_kb=");
        Serial.println(avgFrameKb, 2);

        printMemory();

        Serial.println("========================");
        Serial.println();
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("ESP32-CAM Frame Benchmark started.");

    Serial.print("PSRAM found: ");
    Serial.println(psramFound() ? "yes" : "no");

    if (!setupCamera()) {
        Serial.println("Camera setup failed.");
        while (true) {
            delay(1000);
        }
    }

    Serial.println("Camera initialized.");
    Serial.println("Capturing frames locally. No Wi-Fi needed.");

    benchmarkStartMs = millis();
}

void loop() {
    captureOneFrame();
    delay(100);
}