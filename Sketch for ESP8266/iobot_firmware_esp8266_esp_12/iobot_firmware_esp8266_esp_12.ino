// IoBot Sketch for ESP8266 v1.0 Copyright 2015 Zygmunt Wojcik
//
// This IoBot Sketch is a program for ESP8266 ESP-12 WifI Microcontroller Board.
//
// It allow Board to listen for LAPP messages (http://shop.ciseco.co.uk/llap/)
// from IoBot App on Serial and UDP port: 8888. Depending on received message it moves
// robot's arms, head and body or blink a led.
//
// It is writen in Arduino, C+/C++ based programming language.
//
// It uses Standard ESP8266Wifi and Servo library included in Arduino IDE.
//
// IoBot Sketch for ESP8266 is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// IoBot App and IoBot Sketch for ESP8266 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program.
// If not, see http://www.gnu.org/licenses/.

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Servo.h>

#define leftLed 4
#define rightLed 5
#define UDP_PACKET_SIZE 13

char packetBuffer[UDP_PACKET_SIZE]; //buffer to hold incoming packet

char ssid[] = "--------";  //  your network SSID (name)
char pass[] = "--------";  // your network password

char deviceID[] = "IB";

int headValue = 90;
int rightValue = 0; 
int bodyValue = 90;
int leftValue = 180;

unsigned int localPort = 8888;      // local port to listen on

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

String msg = "";

Servo headServo;  // create servo objects to control a servos
Servo rightServo;  
Servo bodyServo;
Servo leftServo;  

// --------------------------------------------------------- SETUP ---------------------------------
void setup() {
  Serial.begin(115200);
  
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Start UDP
  Serial.println("Starting UDP");
  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());

  // Initialize led pins and turn the leds on
  pinMode(leftLed, OUTPUT);
  pinMode(rightLed, OUTPUT);
  digitalWrite(leftLed, HIGH);
  digitalWrite(rightLed, HIGH);

  headServo.attach(12);  // attach the servos
  rightServo.attach(13);
  bodyServo.attach(14);
  leftServo.attach(16); 

  start_position(); // go to start position
}

// --------------------------------------------------------- LOOP ---------------------------------
void loop() {

  // ----------------------------------- Check incoming UDP message  
  int packetSize = udp.parsePacket();
  if(packetSize >= 12) 
  {
    Serial.print("Received packet from: ");
    IPAddress remote = udp.remoteIP();
    Serial.print(remote);
    Serial.print(", port ");
    Serial.println(udp.remotePort());

    // read the packet into packetBufffer
    udp.read(packetBuffer,UDP_PACKET_SIZE);
    Serial.println("Contents:");
    Serial.println(packetBuffer);
    msg = String(&packetBuffer[1]);
   
    processMessage();
  }
  
  // ----------------------------------  Check incoming SERIAL message  
  if (Serial.available() >= 12) {
    // get the new byte:
    char inChar = (char)Serial.peek();
    if (inChar == 'a') {
      char cMessage[13];
      for (byte i = 0; i<12; i++) {
        inChar = (char)Serial.read();
        cMessage[i] = inChar;
        if (i < 11 && Serial.peek() == 'a') {
          // out of synch so abort and pick it up next time round
          return;
        }
      }
      cMessage[12]=0;
      msg = String(&cMessage[1]);

      processMessage();
    }
    else
    Serial.read();	// throw away the character
  }
}
// --------------------------------------------------------- FUNCTIONS ---------------------

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> TRICKS <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

