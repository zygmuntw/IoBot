// IoBot Sketch v1.0 Copyright 2015 Zygmunt Wojcik
//
// IoBot Sketch is a program for Arduino UNO Microcontroller Board.
//
// It allow the Arduino Board to listen for LAPP messages (http://shop.ciseco.co.uk/llap/)
// from IoBot App on Serial and UDP port: 8888. Depending on received message it moves
// robot's arms, head and body or blink a led.
//
// It is writen in Arduino, C+/C++ based programming language.
//
// It uses Ethernet Standard Library included in Arduino IDE and VarSpeedServo
// external library - https://github.com/netlabtoolkit/VarSpeedServo.git
//
// IoBot Sketch for Arduino is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// IoBot App and IoBot Sketch for Arduino is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program.
// If not, see http://www.gnu.org/licenses/.



#include <SPI.h>       
#include <VarSpeedServo.h>
#include <Ethernet.h> 
#include <EthernetUdp.h>

#define leftLed 2
#define rightLed 3

#define UDP_TX_PACKET_MAX_SIZE 13

char* deviceID = "IB";

int headValue = 90;
int rightValue = 0; 
int bodyValue = 90;
int leftValue = 180;

// the media access control (ethernet hardware) address for the shield:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  
//the IP address for the shield:
byte ip[] = { 192, 168, 1, 177 };    
// the router's gateway address:
byte gateway[] = { 192, 168, 1, 1 };
// the subnet:
byte subnet[] = { 255, 255, 255, 0 };

unsigned int localPort = 8888;      // local port to listen on

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet

String msg = "";

VarSpeedServo headServo;  // create servo objects to control a servo
VarSpeedServo rightServo;  
VarSpeedServo bodyServo;
VarSpeedServo leftServo;  

// --------------------------------------------------------- SETUP ---------------------------------
void setup() {
  headServo.attach(5);  // attaches the servo
  headServo.write(headValue, 30);
  rightServo.attach(6);
  rightServo.write(rightValue, 30);
  bodyServo.attach(7);
  bodyServo.write(bodyValue, 30);
  leftServo.attach(9); 
  leftServo.write(leftValue, 30, true);
  
  Serial.begin(115200);
  
  // initialize the ethernet device
  Ethernet.begin(mac, ip, gateway, subnet);
  
  int success = Udp.begin(localPort);
  
  Serial.print("initialize: ");
  Serial.println(success ? "success" : "failed");
  
  Serial.print("Server address:");
  Serial.println(Ethernet.localIP());
  
  pinMode(leftLed, OUTPUT);
  pinMode(rightLed, OUTPUT);
  digitalWrite(leftLed, HIGH);
  digitalWrite(rightLed, HIGH);
}

// --------------------------------------------------------- LOOP ---------------------------------
void loop() {

  // ----------------------------------- Check incoming UDP message  
  int packetSize = Udp.parsePacket();
  if(packetSize >= 12) 
  {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remote = Udp.remoteIP();
    for (int i =0; i < 4; i++)
    {
      Serial.print(remote[i], DEC);
      if (i < 3)
      {
        Serial.print(".");
      }
    }
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    Udp.read(packetBuffer,UDP_TX_PACKET_MAX_SIZE);
    Serial.println("Contents:");
    Serial.println(packetBuffer);
    msg = String(&packetBuffer[1]);
   
    processMessage();
    
    // Stop and restart needed for UIPEthernet 
    Udp.stop();
    //restart with new connection to receive packets from other clients
    //Serial.print("restart connection: ");
    Serial.println (Udp.begin(localPort) ? "success" : "failed");
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
      
      //Serial.print(F("msg: ")); // debug
      //Serial.println(msg); // debug
      
      processMessage();
    }
    else
    Serial.read();	// throw away the character
  }
}
// --------------------------------------------------------- FUNCTIONS ---------------------

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> TRICKS <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

// Do trick
void doTrick(char* trick) {
  
  // say no
  if (trick == "SAYNO") {
    int moveSpeed = 180;
    
    for (int i=0; i<3; i++) {
      headServo.write(0, moveSpeed, true);
      headServo.write(180, moveSpeed, true);
    }
      headServo.write(90, moveSpeed);
  }
  
  // go crazy
  else if (trick == "CRAZY") {
    int moveSpeed = 200;
    
    for (int i=0; i<30; i++) {
      byte r=ran();
      headServo.write(r, moveSpeed);
      bodyServo.write(r, moveSpeed);  
      leftServo.write(r, moveSpeed); 
      rightServo.write(r, moveSpeed, true);  
    }
    start_position(50);
  }
  
  // dance
  else if (trick == "DANCE") {
    int moveSpeed = 150;
    int incremental = 10;
    int loops = 10;
    
    int distance = 0;

    for (int i=0; i<3; i++) {  
      if (i==0) start_position(50);
      if (i==1) start_position(250);
      
      for (int j=0; j<loops; j++) {
        distance = distance + incremental;
        
        headServo.write(90 + distance, moveSpeed);
        if (i>1) bodyServo.write(90 - distance, moveSpeed);
        if (i==1) bodyServo.write(90 - distance, moveSpeed, true); 
        if (i != 1) {
          leftServo.write(90 - distance, moveSpeed);  
          rightServo.write(90 - distance, moveSpeed, true);
        }
        
        headServo.write(90 - distance, moveSpeed);
        if (i>1) bodyServo.write(90 + distance, moveSpeed);
        if (i==1) bodyServo.write(90 + distance, moveSpeed, true); 
        if (i != 1) { 
          leftServo.write(90 + distance, moveSpeed);  
          rightServo.write(90 + distance, moveSpeed, true);
        }
        if (distance == (loops/2)*incremental || distance == 0) {
          incremental = -incremental;
        }
      }      
    }
   start_position(50); 
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
        leftServo.write(map(leftValue, 1, 180, 180, 1), 255); 
        
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
        rightServo.write(rightValue, 255); 
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
void start_position(int spd) {
  headServo.write(90, spd);
  rightServo.write(0, spd);
  bodyServo.write(90, spd);  
  leftServo.write(180, spd,true);
}
