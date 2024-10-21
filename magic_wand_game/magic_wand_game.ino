#include <TensorFlowLite.h>
#include <Arduino_LSM9DS1.h>

#include <Adafruit_NeoPixel.h>
#define NUM_LEDS 30
#define DATA_PIN PIN_A0
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);

#define LED_IN D10
#define LED_RCK D9
#define LED_SCK D8

int8_t HP=8;
int8_t MP=8;
int8_t state=0;


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

    strip.begin();           // 初始化灯带
    strip.show(); 

    pinMode(LED_IN,OUTPUT);
    pinMode(LED_RCK,OUTPUT);
    pinMode(LED_SCK,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
    
    if(state==0)
    {
      HP=8;
      MP=4;
      String inString="";
      while(Serial.available()>0){
        inString += char(Serial.read());
        delay(10);      // 延时函数用于等待字符完全进入缓冲区，可以尝试没有延时，输出结果会是什么
      }
      if(inString!=""){
        if(inString[0] == 'S')
        {
          state = 1;
        }
      }
    }
    else if(state == 1)
    {
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
  delay(200);
    Serial.print(model_output->data.f[0],2);
    Serial.print(',');
    Serial.print(model_output->data.f[1],2);
    Serial.print(',');
    Serial.print(model_output->data.f[2],2);
    Serial.print(',');
    Serial.print(model_output->data.f[3],2);
    Serial.println(' ');
  if(model_output->data.f[0]>0.9) LED_CONTROL(1);
  else if(model_output->data.f[2]>0.9) LED_CONTROL(2);
  else if(model_output->data.f[3]>0.9) LED_CONTROL(3);
  delay(500);
  String inString="";
      while(Serial.available()>0){
        inString += char(Serial.read());
        delay(10);      // 延时函数用于等待字符完全进入缓冲区，可以尝试没有延时，输出结果会是什么
      }
      if(inString!=""){
        Serial.print("Input String:");
        Serial.println(inString);
        if(inString[0] == 'Y')
        {
          state = 2;
        }
        else if(inString[0] == 'N')
        {
          state = 3;
        }
        else if(inString[0] == 'W')
        {
          state = 4;
        }
        else
        {
          HP=inString[0];
          MP=inString[2];
          flashHM();
        }
      }
  //delay(500);
  }
  else if(state == 2)
  {
    for(int i=0;i<5;i++)
    {
      HP=0;
      MP=0;
      flashHM();
      delay(200);
      HP=8;
      MP=8;
      flashHM();
    }
      HP=8;
      MP=8;
      flashHM();
      state = 4;
  }
  else if(state == 3)
  {
    for(int i=0;i<5;i++)
    {
      HP=0;
      MP=0;
      flashHM();
      delay(200);
      HP=8;
      MP=8;
      flashHM();
    }
    HP=0;
    MP=0;
    flashHM();
    state =4;
  }
  else if (state =4)
  {

  }
}

void LED_CONTROL(int i)
{
    switch(i)
    {
      case 1:
      {
        for (int i = 0; i < NUM_LEDS; i++)
        {
          strip.setPixelColor(i, 20, 0, 0);  // 设置颜色为红色
          strip.show();              // 展示颜色
          delay(50-i);                 // 50毫秒的延迟
        }
        for (int i = 0; i < NUM_LEDS; i++)
        {
          strip.setPixelColor(i, 0, 0, 0);  // 设置颜色为红色
          strip.show();              // 展示颜色
          delay(50-i);                 // 50毫秒的延迟
        }
        break;
      }
      case 2:
      {
        for (int i = 0; i < NUM_LEDS; i++)
        {
          strip.setPixelColor(i, 0, 20, 0);  // 设置颜色为红色
          strip.show();              // 展示颜色
          delay(50-i);                 // 50毫秒的延迟
        }
        for (int i = 0; i < NUM_LEDS; i++)
        {
          strip.setPixelColor(i, 0, 0, 0);  // 设置颜色为红色
          strip.show();              // 展示颜色
          delay(50-i);                 // 50毫秒的延迟
        }
        break;
      }
      case 3:
      {
        for (int i = 0; i < NUM_LEDS; i++)
        {
          strip.setPixelColor(i, 0, 0, 20);  // 设置颜色为红色
          strip.show();              // 展示颜色
          delay(50-i);                 // 50毫秒的延迟
        }
        for (int i = 0; i < NUM_LEDS; i++)
        {
          strip.setPixelColor(i, 0, 0, 0);  // 设置颜色为红色
          strip.show();              // 展示颜色
          delay(50-i);                 // 50毫秒的延迟
        }
        break;
      }
      default:break;
    }
}

void flashHM()
{
  for(int i=0;i<8;i++)
  {
    if(i<HP) digitalWrite(LED_IN,LOW);
    else digitalWrite(LED_IN,HIGH);
    digitalWrite(LED_SCK,HIGH);
    delay(1);
    digitalWrite(LED_SCK,LOW);
  }
  for(int i=0;i<8;i++)
  {
    if(i<MP) digitalWrite(LED_IN,LOW);
    else digitalWrite(LED_IN,HIGH);
    digitalWrite(LED_SCK,HIGH);
    delay(1);
    digitalWrite(LED_SCK,LOW);
  }
  digitalWrite(LED_RCK,HIGH);
  delay(1);
  digitalWrite(LED_RCK,LOW);

}
