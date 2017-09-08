import gab.opencv.*; //<>//
import java.awt.Rectangle;
import controlP5.*;
import processing.serial.*;
import de.voidplus.leapmotion.*;


Serial serial;

ControlP5 cp5;
LeapMotion leap;
OpenCV opencv;
Rectangle[] faces;


//Variables
Textlabel textarea;
Canvas canvas;

int servoCount = 7;
int pos[] = {1, 1, 95, 65, 70, 1, 1};
int speed = 5;

String curr = " ";
String port = "/dev/ttyUSB0";

int t = 0;

boolean right = false;
boolean left = false;
boolean up = false;
boolean down = false;

void setup() {
  size(700, 700);
  //fullScreen();

 
  opencv = new OpenCV(this, 300, 150);
  opencv.loadCascade(OpenCV.CASCADE_FRONTALFACE);
  //opencv.setROI(20, 500, 300, 150);
  
  
  try{
    serial = new Serial(this, port, 9600);
    serial.bufferUntil(10);
    leap = new LeapMotion(this).allowGestures();
    
  }finally{}
  
  
  //serial.buffer(100);

  PFont font = createFont("arial", 20);

  cp5 = new ControlP5(this);

  cp5.addLabel("DUMMY CONTROLLER")
    .setPosition(250, 20)
    .setSize(200, 40)
    .setFont(createFont("arial", 20))
    ;

  cp5.addScrollableList("Ports")
    .setPosition(600, 50)
    .addItems(serial.list())
    .setOpen(false)
    ;
  cp5.addButton("Connect")
    .setPosition(620, 20)
    .setSize(60, 20)
    .setFont(createFont("arial", 10))
    ;

  cp5.addTextfield("input")
    .setPosition(20, 20)
    .setSize(200, 40)
    .setFont(font)
    .setFocus(true)
    .setColor(color(255, 0, 0))
    ;

  cp5.addSlider("Servo0")
    .setPosition(20, 100)
    .setSize(360, 20)
    .setFont(font)
    .setValue(pos[0])
    .setRange(1, 180)
    .setColorTickMark(0)
    .setDecimalPrecision(0)
    ;
  cp5.addSlider("Servo1")
    .setPosition(20, 130)
    .setSize(360, 20)
    .setFont(font)
    .setValue(pos[1])
    .setRange(1, 180)
    .setColorTickMark(0)
    .setDecimalPrecision(0)
    ;

  cp5.addSlider("Servo2")
    .setPosition(20, 160)
    .setSize(360, 20)
    .setFont(font)
    .setValue(pos[2])
    .setRange(1, 180)
    .setColorTickMark(0)
    .setDecimalPrecision(0)
    ;

  cp5.addSlider("Servo3")
    .setPosition(20, 190)
    .setSize(360, 20)
    .setFont(font)
    .setValue(pos[3])
    .setRange(1, 180)
    .setColorTickMark(0)
    .setDecimalPrecision(0)
    ;

  cp5.addSlider("Servo4")
    .setPosition(20, 220)
    .setSize(360, 20)
    .setFont(font)
    .setValue(pos[4])
    .setRange(1, 180)
    .setColorTickMark(0)
    .setDecimalPrecision(0)
    ;

  cp5.addSlider("Servo5")
    .setPosition(20, 250)
    .setSize(360, 20)
    .setFont(font)
    .setValue(pos[5])
    .setRange(1, 180)
    .setColorTickMark(0)
    .setDecimalPrecision(0)
    ;

  cp5.addSlider("Servo6")
    .setPosition(20, 280)
    .setSize(360, 20)
    .setFont(font)
    .setValue(pos[6])
    .setRange(1, 180)
    .setColorTickMark(0)
    .setDecimalPrecision(0)
    ;

  cp5.addSlider("Speed")
    .setPosition(20, 320)
    .setSize(360, 20)
    .setFont(font)
    .setValue(speed)
    .setRange(1, 15)
    .setColorTickMark(0)
    .setDecimalPrecision(0)
    ;

  cp5.addButton("nod")
    .setPosition(500, 100)
    .setFont(createFont("arial", 15))
    ;

  cp5.addButton("no")
    .setPosition(500, 130)
    .setFont(createFont("arial", 15))
    ;

  cp5.addButton("reset")
    .setPosition(500, 160)
    .setFont(createFont("arial", 15))
    .setColorBackground(color(150, 0, 0))
    .setColorActive(color(255, 0, 0))
    .setColorForeground(color(200, 0, 0))
    ;

  cp5.addButton("resetSpeed")
    .setPosition(500, 320)
    .setFont(createFont("arial", 10))
    ;

  cp5.addLabel("Preset 1")
    .setPosition(20, 370)
    .setFont(createFont("arial", 15))
    ;
  cp5.addButton("save1")
    .setPosition(20, 400)
    .setFont(createFont("arial", 15))
    ;
  cp5.addButton("play1")
    .setPosition(20, 430)
    .setFont(createFont("arial", 15))
    ;
  cp5.addButton("clear1")
    .setPosition(20, 460)
    .setFont(createFont("arial", 15))
    ;

  cp5.addLabel("Preset 2")
    .setPosition(100, 370)
    .setFont(createFont("arial", 15))
    ;
  cp5.addButton("save2")
    .setPosition(100, 400)
    .setFont(createFont("arial", 15))
    ;
  cp5.addButton("play2")
    .setPosition(100, 430)
    .setFont(createFont("arial", 15))
    ;
  cp5.addButton("clear2")
    .setPosition(100, 460)
    .setFont(createFont("arial", 15))
    ;

  cp5.addLabel("Preset 3")
    .setPosition(180, 370)
    .setFont(createFont("arial", 15))
    ;
  cp5.addButton("save3")
    .setPosition(180, 400)
    .setFont(createFont("arial", 15))
    ;
  cp5.addButton("play3")
    .setPosition(180, 430)
    .setFont(createFont("arial", 15))
    ;
  cp5.addButton("clear3")
    .setPosition(180, 460)
    .setFont(createFont("arial", 15))
    ;

  textarea = cp5.addTextlabel("SerialRead")
    .setPosition(500, 370)
    .setSize(200, 400)
    .setLineHeight(10)

    ;
   
 
  textFont(font);
  setSliders();
}



