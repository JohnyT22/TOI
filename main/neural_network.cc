// neural_network.c
// TOI 1.projekt
// Autor: Jan Tomeček, FIT
// Vyvýjeno ve VSCode pomocí ESP-IDF rozšíření
// zdoj odkud bylo cerpano
// https://github.com/espressif/tflite-micro-esp-examples

#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"


#include <esp_heap_caps.h>
#include <esp_timer.h>
#include <esp_log.h>

#include "neural_network.h"

extern unsigned char model_tflite[];
extern unsigned int model_tflite_len;

const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;

const int kTensorArenaSize = 4*sizeof(float) + model_tflite_len;
static uint8_t *tensor_arena;

void NNInit(){
    model = tflite::GetModel(model_tflite);

    if (model->version() != TFLITE_SCHEMA_VERSION) {
        MicroPrintf("Model provided is schema version %d not equal to supported "
                    "version %d.", model->version(), TFLITE_SCHEMA_VERSION);
        return;
    }
    
    if (tensor_arena == NULL) {
        tensor_arena = (uint8_t *) heap_caps_malloc(kTensorArenaSize, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    }
    if (tensor_arena == NULL) {
        printf("Couldn't allocate memory of %d bytes\n", kTensorArenaSize);
        return;
    }

    static tflite::MicroMutableOpResolver<2> micro_op_resolver;
    micro_op_resolver.AddLogistic();
    micro_op_resolver.AddFullyConnected();
    

     static tflite::MicroInterpreter static_interpreter(
        model, micro_op_resolver, tensor_arena, kTensorArenaSize);
    interpreter = &static_interpreter;

    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk) {
        MicroPrintf("AllocateTensors() failed");
        return;
    }

    input = interpreter->input(0);
    output = interpreter->output(0);
}

float* getNNInput(){
    return input->data.f;
}

float* getNNPrediction(){
    interpreter->Invoke();
    return output->data.f;
}