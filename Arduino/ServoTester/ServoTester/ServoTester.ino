#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <string>
#include "ESP8266WiFi.h"
#include <Wire.h>
#define WIRE Wire

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

//Converts deg (0-180) to pulsewidth between min and max
int pulse(int deg){
  return map(deg, 0, 180, SERVOMIN, SERVOMAX);
}


//DEFAULT Angles (Rest Position):
//0: 1°   1: 1°   2: 90°
//3: 65°  4: 70°  5: 1°
//Angles for all servos
int defPos[6] = {1, 1, 90, 65, 70, 1};
int pos[6] = {1, 1, 90, 65, 70, 1};



// our servo # counter
uint8_t servonum = 0;



void setup() {
  Serial.begin(9600);
  Serial.setDebugOutput(true);
  Serial.print("Servo Test on Servo #");
  Serial.println(servonum);

  pwm.begin();

  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates

  yield();
  resetPos();
  wifi();

}

void wifi(){
  const char* ssid = "WOKO";
  const char* password = "wlan-4-woko";


  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("nConnecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  //Successful Connect:
  nod();
}


void nod(){
  pwm.setPWM(4, 0, pulse(pos[4]+20));
  delay(250);
  pwm.setPWM(4, 0, pulse(pos[4]));
}

void resetPos() {
  pwm.setPWM(0, 0, pulse(defPos[0]));
  delay(10);
  pwm.setPWM(1, 0, pulse(defPos[1]));
  delay(10);
  pwm.setPWM(2, 0, pulse(defPos[2]));
  delay(10);
  pwm.setPWM(3, 0, pulse(defPos[3]));
  delay(10);
  pwm.setPWM(4, 0, pulse(defPos[4]));
  delay(10);
  pwm.setPWM(5, 0, pulse(defPos[5]));
}


void loop() {

  while(Serial.available()==0){}

  String input = Serial.readString();

  if(input=="reset"){
    resetPos();
    Serial.println(input);
  }
  if(input=="nod"){
    nod();
    Serial.println("Nodding...");
  }
  if(input.substring(0, 3)=="get"){
    Serial.println(pos[input.substring(4).toInt()]);
  }
  // e.g. "5 145"
  else{
    servonum = input.substring(0, 1).toInt();
    pwm.setPWM(servonum, 0, pulse(input.substring(2).toInt()));
    pos[servonum] = input.substring(2).toInt();
    Serial.println(input);
    Serial.println(pos[servonum]);
  }
}
