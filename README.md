# Dummy-Project

Robotic Arm Project  
Servos are controlled via ESPDuino(esp8266 + Uno R3) and DOIT 16-way Servo motor shield (compatible with adafruits shield).
the esp8266 hosts a website (current control interface) which is loaded from local SPIFFS (SPI Flash File System).  
All current values from the servos are also stored locally, which is also allows for recording and replaying of Sequences.

**Status:** In Progress

#### GIFS
![Dummy Test 1](http://i.imgur.com/yWCVsk6.gifv)
![Dummy Test 2](http://i.imgur.com/u3gIL13.gifv)

### TODO
- Better Speed Control via WiFi and Serial
- Easy Input System
- Coordinate System
- Inverse Kinematics
- Leap Integration
