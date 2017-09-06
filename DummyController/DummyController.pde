
import controlP5.*;
import processing.serial.*;

Serial serial;

ControlP5 cp5;

//Variables
Textlabel textarea;


void setup() {
  size(700,700);
  //fullScreen();
  
  String port = "/dev/ttyUSB0";
  serial = new Serial(this, port, 9600);
  //serial.buffer(10);
  
  PFont font = createFont("arial",20);
  
  cp5 = new ControlP5(this);
  
  cp5.addLabel("DUMMY CONTROLLER")
      .setPosition(250, 20)
      .setSize(200, 40)
      .setFont(createFont("arial", 20))
      ;
  
  cp5.addTextfield("input")
     .setPosition(20,20)
     .setSize(200,40)
     .setFont(font)
     .setFocus(true)
     .setColor(color(255,0,0))
     ;
     
  cp5.addSlider("Servo0")
     .setPosition(20,100)
     .setSize(360,20)
     .setFont(font)
     .setRange(1, 180)
     .setColorTickMark(0)
     .setDecimalPrecision(0)
     ;
  cp5.addSlider("Servo1")
     .setPosition(20,130)
     .setSize(360,20)
     .setFont(font)
     .setRange(1, 180)
     .setColorTickMark(0)
     .setDecimalPrecision(0)
     ;
     
  cp5.addSlider("Servo2")
     .setPosition(20,160)
     .setSize(360,20)
     .setFont(font)
     .setRange(1, 180)
     .setColorTickMark(0)
     .setDecimalPrecision(0)
     ;
      
  cp5.addSlider("Servo3")
     .setPosition(20,190)
     .setSize(360,20)
     .setFont(font)
     .setRange(1, 180)
     .setColorTickMark(0)
     .setDecimalPrecision(0)
     ;
 
  cp5.addSlider("Servo4")
     .setPosition(20,220)
     .setSize(360,20)
     .setFont(font)
     .setRange(1, 180)
     .setColorTickMark(0)
     .setDecimalPrecision(0)
     ;

  cp5.addSlider("Servo5")
     .setPosition(20,250)
     .setSize(360,20)
     .setFont(font)
     .setRange(1, 180)
     .setColorTickMark(0)
     .setDecimalPrecision(0)
     ;
     
  cp5.addSlider("Servo6")
     .setPosition(20,280)
     .setSize(360,20)
     .setFont(font)
     .setRange(1, 180)
     .setColorTickMark(0)
     .setDecimalPrecision(0)
     ;
     
  cp5.addSlider("Speed")
     .setPosition(20,320)
     .setSize(360,20)
     .setFont(font)
     .setRange(1, 15)
     .setColorTickMark(0)
     .setDecimalPrecision(0)
     ;
     
  cp5.addButton("Nod")
      .setPosition(500, 100)
      .setFont(createFont("arial", 15))
      ;
      
  cp5.addButton("No")
      .setPosition(500, 130)
      .setFont(createFont("arial", 15))
      ;
     
  cp5.addButton("Reset")
      .setPosition(500, 160)
      .setFont(createFont("arial", 15))
      .setColorBackground(color(150, 0, 0))
      .setColorActive(color(255, 0, 0))
      .setColorForeground(color(200, 0, 0))
      ;
     
  cp5.addButton("ResetSpeed")
      .setPosition(500, 320)
      .setFont(createFont("arial", 10))
      ;
     
  cp5.addLabel("Preset 1")
      .setPosition(20, 370)
      .setFont(createFont("arial", 15))
      ;
  cp5.addButton("Save1")
      .setPosition(20, 400)
      .setFont(createFont("arial", 15))
      ;
  cp5.addButton("Play1")
      .setPosition(20, 430)
      .setFont(createFont("arial", 15))
      ;
  cp5.addButton("Clear1")
      .setPosition(20, 460)
      .setFont(createFont("arial", 15))
      ;
     
  cp5.addLabel("Preset 2")
      .setPosition(100, 370)
      .setFont(createFont("arial", 15))
      ;
  cp5.addButton("Save2")
      .setPosition(100, 400)
      .setFont(createFont("arial", 15))
      ;
  cp5.addButton("Play2")
      .setPosition(100, 430)
      .setFont(createFont("arial", 15))
      ;
  cp5.addButton("Clear2")
      .setPosition(100, 460)
      .setFont(createFont("arial", 15))
      ;
      
  cp5.addLabel("Preset 3")
      .setPosition(180, 370)
      .setFont(createFont("arial", 15))
      ;
  cp5.addButton("Save3")
      .setPosition(180, 400)
      .setFont(createFont("arial", 15))
      ;
  cp5.addButton("Play3")
      .setPosition(180, 430)
      .setFont(createFont("arial", 15))
      ;
  cp5.addButton("Clear3")
      .setPosition(180, 460)
      .setFont(createFont("arial", 15))
      ;
     
  textarea = cp5.addTextlabel("SerialRead")
      .setPosition(20, 500)
      .setSize(200, 400)
      .setLineHeight(10)
   
      ;


  textFont(font);
}



void draw() {
  background(0);
  fill(255);
  checkSerial();
  
}


void checkSerial(){ //Problems with StringIndex -1
  if(serial.available()>0){
    String string = serial.readString(); //10 = linefeed in ascii
    if(string.matches("\\A\\p{ASCII}*\\z") && string!= "") printArea(string);
    
  }
}

void printArea(String text){
  textarea.append(text, 10); //<>//
  
}

//CallbackListener callback = new CallbackListener()



void controlEvent(ControlEvent theEvent) {
   String name = theEvent.getName();
   String value = Integer.toString(round(theEvent.getValue()));
 
    if(theEvent.getName()=="input"){
      send(theEvent.getStringValue());
    }
    
    if(theEvent.isAssignableFrom(Slider.class) && theEvent.getName()!="Speed"){
      
      String servo = name.substring(name.length() - 1);
      send(servo + " " + value);
    }
    else if(theEvent.getName()=="Speed"){
      
      send("speed " + value);
    }
    else if(theEvent.isAssignableFrom(Button.class)){
      send(name);
    }
}


public void send(String text){
  serial.write(text);
  println("Sent: " + text);
}