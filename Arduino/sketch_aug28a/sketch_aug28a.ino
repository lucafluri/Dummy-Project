/*************************************************** 
  This is an example for our Adafruit 16-channel PWM & Servo driver
  Servo test - this will drive 16 servos, one after the other

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/815

  These displays use I2C to communicate, 2 pins are required to  
  interface. For Arduino UNOs, thats SCL -> Analog 5, SDA -> Analog 4

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

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
#define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  500 // this is the 'maximum' pulse length count (out of 4096)

// our servo # counter
uint8_t servonum = 2;

void setup() {
  Serial.begin(9600);
  Serial.println("Servo Test on Servo #");
  Serial.println(servonum);

  pwm.begin();
  
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates

  yield();
}



void loop() {
  // Drive each servo one at a time
  uint16_t oldInput;
  uint16_t input = Serial.parseInt();
  if(input == 99){
    Serial.println("Select Servo 1-7");
    servonum = Serial.parseInt();  
    Serial.println("Servo Test on Servo #");
    Serial.println(servonum);
  }
  if(input != oldInput){
    Serial.println(input);
    pwm.setPWM(servonum, 0, input);
    delay(10);
    oldInput = input;
  }
}
