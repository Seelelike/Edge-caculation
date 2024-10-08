#include <TensorFlowLite.h>
#include <Arduino_LSM9DS1.h>


#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "gesture_model.h"

namespace{
const tflite::Model* model = nullptr;
tflite::ErrorReporter* error_reporter = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* model_input = nullptr;
TfLiteTensor* model_output = nullptr;
int input_length;
}
const int SAMPLE_RATE = 50;
constexpr int kTensorArenaSize = 60 * 1024;
uint8_t tensor_arena[kTensorArenaSize];
int LED = LED_BUILTIN;

void setup() {
    
    static tflite::MicroErrorReporter micro_error_reporter;
    error_reporter = &micro_error_reporter;

    model = tflite::GetModel(gesture_model_data);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Model provided is schema version %d not equal "
                         "to supported version %d.",
                         model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

    static tflite::AllOpsResolver resolver;
    static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
    interpreter = &static_interpreter;
    interpreter->AllocateTensors();
    
    model_input = interpreter->input(0);
    model_output = interpreter->output(0);
  if ((model_input->dims->size != 4) || (model_input->dims->data[0] != 1) ||
      (model_input->dims->data[1] != 150) ||
      (model_input->dims->data[2] != 3) ||
      (model_input->type != kTfLiteFloat32)) {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Bad input tensor parameters in model");
    return;
    }

    input_length = model_input->bytes / sizeof(float);

    if (!IMU.begin()) {
    TF_LITE_REPORT_ERROR(error_reporter, "Failed to initialize IMU");
    return;
    }
    IMU.setOneShotMode();

    pinMode(LED,OUTPUT);
    digitalWrite(LED,HIGH);

    Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
    float x,y,z;
    int count;
    delay(250);
    digitalWrite(LED,HIGH);
    delay(250);
    digitalWrite(LED,LOW);
    delay(250);
    digitalWrite(LED,HIGH);
    delay(250);
    digitalWrite(LED,LOW);
    
    for(count=0;count <150;count++)
    {
        if(!IMU.accelerationAvailable())
        {
            count--;
            continue;
        }
        IMU.readAcceleration(x,y,z);
        model_input->data.f[count*3]=x;
        model_input->data.f[count*3+1]=y;
        model_input->data.f[count*3+2]=z;
        delay(1000/SAMPLE_RATE);
    }
    TfLiteStatus invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed on index");
    return;
  }
    Serial.print(model_output->data.f[0]);
    Serial.print(' ');
    Serial.print(model_output->data.f[1]);
    Serial.print(' ');
    Serial.print(model_output->data.f[2]);
    Serial.print(' ');
    Serial.print(model_output->data.f[3]);
    Serial.println(' ');
}

