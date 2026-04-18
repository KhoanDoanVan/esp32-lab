#include "esp_camera.h"
#include <WebServer.h>
#include <WiFi.h>

const char* WIFI_SSID = "simonisdev 🐶";
const char* WIFI_PASSWORD = "khoanvn2";

// AI Thinker ESP32-CAM pin map
#define PWDN_GPIO_NUM       32
#define RESET_GPIO_NUM      -1
#define XCLK_GPIO_NUM       0
#define SIOD_GPIO_NUM       26
#define SIOC_GPIO_NUM       27

#define Y9_GPIO_NUM         35
#define Y8_GPIO_NUM         34
#define Y7_GPIO_NUM         39
#define Y6_GPIO_NUM         36
#define Y5_GPIO_NUM         21
#define Y4_GPIO_NUM         19
#define Y3_GPIO_NUM         18
#define Y2_GPIO_NUM         5
#define VSYNC_GPIO_NUM      25
#define HREF_GPIO_NUM       23
#define PCLK_GPIO_NUM       22

WebServer server(80);

constexpr framesize_t STREAM_FRAME_SIZE = FRAMESIZE_QVGA;
constexpr int JPEG_QUALITY = 12;
constexpr int STREAM_DELAY_MS = 40;

unsigned long framesServed = 0;
unsigned long lastFrameBytes = 0;
unsigned long lastCaptureTimeMs = 0;

void handleRoot() {
    String html = "";

    html += "<!DOCTYPE html>";
    html += "<html>";
    html += "<head>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "<title>ESP32-CAM Live Stream</title>";
    html += "</head>";
    html += "<body style='font-family: Arial; text-align: center; margin: 24px;'>";
    html += "<h1>ESP32-CAM Live Stream</h1>";
    html += "<p>MJPEG stream from the camera module.</p>";
    html += "<img src='/stream' style='width: 100%; max-width: 640px; border: 1px solid #ccc;'>";
    html += "<p><a href='/capture'>Open one JPEG snapshot</a></p>";
    html += "<p><a href='/status'>Open JSON status</a></p>";
    html += "</body>";
    html += "</html>";

    server.send(200, "text/html", html);
}

void handleCapture() {
    unsigned long startTime = millis();

    camera_fb_t* frame = esp_camera_fb_get();

    if (!frame) {
        server.send(500, "text/plain", "Camera capture failed");
        return;
    }

    lastFrameBytes = frame->len;
    lastCaptureTimeMs = millis() - startTime;

    WiFiClient client = server.client();

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Cache-Control", "no-store");
    server.setContentLength(frame->len);
    server.send(200, "image/jpeg", "");

    client.write(frame->buf, frame->len);

    esp_camera_fb_return(frame);
}

void handleStream() {
    WiFiClient client = server.client();

    client.print("HTTP/1.1 200 OK\r\n");
    client.print("Content-Type: multipart/x-mixed-replace; boundary=frame\r\n");
    client.print("Cache-Control: no-cache\r\n");
    client.print("Connection: close\r\n");
    client.print("\r\n");

    Serial.println("Stream Client Connected.");

    while (client.connected()) {
        unsigned long startTime = millis();

        camera_fb_t* frame = esp_camera_fb_get();

        if (!frame) {
            Serial.println("Camera capture failed during stream.");
            break;
        }

        lastFrameBytes = frame->len;
        lastCaptureTimeMs = millis() - startTime;
        framesServed++;

        client.print("--frame\r\n");
        client.print("Content-Type: image/jpeg\r\n");
        client.print("Content-Length: ");
        client.print(frame->len);
        client.print("\r\n\r\n");

        client.write(frame->buf, frame->len);
        client.print("\r\n");

        esp_camera_fb_return(frame);

        if (!client.connected()) {
            break;
        }

        delay(STREAM_DELAY_MS);
    }

    Serial.println("Stream client disconnected.");
}

void handleStatus() {
    String json = "";

    json += "{";
    json += "\"chip\":\"ESP32-CAM\",";
    json += "\"psram\":";
    json += psramFound() ? "true" : "false";
    json += ",";
    json += "\"free_heap\":";
    json += ESP.getFreeHeap();
    json += ",";
    json += "\"frames_served\":";
    json += framesServed;
    json += ",";
    json += "\"last_frame_bytes\":";
    json += lastFrameBytes;
    json += ",";
    json += "\"last_capture_ms\":";
    json += lastCaptureTimeMs;
    json += ",";
    json += "\"ip\":\"";
    json += WiFi.localIP().toString();
    json += "\"";
    json += "}";

    server.send(200, "application/json", json);
}

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

    if (psramFound()) {
        config.frame_size = STREAM_FRAME_SIZE;
        config.jpeg_quality = JPEG_QUALITY;
        config.fb_count = 2;
        config.fb_location = CAMERA_FB_IN_PSRAM;
        config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
        config.frame_size = FRAMESIZE_QQVGA;
        config.jpeg_quality = 20;
        config.fb_count = 1;
        config.fb_location = CAMERA_FB_IN_DRAM;
        config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    }

    esp_err_t error = esp_camera_init(&config);

    if (error != ESP_OK) {
        Serial.print("Camera init failed with error 0x");
        Serial.println(error, HEX);
        return false;
    }

    sensor_t* sensor = esp_camera_sensor_get();

    if (sensor) {
        sensor->set_framesize(sensor, psramFound() ? STREAM_FRAME_SIZE : FRAMESIZE_QQVGA);

        Serial.print("Camera sensor PID: 0x");
        Serial.println(sensor->id.PID, HEX);
    }

    return true;
}

void connectWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to Wi-Fi");

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }

    Serial.println();
    Serial.println("Wi-Fi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}


void setupServer() {
    server.on("/", handleRoot);
    server.on("/stream", handleStream);
    server.on("/capture", handleCapture);
    server.on("/status", handleStatus);

    server.begin();

    Serial.println("HTTP server started.");
    Serial.print("Open: http://");
    Serial.println(WiFi.localIP());

    Serial.print("Stream: http://");
    Serial.print(WiFi.localIP());
    Serial.println("/stream");

    Serial.print("Capture: http://");
    Serial.print(WiFi.localIP());
    Serial.println("/capture");

    Serial.print("Status: http://");
    Serial.print(WiFi.localIP());
    Serial.println("/status");
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("ESP32-CAM MJPEG Live Stream started.");

    Serial.print("PSRAM found: ");
    Serial.print(psramFound() ? "yes" : "no");

    if (!setupCamera()) {
        Serial.print("Camera setup failed");
        while (true) {
            delay(1000);
        }
    }

    Serial.println("Camera initialized.");

    connectWiFi();
    setupServer();
}

void loop() {
    server.handleClient();
}