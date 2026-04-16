# 22 - TFLite Micro Minimal Resolver

This experiment runs the same deployed sine model as experiment 20, but replaces `AllOpsResolver` with `MicroMutableOpResolver`.

## Goal

The goal is to register only the TensorFlow Lite Micro operators that the model actually needs.

Experiment 20 used:

```cpp
tflite::AllOpsResolver resolver;
```

That is convenient, but it includes many operators the firmware does not need.

Experiment 22 uses:

```cpp
tflite::MicroMutableOpResolver<1> resolver;
resolver.AddFullyConnected();
```

This is closer to real MCU deployment practice.

## Why This Matters

On ESP32, flash and RAM matter. A production TinyML firmware should avoid pulling unnecessary code into the binary.

The deployment pattern becomes:

```text
inspect model
-> identify required operators
-> register only those operators
-> allocate tensors
-> run inference
-> measure size and latency
```

## How To Run

1. Install `Chirale_TensorFLowLite` in Arduino IDE.
2. Open `22_tflite_micro_minimal_resolver.ino`.
3. Select `ESP32 Dev Module`.
4. Upload to the ESP32.
5. Open Serial Monitor at `115200`.

No microphone, Wi-Fi, external sensor, or external display is needed.

## Expected Result

The LED should still fade smoothly, and Serial Monitor should print:

```text
predicted_sin=...
actual_sin=...
inference=...us
```

If the firmware prints an error about a missing op, that means the model needs another operator added to the resolver.