void draw() {
  background(0);
  fill(255);
  checkSerial();
  
  
  //leapImages();
  opencvFace();
  
  /*if(millis()>t){
    t = millis() + 250;
    trackHand();
  }*/
  //trackHand();

}

void opencvFace(){
  if (leap.hasImages()) {
    for (Image camera : leap.getImages()) {
      if (camera.isLeft()) {
        // left camera
        camera.resize(300, 150);
        opencv.loadImage(camera);
        image(opencv.getInput(), 20, 500, 300, 150);
        
      } else {
        // right camera
        image(camera, 330, 500, 300, 150);
      }
    }
  }
  
  //BRIGHT SPOT DETECTION
  /*
  PVector loc = opencv.max();
  
  stroke(255, 0, 0);
  strokeWeight(4);
  noFill();
  ellipse(loc.x+20, loc.y+500, 5, 5);
*/
  
  //FACE DETECTION
  faces = opencv.detect();

  noFill();
  stroke(0, 255, 0);
  strokeWeight(3);
  
  for (int i = 0; i < faces.length; i++) {
    rect(faces[i].x + 20, faces[i].y + 500, faces[i].width, faces[i].height);
  }
  
  
}

void leapImages(){
  if (leap.hasImages()) {
    for (Image camera : leap.getImages()) {
      if (camera.isLeft()) {
        // left camera
        image(camera, 20, 500, 300, 150);
      } else {
        // right camera
        image(camera, 330, 500, 300, 150);
      }
    }
  }
}

void trackHand(){
 
 for(Hand hand : leap.getHands()){
   PVector handPosition       = hand.getPosition();
   float   handGrab           = hand.getGrabStrength();
   if(hand.isRight()){
     float x = handPosition.x;
     float y = handPosition.y;
     float z = handPosition.z;
     if(handGrab == 1.0){
       print("Grabbed");
       if(right || left){
         send("S1");
       }
       right = false;
       left = false;
       
       if(up || down){
       send("S2");
       }
       up = false;
       down = false;
     }
     else{ //Only print while hand not fully grabbed
       println("X: " + x);
       println("Z: " + z);
       
       if(x>400.0){
         println("Hand right");
         if(!right){
           send("r");
           right = true;
           left = false;
         }
       }
       else if(x<300.0){
         println("Hand left");
         if(!left){
           send("l");
           left = true;
           right = false;
         }
       }
       else{
         if(right || left) send("S1");
         right = false;
         left = false;
       }
       
       if(z>60.0){
         println("Hand up");
         if(!up){
           send("u");
           up = true;
           down = false;
         }
       }
       else if(z<40.0){
         println("Hand down");
         if(!down){
           send("d");
           down = true;
           up = false;
         }
       }
       else{
        if(up || down) send("S2");
        up = false;
        down = false;
       }
     }
   }
 }
}

// ======================================================
// 1. Swipe Gesture

void leapOnSwipeGesture(SwipeGesture g, int state){
  int     id               = g.getId();
  Finger  finger           = g.getFinger();
  PVector position         = g.getPosition();
  PVector positionStart    = g.getStartPosition();
  PVector direction        = g.getDirection();
  float   speed            = g.getSpeed();
  long    duration         = g.getDuration();
  float   durationSeconds  = g.getDurationInSeconds();

  switch(state){
    case 1: // Start
      break;
    case 2: // Update
      break;
    case 3: // Stop
      println("SwipeGesture: " + id);
      //send("reset");
      break;
  }
}


