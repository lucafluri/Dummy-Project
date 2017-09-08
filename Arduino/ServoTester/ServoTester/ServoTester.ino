#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <string>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <ArduinoOTA.h>
#include <math.h>
#include <Ticker.h>
#include <WebSocketsClient.h>
#include <Hash.h>
WebSocketsClient webSocket;

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

#define SERVOMIN4 150
#define SERVOMAX4 500

#define SERVOMIN5 140
#define SERVOMAX5 586

#define SERVOMIN6 173 //GRIPPER SLOT
#define SERVOMAX6 294



// our servo # counter
uint8_t servonum = 0;

//Variables
const char* ssid = "WOKO";
const char* password = "wlan-4-woko";
const char* OTAName = "Dummy";
const char* OTAPassword = "lemons";

ESP8266WebServer server(80);
File fsUploadFile;


//@fold-children

//DEFAULT Angles (Rest Position):
//0: 1°   1: 1°   2: 90°
//3: 65°  4: 70°  5: 1°
//Angles for all servos
int defPos[] = {1, 1, 95, 65, 70, 1, 1};
int pos[] = {1, 1, 95, 65, 70, 1, 1};
int servoCount = 7;
int speedVal = 5; //Delay in ms
int defSpeed = 5;
int mil = 20; //Repeat delay for handtracking

String preset1 = "/js/teach1.dummy";
String preset2 = "/js/teach2.dummy";
String preset3 = "/js/teach3.dummy";

void printPos(){
  String ang = "";
  for(int i = 0; i<servoCount; i++){
    ang += String(pos[i]) + " ";
    yield();
  }
  Serial.println("Pos: " + ang);
}

void saveAngles(){
  File f = SPIFFS.open("/js/angles.dummy", "w+");
  if(!f) Serial.println("file open failed!");
  Serial.println("Saving Angles to file...");


  for(int i = 0; i<servoCount; i++){
    f.println(pos[i]);
    yield();
  }
  printPos();
  f.close();
}

void getAngles(){
  File f = SPIFFS.open("/js/angles.dummy", "r");
  if(!f) Serial.println("file open failed!");
  Serial.println("Reading Angles from file");
  //Serial.println(f.readString());

  for(int i = 0; i < servoCount; i++){
    int parse = f.parseInt();
    pos[i] = parse;
    yield();
  }
  f.close();
}



Ticker t0;
Ticker t1;
Ticker t2;
Ticker t3;
Ticker t4;
Ticker t5;
Ticker t6;
Ticker timer1;
Ticker timer2;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

	switch(type) {
		case WStype_DISCONNECTED:
			Serial.printf("[WSc] Disconnected!\n");
			break;
		case WStype_CONNECTED: {
			Serial.printf("[WSc] Connected to url: %s\n", payload);
      Serial.println("[WSc] Connected!");

			// send message to server when Connected
			webSocket.sendTXT("Connected");
		}
			break;
		case WStype_TEXT:
			Serial.printf("[WSc] get text: %s\n", payload);

			// send message to server
			// webSocket.sendTXT("message here");
			break;
		case WStype_BIN:
			Serial.printf("[WSc] get binary length: %u\n", length);
			hexdump(payload, length);

			// send data to server
			// webSocket.sendBIN(payload, length);
			break;
	}

}

