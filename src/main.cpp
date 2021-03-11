/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

// #include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"

#include "model.h"
// #include "accelerometer_handler.h"

#include "mbed.h"
#include "stdlib.h"

#include "USBSerial.h"

#define kChannelNumber 3

USBSerial ser;

// This constant represents the range of x values our model was trained on,
// which is from 0 to (2 * Pi). We approximate Pi to avoid requiring additional
// libraries.
const float kXrange = 2.f * 3.14159265359f;

// This constant determines the number of inferences to perform across the range
// of x values defined above. Since each inference takes time, the higher this
// number, the more time it will take to run through the entire range. The value
// of this constant can be tuned so that one full cycle takes a desired amount
// of time. Since different devices take different amounts of time to perform
// inference, this value should be defined per-device.
const int kInferencesPerCycle = 100;

DigitalOut led((PinName)0x6C);

// Globals, used for compatibility with Arduino-style sketches.
namespace {
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
// no output tensors in magicwand example
// TfLiteTensor* output = nullptr;
int inference_count = 0;
int input_length;

// Create an area of memory to use for input, output, and intermediate arrays.
// Minimum arena size, at the time of writing. After allocating tensors
// you can retrieve this value by invoking interpreter.arena_used_bytes().
const int kTensorArenaSize = 30 * 1024;
uint8_t tensor_arena[kTensorArenaSize];
}  // namespace

// The name of this function is important for Arduino compatibility.
void setup() {
  // Set up logging. Google style is to avoid globals or statics because of
  // lifetime uncertainty, but since this has a trivial destructor it's okay.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  model = tflite::GetModel(g_model);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Model provided is schema version %d not equal "
                         "to supported version %d.",
                         model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  // Pull in only the operation implementations we need.
  // This relies on a complete list of all the ops needed by this graph.
  // An easier approach is to just use the AllOpsResolver, but this will
  // incur some penalty in code space for op implementations that are not
  // needed by this graph.
  static tflite::MicroMutableOpResolver<5> micro_op_resolver;
  micro_op_resolver.AddDepthwiseConv2D();
  micro_op_resolver.AddMaxPool2D();
  micro_op_resolver.AddConv2D();
  micro_op_resolver.AddFullyConnected();
  micro_op_resolver.AddSoftmax();

  // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(
      model, micro_op_resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors.
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed");
    return;
  }

  // Obtain pointers to the model's input and output tensors.
    // Obtain pointer to the model's input tensor.
  input = interpreter->input(0);
  if ((input->dims->size != 4) || (input->dims->data[0] != 1) ||
      (input->dims->data[1] != 128) ||
      (input->dims->data[2] != kChannelNumber) ||
      (input->type != kTfLiteFloat32)) {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Bad input tensor parameters in model");
    return;
  }

  // no output tensors in magicwand example
  // output = interpreter->output(0);

  input_length = input->bytes / sizeof(float);

  // TfLiteStatus setup_status = SetupAccelerometer(error_reporter);
  // if (setup_status != kTfLiteOk) {
  //   TF_LITE_REPORT_ERROR(error_reporter, "Set up failed\n");
  // }

  // Keep track of how many inferences we have performed.
  inference_count = 0;
}

// The name of this function is important for Arduino compatibility.
void loop() {
  // Calculate an x value to feed into the model. We compare the current
  // inference_count to the number of inferences per cycle to determine
  // our position within the range of possible x values the model was
  // trained on, and use this to calculate a value.
  // float position = static_cast<float>(inference_count) /
  //                  static_cast<float>(kInferencesPerCycle);
  // float x_val = position * kXrange;

  // Place our calculated x value in the model's input tensor
  // input->data.f[0] = x_val;

  // Run inference, and report any error
  // TfLiteStatus invoke_status = interpreter->Invoke();
  // if (invoke_status != kTfLiteOk) {
  //   TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed on x_val: %f\n",
  //                        static_cast<double>(x_val));
  //   return;
  // }

  // Read the predicted y value from the model's output tensor
  // float y_val = output->data.f[0];
  // led = y_val>=0;

  // Output the results.
  // Log the current X and Y values
  // TF_LITE_REPORT_ERROR(error_reporter, "x_value: %f, y_value: %f\n",
  //                      static_cast<double>(x_val),
  //                      static_cast<double>(y_val));

  // Attempt to read new data from the accelerometer.
  // bool got_data =
  //     ReadAccelerometer(error_reporter, input->data.f, input_length);
  // If there was no new data, wait until next time.
  // if (!got_data) return;

  // Run inference, and report any error.
  // TfLiteStatus invoke_status = interpreter->Invoke();
  // if (invoke_status != kTfLiteOk) {
  //   TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed on index: %d\n",
  //                        begin_index);
  //   return;
  // }
  // Analyze the results to obtain a prediction
  // int gesture_index = PredictGesture(interpreter->output(0)->data.f);

  ser.printf("test\n\r");
  // Increment the inference_counter, and reset it if we have reached
  // the total number per cycle
  inference_count += 1;
  ser.printf("inf_c: %d\n\r",inference_count);
  // if (inference_count >= kInferencesPerCycle) inference_count = 0;
}


int main(void) {
  setup();
  while (true) {
    loop();
  }
}


