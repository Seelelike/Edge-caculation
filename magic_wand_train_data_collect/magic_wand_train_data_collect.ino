#include <Arduino_LSM9DS1.h>
#include <Arduino.h>

const int SAMPLE_RATE = 50;
int LED = LED_BUILTIN;
void setup() {
  // put your setup code here, to run once:
    IMU.begin();
    IMU.setOneShotMode();
    Serial.begin(115200);
    pinMode(LED,OUTPUT);
    digitalWrite(LED,HIGH);
    Serial.println();
    Serial.println();
    //Serial.println("开始采集");
}

void loop() {
  // put your main code here, to run repeatedly:
    delay(250);
    digitalWrite(LED,HIGH);
    delay(250);
    digitalWrite(LED,LOW);
    delay(250);
    digitalWrite(LED,HIGH);
    delay(250);
    digitalWrite(LED,LOW);

    Serial.println("x,y,z");
    int count =0;
    float x,y,z;
    for(count=0;count <150;count++)
    {
        if(!IMU.accelerationAvailable())
        {
            count--;
            continue;
        }
        IMU.readAcceleration(x,y,z);
        //Serial.print(count);
        //Serial.print(':');
        Serial.print(x,3);
        Serial.print(',');
        Serial.print(y,3);
        Serial.print(',');
        Serial.println(z,3);
        delay(1000/SAMPLE_RATE);
    }
    Serial.println();
    Serial.println();
    delay(3000);
}
