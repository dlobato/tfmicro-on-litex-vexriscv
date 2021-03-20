#include <cinttypes>
#include <irq.h>
#include <base/uart.h>
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/kernels/kernel_util.h"


extern char model_data[];
extern unsigned model_data_size;

// Create an area of memory to use for input, output, and intermediate arrays.
  // The size of this will depend on the model you're using, and may need to be
  // determined by experimentation.
const int tensor_arena_size = 20 * 1024;
uint8_t tensor_arena[tensor_arena_size];

int main()
{
#ifdef CONFIG_CPU_HAS_INTERRUPT
  irq_setmask(0);
  irq_setie(1);
#endif
  uart_init();

  // Set up profiling
  tflite::MicroProfiler profiler;

  const tflite::Model *model = tflite::GetModel((const void *)model_data);
  if (model->version() != TFLITE_SCHEMA_VERSION)
  {
    TF_LITE_REPORT_ERROR(tflite::GetMicroErrorReporter(),
        "Model provided is schema version %d not equal "
        "to supported version %d.",
        model->version(), TFLITE_SCHEMA_VERSION);
    return 1;
  }

  // This pulls in all the operation implementations we need.
  static tflite::AllOpsResolver resolver;

  // Build an interpreter to run the model with.
  tflite::MicroInterpreter interpreter(model, resolver, tensor_arena, tensor_arena_size, tflite::GetMicroErrorReporter(), &profiler);
  interpreter.AllocateTensors();

  // Get information about the memory area to use for the model's input.
  TfLiteTensor *model_input = interpreter.input(0); // dim: 1x28x28x1
  if ((model_input->dims->size != 4) || 
      (model_input->dims->data[0] != 1) ||
      (model_input->dims->data[1] != 28) ||
      (model_input->dims->data[2] != 28) ||
      (model_input->dims->data[3] != 1) ||
      (model_input->type != kTfLiteInt8))
  {
    TF_LITE_REPORT_ERROR(tflite::GetMicroErrorReporter(), "Bad input tensor parameters in model");
    return 1;
  }

  // Get information about the memory area for the model's output.
  TfLiteTensor *model_output = interpreter.output(0); // dim: 1x10
  if ((model_output->dims->size != 2) || 
      (model_output->dims->data[0] != 1) ||
      (model_output->dims->data[1] != 10) ||
      (model_output->type != kTfLiteInt8))
  {
    TF_LITE_REPORT_ERROR(tflite::GetMicroErrorReporter(), "Bad output tensor parameters in model");
    return 1;
  }

  uint8_t input_data[28*28] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0, 84,185,159,151, 60, 36,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,222,254,254,254,254,241,198,198,198,198,198,198,198,198,170, 52,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0, 67,114, 72,114,163,227,254,225,254,254,254,250,229,254,254,140,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 17, 66, 14, 67, 67, 67, 59, 21,236,254,106,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 83,253,209, 18,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 22,233,255, 83,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,129,254,238, 44,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 59,249,254, 62,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,133,254,187,  5,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  9,205,248, 58,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,126,254,182,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 75,251,240, 57,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 19,221,254,166,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  3,203,254,219, 35,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 38,254,254, 77,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 31,224,254,115,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,133,254,254, 52,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 61,242,254,254, 52,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,121,254,254,219, 40,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,121,254,207, 18,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
  };

  while(1)
  {
    profiler.ClearEvents();
    //set input
    for (int i = 0; i < sizeof(input_data); i++)
    {
      model_input->data.int8[i] = input_data[i] ^ 0x80;
    }

    // Run the model
    TfLiteStatus invoke_status = interpreter.Invoke();
    if (invoke_status != kTfLiteOk)
    {
      TF_LITE_REPORT_ERROR(tflite::GetMicroErrorReporter(), "Invoke failed");
      return 1;
    }

    // get output argmax
    uint8_t argmax = 0;
    for (size_t i = 1; i < model_output->dims->data[1]; i++) {
      if (model_output->data.int8[i] > model_output->data.int8[argmax]) {
        argmax = i;
      }
    }
    MicroPrintf("Micro MNIST: it's a %d", argmax);
    profiler.Log();
  }
}