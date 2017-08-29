#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <string>
#include "ESP8266WiFi.h"
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <ArduinoOTA.h>
#include <math.h>

//Gauss function for speedcontrol:  ( Bell Curve )
// exp(-22*pow((x-0.5), 2))  (D: [0,1], W: [0,1])
//Exp function BEETTTTEERR:
// exp(5*pow((x-1), 3))

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

// our servo # counter
uint8_t servonum = 0;

//Variables
const char* ssid = "WOKO";
const char* password = "wlan-4-woko";
const char* OTAName = "Dummy";
const char* OTAPassword = "lemons";

ESP8266WiFiMulti wifiMulti;
ESP8266WebServer server(80);
File fsUploadFile;


//DEFAULT Angles (Rest Position):
//0: 1°   1: 1°   2: 90°
//3: 65°  4: 70°  5: 1°
//Angles for all servos
int defPos[6] = {1, 1, 90, 65, 70, 1};
int pos[6] = {1, 1, 90, 65, 70, 1};

//Ticker timer;


void setup() {
  Serial.begin(9600);
  Serial.setDebugOutput(true);

  pwm.begin();

  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates

  //resetPos();

  yield();
  wifi();
  startOTA();
  startSPIFFS();               // Start the SPIFFS and list all contents
  startServer();


  nod();
  //no();

}


//Converts deg (0-180) to pulsewidth between min and max
int pulse(int deg, int servo = 0, int min = 100, int max = 600){
  if(servo<=3){
    return map(deg, 0, 180, min, max);
  }
  else if(servo>=4){
    return map(deg, 0, 180, 150, 500);
  }
}


void setPosHard(int servo, int angle){
  pwm.setPWM(servo, 0, pulse(angle, servo));
  pos[servo] = angle;

}

void setPos(int servo, int angle, int minSteps=150){
  double startPulse = pulse(pos[servo], servo);
  double diff = pulse(angle, servo)-startPulse;
  double diffAbs = abs(diff);
  if(diffAbs==0){
    setPosHard(servo, angle);
    return;
  }
  if(diffAbs < minSteps){
    diffAbs = minSteps;
  }

  for(int i = 0; i <= diffAbs; i++){
    double step = i/diffAbs;
    double mult = exp(5*pow((step-1), 3));
    int newPulse = round(mult * diff) + startPulse;
    Serial.println(newPulse);
    pwm.setPWM(servo, 0, newPulse);
    yield();
  }

  //pwm.setPWM(servo, 0, pulse(angle, servo));
  pos[servo] = angle;

}




void wifi(){

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());


}

void startOTA() { // Start the OTA service
  ArduinoOTA.setHostname(OTAName);
  ArduinoOTA.setPassword(OTAPassword);

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\r\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("OTA ready\r\n");
}

void startSPIFFS() { // Start the SPIFFS and list all contents
  SPIFFS.begin();                             // Start the SPI Flash File System (SPIFFS)
  Serial.println("SPIFFS started. Contents:");
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {                      // List the file system contents
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Serial.printf("\tFS File: %s, size: %s\r\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    Serial.printf("\n");
  }
}



void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}



String formatBytes(size_t bytes) { // convert sizes in bytes to KB and MB
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  }
}


void startServer(){
  //WEBSERVER

  server.serveStatic("/index.html", SPIFFS, "/index.html");
  server.serveStatic("/js", SPIFFS, "/js");
  server.serveStatic("/css", SPIFFS, "/css", "max-age=86400");
  server.serveStatic("/images", SPIFFS, "/images", "max-age=86400");
  server.serveStatic("/", SPIFFS, "/index.html");

  server.on("/servo0", HTTP_POST, []() {
    String value = server.arg("value");
    setPos(0, value.toInt());
    Serial.println("Received Servo0 to " + value);
  });
  server.on("/servo1", HTTP_POST, []() {
    String value = server.arg("value");
    setPos(1, value.toInt());
    Serial.println("Received Servo1 to " + value);
  });
  server.on("/servo2", HTTP_POST, []() {
    String value = server.arg("value");
    setPos(2, value.toInt());
    Serial.println("Received Servo2 to " + value);
  });
  server.on("/servo3", HTTP_POST, []() {
    String value = server.arg("value");
    setPos(3, value.toInt());
    Serial.println("Received Servo3 to " + value);
  });
  server.on("/servo4", HTTP_POST, []() {
    String value = server.arg("value");
    setPos(4, value.toInt());
    Serial.println("Received Servo4 to " + value);
  });
  server.on("/servo5", HTTP_POST, []() {
    String value = server.arg("value");
    setPos(5, value.toInt());
    Serial.println("Received Servo5 to " + value);
  });




  server.on("/reset", HTTP_POST, []() {
    resetPos();
    Serial.println("Received reset");
  });
  server.on("/nod", HTTP_POST, []() {
    nod();
    Serial.println("Received nod");
  });
  server.on("/no", HTTP_POST, []() {
    no();
    Serial.println("Received no");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.print("HTTP server started @ ");
  Serial.println(WiFi.localIP());

}



void nod(){

  setPos(4, pos[4]+20, 50);
  setPos(4, pos[4]-20, 50);
}
void no(){
  setPos(0, pos[0]+10, 50);
  setPos(0, pos[0]-10, 50);
}

void resetPos() {
  Serial.print("Resetting...");
  for(int i = 0; i<=5; i++){
    setPosHard(i, defPos[i]);
  }
}

void checkInput(String input){
  if(input=="reset"){
    resetPos();
    Serial.println(input);
  }
  else if(input=="nod"){
    nod();
    Serial.println("Nodding...");
  }
  else if(input=="no"){
    no();
    Serial.println("No...");
  }
  else if(input.substring(0, 3)=="get"){
    Serial.println(pos[input.substring(4).toInt()]);
  }
  // e.g. "5 145"
  else{
    servonum = input.substring(0, 1).toInt();
    setPos(servonum, input.substring(2).toInt());
    Serial.println(input);
  }
}


void loop() {
  server.handleClient();
  ArduinoOTA.handle();

  //timer.update();


  if(Serial.available()!=0){
    String input = Serial.readString();
    checkInput(input);
  }

}
