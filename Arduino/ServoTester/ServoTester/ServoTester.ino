#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
// you can also call it with a different address you want
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41);

// Depending on your servo make, the pulse width min and max may vary, you
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
#define SERVOMIN  100 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)


//DEFAULT Angles (Rest Position):
//0: 1°   1: 1°   2: 90°
//3: 65°  4: 70°  5: 1°


// our servo # counter
uint8_t servonum = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Servo Test on Servo #");
  Serial.println(servonum);

  pwm.begin();

  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates

  yield();
  resetPos();

}

void resetPos() {
  pwm.setPWM(0, 0, map(1, 0, 180, 100, 600));
  delay(10);
  pwm.setPWM(1, 0, map(1, 0, 180, 100, 600));
  delay(10);
  pwm.setPWM(2, 0, map(90, 0, 180, 100, 600));
  delay(10);
  pwm.setPWM(3, 0, map(65, 0, 180, 100, 600));
  delay(10);
  pwm.setPWM(4, 0, map(70, 0, 180, 100, 600));
  delay(10);
  pwm.setPWM(5, 0, map(50, 0, 180, 100, 600));
}


void loop() {

  uint16_t oldInput = 0;
  uint16_t input = Serial.parseInt();
  if(input == 900){
    resetPos();
  }

  if(input == 990){
    servonum = 0;
    Serial.println("Servo #");
    Serial.print(servonum);
    input = Serial.parseInt();
  }
  if(input == 991){
    servonum = 1;
    Serial.println("Servo #");
    Serial.print(servonum);
    input = Serial.parseInt();
  }
  if(input == 992){
    servonum = 2;
    Serial.println("Servo #");
    Serial.print(servonum);
    input = Serial.parseInt();
  }
  if(input == 993){
    servonum = 3;
    Serial.println("Servo #");
    Serial.print(servonum);
    input = Serial.parseInt();
  }
  if(input == 994){
    servonum = 4;
    Serial.print("Servo #");
    Serial.println(servonum);
    input = Serial.parseInt();
  }
  if(input == 995){
    servonum = 5;
    Serial.println("Servo #");
    Serial.print(servonum);
    input = Serial.parseInt();
  }

  if(input == 999){
    Serial.println("Select Servo 0-7");
    while(Serial.available() == 0){}
    input = Serial.parseInt();
    if(input < 8 && input >= 0 && input != servonum){
      servonum = input;
      Serial.println("Servo Test on Servo #");
      Serial.print(servonum);
      input = Serial.parseInt();
    }

  }
  if(input != oldInput){
    Serial.println(input);
    input = map(input, 0, 180, 100, 600);
    pwm.setPWM(servonum, 0, input);
    delay(10);
    oldInput = input;
  }
}
