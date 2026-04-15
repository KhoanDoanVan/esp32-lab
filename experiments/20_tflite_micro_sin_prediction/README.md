# 20 - TFLite Micro Hello World Deployment

This experiment deploys a real pre-trained TensorFlow Lite Micro model to the ESP32. It does not need any external sensor, button, display, or microphone.

The model is the classic TinyML **Hello World** model. It receives one numeric input, `x`, and predicts `sin(x)`. The firmware feeds `x` values from `0` to about `2*pi`, runs inference on the ESP32, prints the prediction, and maps the model output to the onboard LED brightness.

## Why This Matters

This is the first real deployment step in the lab:

```text
model.h in firmware
-> TFLite Micro interpreter
-> tensor arena allocation
-> quantize input
-> Invoke()
-> dequantize output
-> measure inference time
-> drive ESP32 output
```

There is no training on the ESP32. The model was already trained and converted into a C byte array in `model.h`.

## Arduino Library Needed

Install this library in Arduino IDE:

```text
Chirale_TensorFlowLite
```

The Arduino Library Manager may display the package name as `Chirale_TensorFLowLite`. Install that package if you see the capital `FLow` spelling. The sketch includes:

```cpp
#include <Chirale_TensorFlowLite.h>
```

The library targets Arduino boards with `mbed` or `esp32` architecture. It is experimental, but it is a practical way to run TensorFlow Lite Micro from Arduino-style ESP32 sketches.

## How To Run

1. Open Arduino IDE.
2. Install the ESP32 board package if it is not installed yet.
3. Install `Chirale_TensorFLowLite` from Library Manager.
4. Open `20_tflite_micro_hello_world.ino`.
5. Select your ESP32 DevKit V1 board, usually `ESP32 Dev Module`.
6. Select the correct USB serial port.
7. Upload the sketch.
8. Open Serial Monitor at `115200` baud.

No Wi-Fi, microphone, external button, or external sensor is needed.

If upload fails, hold the `BOOT` button when upload starts, then release it after the upload begins.

## Files

```text
20_tflite_micro_sin_prediction.ino
model.h
```

`model.h` is the compiled TFLite flatbuffer as a C array. It came from the Chirale TensorFlow Lite Micro Hello World example.

## Expected Serial Output

You should see lines like:

```text
ESP32 TFLite Micro Hello World started.
Model size: 2488 bytes
Model initialized.
x=0.000 predicted_sin=0.012 actual_sin=0.000 output_int8=...
x=1.000 predicted_sin=0.821 actual_sin=0.841 output_int8=...
```

The predicted value will not be perfect. That is normal. It is a tiny quantized model.

## What To Learn

- `model.h` is how a `.tflite` file is embedded into firmware.
- `tensorArena` is the RAM workspace used by TFLite Micro.
- `AllocateTensors()` prepares input, output, and intermediate tensors.
- Quantized models use `scale` and `zero_point`.
- `interpreter->Invoke()` is the actual inference call.
- `micros()` measures inference latency on the ESP32.

## Source

Model source:

```text
https://github.com/spaziochirale/Chirale_TensorFlowLite/blob/main/examples/hello_world/model.h
```

Library reference:

```text
https://github.com/spaziochirale/Chirale_TensorFlowLite
```
