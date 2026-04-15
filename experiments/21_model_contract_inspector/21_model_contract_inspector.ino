#include <Chirale_TensorFlowLite.h>

#include "model.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"

constexpr int kTensorArenaSize = 8 * 1024;
alignas(16) uint8_t tensorArena[kTensorArenaSize];

const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;

TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;

void printTensorType(TfLiteType type) {
    switch (type)
    {
    case kTfLiteFloat32:
        Serial.print("float32");
        break;
    case kTfLiteInt8:
        Serial.print("int8");
        break;
    case kTfLiteUInt8:
        Serial.print("uint8");
        break;
    case kTfLiteInt16:
        Serial.print("int16");
        break;
    case kTfLiteInt32:
        Serial.print("int32");
        break;
    
    default:
        Serial.print("unknown(");
        Serial.print(type);
        Serial.print(")");
        break;
    }
}


void printTensorDims(TfLiteTensor* tensor) {
    Serial.print("[");

    for (int i = 0; i < tensor->dims->size; i++) {
        Serial.print(tensor->dims->data[i]);

        if (i < tensor->dims->size - 1) {
            Serial.print(", ");
        }
    }

    Serial.print("]");
}

void printTensorInfo(const char* name, TfLiteTensor* tensor) {
    Serial.print(name);
    Serial.println(":");

    Serial.print(" type: ");
    printTensorType(tensor->type);
    Serial.println();

    Serial.print(" dims: ");
    printTensorDims(tensor);
    Serial.println();

    Serial.print(" bytes: ");
    Serial.println(tensor->bytes);

    Serial.print(" scale: ");
    Serial.println(tensor->params.scale, 8);

    Serial.print(" zero_point: ");
    Serial.println(tensor->params.zero_point);
}

void printMemorySnapshot(const char* label) {
    Serial.print(label);
    Serial.println(":");
    
    Serial.print(" free_heap: ");
    Serial.print(ESP.getFreeHeap());
    Serial.println(" bytes");

    Serial.print(" min_free_heap: ");
    Serial.print(ESP.getMinFreeHeap());
    Serial.print(" bytes ");
}

void setupModel() {
    model = tflite::GetModel(g_model);

    Serial.println("Model:");
    Serial.print(" model_size: ");
    Serial.print(g_model_len);
    Serial.println(" bytes ");

    Serial.print(" model_schema_version: ");
    Serial.println(model->version());

    Serial.print(" runtime_schema_version: ");
    Serial.println(TFLITE_SCHEMA_VERSION);

    if (model->version() != TFLITE_SCHEMA_VERSION) {
        Serial.println("ERROR: Model schema version mismatch.");
        while(true) {
            delay(1000);
        }
    }

    static tflite::AllOpsResolver resolver;
    static tflite::MicroInterpreter staticInterpreter(
        model,
        resolver,
        tensorArena,
        kTensorArenaSize
    );

    interpreter = &staticInterpreter;

    printMemorySnapshot("Before AllocateTensors");

    TfLiteStatus allocateStatus = interpreter->AllocateTensors();

    if (allocateStatus != kTfLiteOk) {
        Serial.println("ERROR: AllocateTensors() failed.");
        Serial.println("Try increasing kTensorArenaSize.");
        while (true) {
            delay(1000);
        }
    }

    printMemorySnapshot("After AllocateTensors");

    input = interpreter->input(0);
    output = interpreter->output(0);

    Serial.println();
    Serial.println("Tensor arena:");
    Serial.print(" size: ");
    Serial.print(kTensorArenaSize);
    Serial.println(" bytes");

    Serial.println();
    printTensorInfo("Input tensor 0", input);

    Serial.println();
    printTensorInfo("Output tensor 0", output);
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("ESP32 Model Contract Inspector started.");
    Serial.println();

    setupModel();

    Serial.println();
    Serial.println("Contract summary:");
    Serial.println("  This firmware must feed input tensor 0 using the exact type, shape, scale, and zero_point printed above.");
    Serial.println("  Any future model replacement must be checked here before writing preprocessing code.");
}

void loop() {}