// ======================================================
// 2. Circle Gesture

void leapOnCircleGesture(CircleGesture g, int state){
  int     id               = g.getId();
  Finger  finger           = g.getFinger();
  PVector positionCenter   = g.getCenter();
  float   radius           = g.getRadius();
  float   progress         = g.getProgress();
  long    duration         = g.getDuration();
  float   durationSeconds  = g.getDurationInSeconds();
  int     direction        = g.getDirection();

  switch(state){
    case 1: // Start
      break;
    case 2: // Update
      break;
    case 3: // Stop
      println("CircleGesture: " + id);
      break;
  }

  switch(direction){
    case 0: // Anticlockwise/Left gesture
      break;
    case 1: // Clockwise/Right gesture
      break;
  }
}


// ======================================================
// 3. Screen Tap Gesture

void leapOnScreenTapGesture(ScreenTapGesture g){
  int     id               = g.getId();
  Finger  finger           = g.getFinger();
  PVector position         = g.getPosition();
  PVector direction        = g.getDirection();
  long    duration         = g.getDuration();
  float   durationSeconds  = g.getDurationInSeconds();

  println("ScreenTapGesture: " + id);
}


// ======================================================
// 4. Key Tap Gesture

void leapOnKeyTapGesture(KeyTapGesture g){
  int     id               = g.getId();
  Finger  finger           = g.getFinger();
  PVector position         = g.getPosition();
  PVector direction        = g.getDirection();
  long    duration         = g.getDuration();
  float   durationSeconds  = g.getDurationInSeconds();

  println("KeyTapGesture: " + id);
  send("nod");
  print("Position: " + position);
}

void getAngles(String array) {
  String buffer[] = array.split(" ");
  try{
    for (int i = 0; i < servoCount; i++) {
      try{pos[i] = Integer.parseInt(buffer[i]);}finally{}
      
    }
    for (int i = 0; i < servoCount; i++) {
      if(pos[i] != (int) cp5.get("Servo" + i).getValue()){
        setSliders();
        break;
      }
      
    }
  }finally{}
  
}

boolean checkMouseOver(String name){
  return cp5.isMouseOver(cp5.get(name));
}

void setSliders() {
  for (int i = 0; i < servoCount; i++) {
    cp5.get("Servo" + Integer.toString(i)).setValue(pos[i]);
  }
    
}

void setSpeed(String string) {
  String buffer[] = string.split("\n");
  speed = Integer.valueOf(buffer[0].trim());
  //print(buffer[0]);
  //print("NEWSPEED: " + speed);
  if(speed != (int) cp5.get("Speed").getValue()) cp5.get("Speed").setValue(speed);
}

void checkSerial() { //Problems with StringIndex -1
  if (serial.available()>0) {
    String string = serial.readString(); //10 = linefeed in ascii
    print(string);

    if (string.matches("\\A\\p{ASCII}*\\z") && string!= "") printArea(string);
    
  }
}

void getUpdate(String curr){
  if ((curr).lastIndexOf("Pos: ")!=-1) {
    int index = (curr).lastIndexOf("Pos: ");
    //print(string.substring(index+5));
    if ((curr).substring(index+5).length()>=16) {
  
      getAngles((curr).substring(index+5));
    }
  }
  if ((curr).lastIndexOf("Speed: ")!=-1) {
    int index = (curr).lastIndexOf("Speed: ");
    //print((curr+text).substring(index+7));
    setSpeed((curr).substring(index+7));
    
  }
}

void printArea(String text) {
  textarea.setValue(curr + text);
  getUpdate(curr+text);
  if(curr.length() > 150)curr = text;
  else curr = curr + text;
  
  
}

//CallbackListener callback = new CallbackListener()

void Ports(int index){
    port = cp5.get(ScrollableList.class, "Ports").getItem(index).get("name").toString();
}

void controlEvent(ControlEvent theEvent) {
  String name = theEvent.getName();

  if (theEvent.getName()=="input") {
    send(theEvent.getStringValue());
  }
  
  if (theEvent.isAssignableFrom(Button.class) && theEvent.getName()!="Connect") {
    send(name);
  }
  else if(theEvent.getName()=="Connect"){
    
    try{
      serial.stop();
      serial = new Serial(this, port, 9600);
    }
    finally{}
  }
}

void mouseReleased(){
  for(int i = 0; i<servoCount; i++){
    int value = (int) cp5.get("Servo"+i).getValue();
    if(value != pos[i] && checkMouseOver("Servo"+i)) send(i + " " + value);
  }
  int value = (int) cp5.get("Speed").getValue();
  if(value != speed && checkMouseOver("Speed")) send("speed " + value);
}



public void send(String text) {
  serial.write(text);
  println("Sent: " + text);
}