// Do trick
void doTrick(const char* trick) {
  
  // say no
  if (trick == "SAYNO") {
    for (int i=0; i<3; i++) {
      headServo.write(0);
      if (i==0) {
        delay(180);
      }
      else {
        delay(360);
      }
      headServo.write(180);
      delay(360);
    }
      headServo.write(90);
      delay(180);
  }
  // go crazy
  else if (trick == "CRAZY") {
    
    for (int i=0; i<30; i++) {
      byte r=ran();
      headServo.write(r);
      bodyServo.write(r);  
      leftServo.write(r); 
      rightServo.write(r);
      delay(150);
    }
    start_position();
  }
  // dance
  else if (trick == "DANCE") {
    int incremental = 10;
    int loops = 10;
    int distance = 0;
    for (int i=0; i<3; i++) {  
      if (i==0) start_position();
      if (i==1) start_position();
      
      for (int j=0; j<loops; j++) {
        distance = distance + incremental;
        headServo.write(90 + distance);
        if (i>1) bodyServo.write(90 - distance);
        if (i==1) bodyServo.write(90 - distance);
        delay(100);
        if (i != 1) {
          leftServo.write(90 - distance);  
          rightServo.write(90 - distance);
          delay(100);
        }
        headServo.write(90 - distance);
        if (i>1) bodyServo.write(90 + distance);
        if (i==1) bodyServo.write(90 + distance);
        delay(100);
        if (i != 1) { 
          leftServo.write(90 + distance);  
          rightServo.write(90 + distance);
          delay(100);
        }
        if (distance == (loops/2)*incremental || distance == 0) {
          incremental = -incremental;
        }
      }      
    }
   start_position(); 
  }
}
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> END OF TRICKS <<<<<<<<<<<<<<<<<<<<<<<<

// Process received message
void processMessage() {
  
  //if device id match this device
  if (msg.substring(0, 2) == deviceID) {
    msg = msg.substring(2);
    
    // if message start with: SLD - slider
    if (msg.startsWith("SLD")) {              
      msg = msg.substring(3); 
      
      // if command is L - left
      if (msg.startsWith("L")) {
        msg = msg.substring(1); 
        String tmpPos;
        for (int i=0; i<4; i++) {
          if (isDigit(msg.charAt(i))) {
            tmpPos += String(msg.charAt(i)); 
          }
        }
        leftValue=tmpPos.toInt(); 
        leftServo.write(map(leftValue, 1, 180, 180, 1)); 
        
        return; 
      }
      // if command is R - right
      else if (msg.startsWith("R")) {    
        msg = msg.substring(1); 
        String tmpPos;
        for (int i=0; i<4; i++) {
          if (isDigit(msg.charAt(i))) {
            tmpPos += String(msg.charAt(i)); 
          }
        }
        rightValue=tmpPos.toInt();
        rightServo.write(rightValue); 
        return;        
      }
      // if command is H - head
      else if (msg.startsWith("H")) {    
        msg = msg.substring(1); 
        String tmpPos;
        for (int i=0; i<4; i++) {
          if (isDigit(msg.charAt(i))) {
            tmpPos += String(msg.charAt(i)); 
          }
        }
        headValue=tmpPos.toInt(); 
        headServo.write(headValue);
        return;        
      }
      // if command is B - body
      else if (msg.startsWith("B")) {    
        msg = msg.substring(1); 
        String tmpPos;
        for (int i=0; i<4; i++) {
          if (isDigit(msg.charAt(i))) {
            tmpPos += String(msg.charAt(i)); 
          }
        }
        bodyValue=tmpPos.toInt(); 
        bodyServo.write(bodyValue);
        return;        
      }             
    }
    
    // or if message start with: BUT - button
    else if (msg.startsWith("BUT")) {
      msg = msg.substring(3);
      
      // if command is L - left
      if (msg.startsWith("L")) {
        digitalWrite(leftLed, LOW);
        delay(300);
        digitalWrite(leftLed, HIGH);
        return;
      }
      // if command is R - right
      else if (msg.startsWith("R")) {
        digitalWrite(rightLed, LOW);
        delay(300);
        digitalWrite(rightLed, HIGH);
        return;
      }
    }
    // or if message is: DANCE
    else if (msg.startsWith("DANCE")) {
      doTrick("DANCE");
    } 
    // or if message is: SAYNO
    else if (msg.startsWith("SAYNO")) {
      doTrick("SAYNO");
    } 

    // or if message is: GOCRAZY
    else if (msg.startsWith("CRAZY")) {
      doTrick("CRAZY");
    }
  }
}

// Generate random number 0-180
byte ran() {
  byte r = random(0, 180);
  return r;
}

// Go to start position
void start_position() {
  headServo.write(90);
  rightServo.write(0);
  bodyServo.write(90);  
  leftServo.write(180);
}