void connectWS(){
  // server address, port and URL
	webSocket.begin("ws://localhost:6437/", 6437, "");

	// event handler
	webSocket.onEvent(webSocketEvent);

	// use HTTP Basic Authorization this is optional remove if not needed
	//webSocket.setAuthorization("user", "Password");

	// try ever 5000 again if connection has failed
	webSocket.setReconnectInterval(5000);
}

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
  connectWS();


  t0.stop();
  t1.stop();
  t2.stop();
  t3.stop();
  t4.stop();
  t5.stop();
  t6.stop();

  //saveTimer.start();

  getAngles();
  adjust();


  nod();

  test();

  //no();

}
//Override map() to fit doubles
double MAP(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//Converts pulse to angle
int angle(double pulse, int servo = 0, int min = SERVOMIN, int max = SERVOMAX){

  if(servo<=3){
    return round(MAP(pulse, min, max, 1, 180));
  }
  else if(servo==4){
    return round(MAP(pulse, SERVOMIN4, SERVOMAX4, 1, 180));
  }
  else if(servo==5){
    return round(MAP(pulse, SERVOMIN5, SERVOMAX5, 1, 180));
  }
  else if(servo==6){
    return round(MAP(pulse, SERVOMIN6, SERVOMAX6, 1, 180));
  }

}


//Converts deg (0-180) to pulsewidth between min and max
double pulse(int deg, int servo = 0, int min = SERVOMIN, int max = SERVOMAX){
  if(servo<=3){
    return round(MAP(deg, 1, 180, min, max));
  }
  else if(servo==4){
    return round(MAP(deg, 1, 180, SERVOMIN4, SERVOMAX4));
  }
  else if(servo==5){
    return round(MAP(deg, 1, 180, SERVOMIN5, SERVOMAX5));
  }
  else if(servo==6){
    return round(MAP(deg, 1, 180, SERVOMIN6, SERVOMAX6));
  }
}

double curveMult(double step){
  //return exp(5*pow((step-1), 3));
  return exp(6*pow((step-1), 3));

}


//Servo Classes for moving (Shit Stuff for Ticker)
// As the callback function is a function ptr it has to be a static class function
// which can only use static class variables, so every servo needs a separately declared class
// Static stuff doesnt get inherited and therefore cant be changed for each derived class...
class servo0 {
  public:
    static const int servo = 0;
    static int i;
    static double startPulse, diff, diffAbs;

    static void move(){
      double step = i++/diffAbs;
      double mult = curveMult(step);
      int newPulse = round(mult * diff) + startPulse;
      //Serial.println(newPulse);
      pwm.setPWM(servo, 0, newPulse);
      pos[servo] = angle(newPulse, servo);
      if(i==diffAbs+1){ //Move finished
        saveAngles();
        t0.stop();
      }
      yield();
    }
};
int servo0::i = 0;
double servo0::startPulse = 0;
double servo0::diff = 0;
double servo0::diffAbs = 0;

class servo1 {
  public:
    static const int servo = 1;
    static int i;
    static double startPulse, diff, diffAbs;

    static void move(){
      double step = i++/diffAbs;
      double mult = curveMult(step);
      int newPulse = round(mult * diff) + startPulse;
      //Serial.println(newPulse);
      pwm.setPWM(servo, 0, newPulse);
      pos[servo] = angle(newPulse, servo);
      if(i==diffAbs+1){
        t1.stop();
        saveAngles();
      }

      yield();
    }
};
int servo1::i = 0;
double servo1::startPulse = 0;
double servo1::diff = 0;
double servo1::diffAbs = 0;

class servo2 {
  public:
    static const int servo = 2;
    static int i;
    static double startPulse, diff, diffAbs;

    static void move(){
      double step = i++/diffAbs;
      double mult = curveMult(step);
      int newPulse = round(mult * diff) + startPulse;
      //Serial.println(newPulse);
      pwm.setPWM(servo, 0, newPulse);
      pos[servo] = angle(newPulse, servo);
      if(i==diffAbs+1){
        t2.stop();
        saveAngles();
      }

      yield();
    }
};
int servo2::i = 0;
double servo2::startPulse = 0;
double servo2::diff = 0;
double servo2::diffAbs = 0;

class servo3 {
  public:
    static const int servo = 3;
    static int i;
    static double startPulse, diff, diffAbs;

    static void move(){
      double step = i++/diffAbs;
      double mult = curveMult(step);
      int newPulse = round(mult * diff) + startPulse;
      //Serial.println(newPulse);
      pwm.setPWM(servo, 0, newPulse);
      pos[servo] = angle(newPulse, servo);
      if(i==diffAbs+1){
        t3.stop();
        saveAngles();
      }

      yield();
    }
};
int servo3::i = 0;
double servo3::startPulse = 0;
double servo3::diff = 0;
double servo3::diffAbs = 0;

class servo4 {
  public:
    static const int servo = 4;
    static int i;
    static double startPulse, diff, diffAbs;

    static void move(){
      double step = i++/diffAbs;
      double mult = curveMult(step);
      int newPulse = round(mult * diff) + startPulse;
      //Serial.println(newPulse);
      pwm.setPWM(servo, 0, newPulse);
      pos[servo] = angle(newPulse, servo);
      yield();

      if(i==diffAbs+1){
        t4.stop();
        saveAngles();
      }
    }
};
int servo4::i = 0;
double servo4::startPulse = 0;
double servo4::diff = 0;
double servo4::diffAbs = 0;

class servo5 {
  public:
    static const int servo = 5;
    static int i;
    static double startPulse, diff, diffAbs;

    static void move(){
      double step = i++/diffAbs;
      double mult = curveMult(step);
      int newPulse = round(mult * diff) + startPulse;
      //Serial.println(newPulse);
      pwm.setPWM(servo, 0, newPulse);
      pos[servo] = angle(newPulse, servo);
      if(i==diffAbs+1){
        t5.stop();
        saveAngles();
      }

      yield();
    }
};
int servo5::i = 0;
double servo5::startPulse = 0;
double servo5::diff = 0;
double servo5::diffAbs = 0;

class servo6 {
  public:
    static const int servo = 6;
    static int i;
    static double startPulse, diff, diffAbs;

    static void move(){
      double step = i++/diffAbs;
      double mult = curveMult(step);
      int newPulse = round(mult * diff) + startPulse;
      //Serial.println(newPulse);
      pwm.setPWM(servo, 0, newPulse);
      pos[servo] = angle(newPulse, servo);
      if(i==diffAbs+1){
        t5.stop();
        saveAngles();
      }

      yield();
    }
};
int servo6::i = 0;
double servo6::startPulse = 0;
double servo6::diff = 0;
double servo6::diffAbs = 0;





bool checkColl(int servo, int angle){
  int diffmin = 95;     //Difference Servo 1 to Servo 2: 95°
  int diffmax = 183;

  switch(servo){
    case 1:{
      if((angle < 20 && pos[2] > 125)) return false;
      else if((angle >= (diffmin - pos[2]-1) && angle <= diffmax - pos[2])) return true;
      else {
        Serial.println("Collision Warning!");

        return false;
      }
    }
    case 2: { //TO DO COLLLIIISSSSIIIIOOON DETECTION!!!
      //Difference to Servo 1: 94°
      if((pos[1] < 20 && angle > 125)) return false;
      else if((angle >= (diffmin - pos[1]-1) && angle <= diffmax - pos[1])) return true; //Fix Servo 2 hitting board
      else {
        Serial.println("Collision Warning!");

        return false;
      }
    default: return true;
    }
  }
  yield();
}

void setPosHard(int servo, int angle){
  if(angle<=180 && angle>=1){
    pwm.setPWM(servo, 0, pulse(angle, servo));
    pos[servo] = angle;
    //saveAngles();
  }

}

bool setPos(int servo, int angle, int speed = speedVal, int minSteps=200){
  double startPulse = pulse(pos[servo], servo);
  double diff = pulse(angle, servo)-startPulse;
  double diffAbs = abs(diff);
  if(diffAbs==0){
    setPosHard(servo, angle);
    return true;
  }
  if(diffAbs < minSteps){
    diffAbs = minSteps;
  }

  //Serial.print(checkColl(servo, angle));

  yield();
 switch(servo){
   case 0: {  t0.stop();
              servo0::i = 1;
              servo0::startPulse = startPulse;
              servo0::diff = diff;
              servo0::diffAbs = diffAbs;
              t0.setCallback(servo0::move);
              t0.setInterval(speed);
              t0.setRepeats(diffAbs);
              t0.start();
              return true;
           }
   case 1: {
              t1.stop();
              servo1::i = 1;
              servo1::startPulse = startPulse;
              servo1::diff = diff;
              servo1::diffAbs = diffAbs;
              t1.setCallback(servo1::move);
              t1.setInterval(speed);
              t1.setRepeats(diffAbs);
              t1.start();
              return true;
           }
   case 2: {
              t2.stop();
              servo2::i = 1;
              servo2::startPulse = startPulse;
              servo2::diff = diff;
              servo2::diffAbs = diffAbs;
              t2.setCallback(servo2::move);
              t2.setInterval(speed);
              t2.setRepeats(diffAbs);
              t2.start();
              return true;
           }
   case 3: {  t3.stop();
              servo3::i = 1;
              servo3::startPulse = startPulse;
              servo3::diff = diff;
              servo3::diffAbs = diffAbs;
              t3.setCallback(servo3::move);
              t3.setInterval(speed);
              t3.setRepeats(diffAbs);
              t3.start();
              return true;
           }
   case 4: {  t4.stop();
              servo4::i = 1;
              servo4::startPulse = startPulse;
              servo4::diff = diff;
              servo4::diffAbs = diffAbs;
              t4.setCallback(servo4::move);
              t4.setInterval(speed);
              t4.setRepeats(diffAbs);
              t4.start();
              return true;
           }
   case 5: {  t5.stop();
              servo5::i = 1;
              servo5::startPulse = startPulse;
              servo5::diff = diff;
              servo5::diffAbs = diffAbs;
              t5.setCallback(servo5::move);
              t5.setInterval(speed);
              t5.setRepeats(diffAbs);
              t5.start();
              return true;
           }
   case 6: {  t6.stop();
              servo6::i = 1;
              servo6::startPulse = startPulse;
              servo6::diff = diff;
              servo6::diffAbs = diffAbs;
              t6.setCallback(servo6::move);
              t6.setInterval(speed);
              t6.setRepeats(diffAbs);
              t6.start();
              return true;
           }
  }


  //pos[servo] = angle;

}

bool checkServo(int servo){
  switch(servo){
    case 0:
      if(t0.getState()==1) return false;
      else return true;
    case 1:
      if(t1.getState()==1) return false;
      else return true;
    case 2:
      if(t2.getState()==1) return false;
      else return true;
    case 3:
      if(t3.getState()==1) return false;
      else return true;
    case 4:
      if(t4.getState()==1) return false;
      else return true;
    case 5:
      if(t5.getState()==1) return false;
      else return true;
    case 6:
      if(t6.getState()==1) return false;
      else return true;

  }
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
  SPIFFS.begin();

  Serial.println("SPIFFS started.");         // Start the SPI Flash File System (SPIFFS)
  {
    Serial.println("Contents: ");
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
  server.serveStatic("/angles", SPIFFS, "/js/angles.dummy", "max-age=86400");
  server.serveStatic("/teach1", SPIFFS, "/js/teach1.dummy", "max-age=86400");
  server.serveStatic("/teach2", SPIFFS, "/js/teach2.dummy", "max-age=86400");
  server.serveStatic("/teach3", SPIFFS, "/js/teach3.dummy", "max-age=86400");


  server.on("/servo0", HTTP_POST, []() {
    String value = server.arg("value");
    if(checkColl(0, value.toInt())) setPos(0, value.toInt());
    Serial.println("Received Servo0 to " + value);
  });
  server.on("/servo1", HTTP_POST, []() {
    String value = server.arg("value");
    if(checkColl(1, value.toInt())) setPos(1, value.toInt());
    Serial.println("Received Servo1 to " + value);
  });
  server.on("/servo2", HTTP_POST, []() {
    String value = server.arg("value");
    if(checkColl(2, value.toInt())) setPos(2, value.toInt());
    Serial.println("Received Servo2 to " + value);
  });
  server.on("/servo3", HTTP_POST, []() {
    String value = server.arg("value");
    if(checkColl(3, value.toInt())) setPos(3, value.toInt());
    Serial.println("Received Servo3 to " + value);
  });
  server.on("/servo4", HTTP_POST, []() {
    String value = server.arg("value");
    if(checkColl(4, value.toInt())) setPos(4, value.toInt());
    Serial.println("Received Servo4 to " + value);
  });
  server.on("/servo5", HTTP_POST, []() {
    String value = server.arg("value");
    if(checkColl(5, value.toInt())) setPos(5, value.toInt());
    Serial.println("Received Servo5 to " + value);
  });
  server.on("/servo6", HTTP_POST, []() {
    String value = server.arg("value");
    setPos(6, value.toInt());
    Serial.println("Received Servo6 to " + value);
  });


  server.on("/speed", HTTP_POST, []() {
    String value = server.arg("value");
    setSpeed(value.toInt());

  });




  server.on("/reset", HTTP_POST, []() {
    resetPos();
    Serial.println("Received reset");
  });
  server.on("/resetSpeed", HTTP_POST, []() {
    resetSpeed();
    Serial.println("Received reset Speed");
  });
  server.on("/nod", HTTP_POST, []() {
    nod();
    Serial.println("Received nod");
  });
  server.on("/no", HTTP_POST, []() {
    no();
    Serial.println("Received no");
  });

  //Teach and play
  server.on("/save1", HTTP_POST, []() {
    Serial.println("Saveing new Pos to 1");
    save(preset1);
  });
  server.on("/play1", HTTP_POST, []() {
    Serial.println("Playing preset 1");
    play(preset1);
    Serial.println("Preset 1 finished");
  });
  server.on("/clear1", HTTP_POST, []() {
    Serial.println("Resetting preset 1");
    clear(preset1);
  });

  server.on("/save2", HTTP_POST, []() {
    Serial.println("Saveing new Pos to 2");
    save(preset2);
  });
  server.on("/play2", HTTP_POST, []() {
    Serial.println("Playing preset 2");
    play(preset2);
    Serial.println("Preset 2 finished");
  });
  server.on("/clear2", HTTP_POST, []() {
    Serial.println("Resetting preset 2");
    clear(preset2);
  });

  server.on("/save3", HTTP_POST, []() {
    Serial.println("Saveing new Pos to 3");
    save(preset3);
  });
  server.on("/play3", HTTP_POST, []() {
    Serial.println("Playing preset 3");
    play(preset3);
    Serial.println("Preset 3 finished");
  });
  server.on("/clear3", HTTP_POST, []() {
    Serial.println("Resetting preset 3");
    clear(preset3);
  });




  server.onNotFound(handleNotFound);

  server.begin();
  Serial.print("HTTP server started @ ");
  Serial.println(WiFi.localIP());

}

void test(){
  //TESTS


}

void wait(int servo){
  while(!checkServo(servo)) {
    timerUpdate();
    yield();
  }
}

void waitAll(){
  Serial.print("Waiting");
  for(int i = 0; i < servoCount; i++){
    yield();
    wait(i);
    Serial.print(".");

  }
}

void nod(){
  setPos(4, pos[4]+20, 5, 50);
  wait(4);
  setPos(4, pos[4]-20, 5, 50);
  wait(4);
}

void no(){
  setPos(0, pos[0]+10, 5, 50);
  wait(0);
  setPos(0, pos[0]-10, 5, 50);
  wait(0);
}

void save(String file){
  File f = SPIFFS.open(file, "a");
  if(!f) Serial.println("file open failed!");

  for(int i = 0; i<servoCount; i++){
    f.print(pos[i]);
    f.print(", ");
    yield();
  }
  f.println("");
  f.close();
  Serial.println("Saved");

}

void play(String file){
  File f = SPIFFS.open(file, "r");
  if(!f) Serial.println("file open failed!");
  if(f.peek()==EOF){
    Serial.println("Invalid or empty Preset");
    return;
  }
  //Serial.println(f.readString());
  int parsed;
  int buffer[servoCount];
  while(parsed!=0){
    for(int i = 0; i < servoCount; i++){
      parsed = f.parseInt();
      if(parsed==0)break;
      buffer[i] = parsed;
      yield();
      Serial.print(parsed);
      Serial.print(", ");
    }
    yield();
    for(int i = 0; i< servoCount; i++){
      setPos(i, buffer[i]);
    }
    waitAll();
    //saveAngles();
  }

  f.close();

}

void clear(String file){
  File f = SPIFFS.open(file, "w+");
  if(!f) Serial.println("file open failed!");
  f.close();
  Serial.println("Cleared!");
}

void resetPos() {
  Serial.print("Resetting...");
  for(int i = 0; i < servoCount ; i++){
    setPos(i, defPos[i]);
  }


}

void setSpeed(int speed){
  speedVal = speed;
  Serial.println("Speed: " + String(speedVal));

}

void resetSpeed(){
  Serial.println("Resetting Speed...");
  setSpeed(defSpeed);
}

void resetArr(){
  for(int i = 0; i< servoCount; i++){
    pos[i] = defPos[i];
  }
  Serial.println("Resetted Pos[]");
}

void checkArr(){
  for(int i = 0; i< servoCount; i++){
    if(pos[i]>180 || pos[i] < 1){
      resetArr();
      resetPos();
      timer1.stop();
      timer2.stop();
      //Serial.println("Corrected Pos Array and resetted");
    }
  }

}

void adjust() {
  Serial.println("Adjusting...");
  for(int i = 0; i< servoCount; i++){
    checkArr();
    setPos(i, pos[i]);
    setSpeed(speedVal);
  }
}

void left(){
  setPosHard(0, pos[0]-1);
}

void right(){
  setPosHard(0, pos[0]+1);
}

void up(){
  setPosHard(4, pos[4]+1);
}

void down(){
  setPosHard(4, pos[4]-1);
}

void turn(String dir){

  if(dir=="l"){
    timer1.pause();
    timer1.setCallback(left);
    timer1.setInterval(mil);
    timer1.start();
    Serial.println("Received l");
  }
  else if(dir=="r"){
    timer1.pause();
    timer1.setCallback(right);
    timer1.setInterval(mil);
    timer1.start();
    Serial.println("Received r");
  }
  else if(dir=="u"){
    timer2.pause();
    timer2.setCallback(up);
    timer2.setInterval(mil);
    timer2.start();
    Serial.println("Received u");
  }
  else if(dir=="d"){
    timer2.pause();
    timer2.setCallback(down);
    timer2.setInterval(mil);
    timer2.start();
    Serial.println("Received d");
  }
  else if(dir=="S1"){
    timer1.stop();
    saveAngles();
    Serial.println("Received S1");
  }
  else if(dir=="S2"){
    timer2.stop();
    saveAngles();
    Serial.println("Received S2");
  }


}

void checkInput(String input){
  if(input=="reset"){
    resetPos();
    Serial.println(input);
  }
  else if(input=="resetSpeed"){
    resetSpeed();
    //Serial.println(input);
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
  else if(input=="save1") save(preset1);
  else if(input=="save2") save(preset2);
  else if(input=="save3") save(preset3);
  else if(input=="play1") play(preset1);
  else if(input=="play2") play(preset2);
  else if(input=="play3") play(preset3);
  else if(input=="clear1") clear(preset1);
  else if(input=="clear2") clear(preset2);
  else if(input=="clear3") clear(preset3);

  else if(input.substring(0, 1)=="l"){
    turn("l");
    if(input.length()>1 && input.substring(1, 2)=="u") turn("u");
    else if(input.length()>1 && input.substring(1, 2)=="d") turn("d");
  }
  else if(input.substring(0, 1)=="r" && input!="reset"){
    turn("r");
    if(input.length()>1 && input.substring(1, 2)=="u") turn("u");
    else if(input.length()>1 && input.substring(1, 2)=="d") turn("d");
  }
  else if(input.substring(0, 1)=="u"){
    turn("u");
  }
  else if(input.substring(0, 1)=="d"){
    turn("d");
  }
  else if(input.substring(0, 2)=="S1"){
    turn("S1");
    if(input.length()>2 && input.substring(2, 4)=="S2") turn("S2");
  }
  else if(input.substring(0, 2)=="S2"){
    turn("S2");
  }


  else if(input.substring(0, 5)=="speed"){
    setSpeed(input.substring(6).toInt());

  }
  // e.g. "5 145"
  else if(input.length()<=5){

    servonum = input.substring(0, 1).toInt();
    int angle = input.substring(2).toInt();

    if(!checkColl(servonum, angle)) return;

    setPos(servonum, input.substring(2).toInt());

  }
  else{
    Serial.println(input);
  }
}

void timerUpdate(){
  t0.update();
  t1.update();
  t2.update();
  t3.update();
  t4.update();
  t5.update();
  t6.update();
  timer1.update();
  timer2.update();
  yield();
}

void loop() {
  server.handleClient();
  ArduinoOTA.handle();
  webSocket.loop();
  timerUpdate();
  checkArr();



  if(Serial.available()!=0){
    String input = Serial.readString();
    checkInput(input);
  }

}




//!fold-children
