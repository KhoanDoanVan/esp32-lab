# 01 - ESP32-CAM Camera HTTP Snapshot

This is the first ESP32-CAM lab. It tests whether the camera module works by starting a small HTTP server and serving a live-updating JPEG snapshot page.

Target board assumption:

```text
AI Thinker ESP32-CAM
OV2640 camera
```

## What This Test Does

```text
initialize OV2640 camera
connect ESP32-CAM to Wi-Fi
start HTTP server
serve / as a browser page
serve /jpg as a JPEG snapshot
serve /status as JSON status
```

Open the printed IP address in a browser. If the image updates, the camera is working.

## Files

```text
01_camera_http_snapshot.ino
README.md
```

## Arduino IDE Setup

Install the ESP32 board package:

```text
Tools -> Board -> Boards Manager
search: esp32
install: esp32 by Espressif Systems
```

Recommended board selection:

```text
Board: AI Thinker ESP32-CAM
PSRAM: Enabled
```

If that board is not available, use:

```text
Board: ESP32 Dev Module
PSRAM: Enabled
Flash Mode: QIO or DIO
```

## Wi-Fi Setup

Edit these values before uploading:

```cpp
const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
```

ESP32-CAM uses 2.4 GHz Wi-Fi. If you use iPhone hotspot, enable compatibility mode if needed.

## Upload Wiring

Most ESP32-CAM boards do not have built-in USB. Use a USB-to-TTL adapter.

```text
USB-TTL 5V  -> ESP32-CAM 5V
USB-TTL GND -> ESP32-CAM GND
USB-TTL TX  -> ESP32-CAM U0R
USB-TTL RX  -> ESP32-CAM U0T
GPIO 0      -> GND only while uploading
```

Use 5V for power, but use 3.3V UART logic if your adapter has a logic-level switch. ESP32 GPIO is not 5V tolerant.

Upload flow:

```text
connect GPIO 0 to GND
press RST
click Upload
wait until upload starts
disconnect GPIO 0 from GND
press RST again
open Serial Monitor at 115200
```

## Expected Serial Output

```text
ESP32-CAM Camera HTTP Snapshot Test started.
PSRAM found: yes
Camera initialized.
Wi-Fi connected.
IP address: 172.20.10.x
HTTP server started.
Open: http://172.20.10.x
```

## Browser Routes

```text
/       browser page with auto-refreshing image
/jpg    raw JPEG snapshot
/status JSON status
```

## Troubleshooting

If camera init fails:

```text
enable PSRAM in Arduino IDE
check camera ribbon cable orientation
use stable 5V power
select AI Thinker ESP32-CAM board
enable PSRAM when using ESP32 Dev Module
press RST after upload
```

If Serial Monitor prints `PSRAM found: no`, the firmware will fall back to a very small `QQVGA` frame, but the correct fix for most AI Thinker ESP32-CAM boards is to enable PSRAM in the board settings and upload again.

If Wi-Fi connects but image does not load:

```text
make sure browser device is on the same network
try /jpg directly
lower frame size from VGA to QVGA in the sketch
```
