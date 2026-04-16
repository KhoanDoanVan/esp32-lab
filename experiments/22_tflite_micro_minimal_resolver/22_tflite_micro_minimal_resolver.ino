#include <Chirale_TensorFlowLite.h>
#include <math.h>

#include "model.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"

const int LED_PIN = 2;

constexpr int kTensorArenaSize = 8 * 1024;
alignas(16) uint8_t tensorArena[kTensorArenaSize];

const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;

TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;

constexpr float kXRange = 6.28318530718f;
constexpr unsigned long kInferenceIntervalMs = 200;

unsigned long previousInferenceTime = 0;
float xValue = 0.0f;

int8_t quantizeFloatToInt8(float value, TfLiteTensor* tensor) {
    int quantized = round(value / tensor->params.scale) + tensor->params.zero_point;

    if (quantized < -128) {
        quantized = -128;
    }

    if (quantized > 127) {
        quantized = 127;
    }

    return static_cast<int8_t>(quantized);
}

float dequantizeInt8ToFloat(int8_t value, TfLiteTensor* tensor) {
    return (value - tensor->params.zero_point) * tensor->params.scale;
}

void printTensorInfo(const char* name, TfLiteTensor* tensor) {
    Serial.print(name);
    Serial.print(" type=");
    Serial.print(tensor->type);

    Serial.print(" bytes=");
    Serial.print(tensor->bytes);

    Serial.print(" scale=");
    Serial.print(tensor->params.scale, 8);

    Serial.print(" zero_point=");
    Serial.println(tensor->params.zero_point);
}

void printMemoryInfo(const char* label) {
    Serial.println(label);

    Serial.print("Free heap: ");
    Serial.print(ESP.getFreeHeap());
    Serial.println(" bytes");

    Serial.print("Minimum free heap: ");
    Serial.print(ESP.getMinFreeHeap());
    Serial.println(" bytes");

    Serial.print("Sketch size: ");
    Serial.print(ESP.getSketchSize());
    Serial.println(" bytes");

    Serial.print("Free sketch space: ");
    Serial.print(ESP.getFreeSketchSpace());
    Serial.println(" bytes");
}

void setupModel() {
    model = tflite::GetModel(g_model);

    Serial.print("Model size: ");
    Serial.print(g_model_len);
    Serial.println(" bytes");

    Serial.print("Model schema version: ");
    Serial.println(model->version());

    Serial.print("Runtime schema version: ");
    Serial.println(TFLITE_SCHEMA_VERSION);

    if (model->version() != TFLITE_SCHEMA_VERSION) {
        Serial.println("ERROR: Model schema version mismatch.");
        while (true) {
            delay(1000);
        }
    }

    static tflite::MicroMutableOpResolver<1> resolver;

    if (resolver.AddFullyConnected() != kTfLiteOk) {
        Serial.println("ERROR: Failed to add FullyConnected op.");
        while (true) {
            delay(1000);
        }
    }

    static tflite::MicroInterpreter staticInterpreter(
        model,
        resolver,
        tensorArena,
        kTensorArenaSize
    );

    interpreter = &staticInterpreter;

    printMemoryInfo("Before AllocateTensors:");

    TfLiteStatus allocateStatus = interpreter->AllocateTensors();
    if (allocateStatus != kTfLiteOk) {
        Serial.println("ERROR: AllocateTensors() failed.");
        Serial.println("The model may need another op, or kTensorArenaSize may be too small.");
        while (true) {
            delay(1000);
        }
    }

    printMemoryInfo("After AllocateTensors:");

    input = interpreter->input(0);
    output = interpreter->output(0);

    Serial.print("Tensor arena: ");
    Serial.print(kTensorArenaSize);
    Serial.println(" bytes");

    printTensorInfo("Input tensor", input);
    printTensorInfo("Output tensor", output);
}

void runSineInference(float x) {
    unsigned long startTime = micros();

    input->data.int8[0] = quantizeFloatToInt8(x, input);

    TfLiteStatus invokeStatus = interpreter->Invoke();
    if (invokeStatus != kTfLiteOk) {
        Serial.println("ERROR: Invoke failed.");
        return;
    }

    float predictedY = dequantizeInt8ToFloat(output->data.int8[0], output);
    float actualY = sin(x);
    unsigned long inferenceTimeUs = micros() - startTime;

    int ledBrightness = constrain(
        static_cast<int>((predictedY + 1.0f) * 127.5f),
        0,
        255
    );
    analogWrite(LED_PIN, ledBrightness);

    Serial.print("x=");
    Serial.print(x, 3);

    Serial.print(" predicted_sin=");
    Serial.print(predictedY, 3);

    Serial.print(" actual_sin=");
    Serial.print(actualY, 3);

    Serial.print(" output_int8=");
    Serial.print(output->data.int8[0]);

    Serial.print(" led=");
    Serial.print(ledBrightness);

    Serial.print(" inference=");
    Serial.print(inferenceTimeUs);
    Serial.println("us");
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.println("ESP32 TFLite Micro Minimal Resolver started.");
    Serial.println("This version registers only the operator required by the sine model.");

    setupModel();

    Serial.println("Running sine inference with MicroMutableOpResolver.");
}

void loop() {
    unsigned long currentTime = millis();

    if (currentTime - previousInferenceTime >= kInferenceIntervalMs) {
        previousInferenceTime = currentTime;

        runSineInference(xValue);

        xValue += 0.20f;
        if (xValue > kXRange) {
            xValue = 0.0f;
        }
    }
}
