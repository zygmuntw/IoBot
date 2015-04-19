IoBot
=====

IoBot is a DIY Mobile / Desktop App controlled 3D printed robot project.

See http://iobot.info/ for detailed instruction how to build and use your robot.

This repository contains all the source files needed to built the IoBot:
- App
- Sketch for Arduino
- 3D Design
- Schematics



IoBot App
---------

IoBot App is application that allows you to control DIY, internet connected, Arduino based robot (IoBot).

IoBot App is writen in Python and Kivy.


__Features:__

- Move robot's arms, head and body
- Blink robot's eyes
- Triger 3 programmed tricks
- Serial USB connection
- LAN IP connection


IoBot App is sending LLAP messages (http://shop.ciseco.co.uk/llap/) to Arduino via Serial or UDP port: 8888



IoBot Sketch for Arduino
------------------------

IoBot Sketch is a program for Arduino UNO Microcontroller Board.

It allow the Arduino Board to listen for LAPP messages (http://shop.ciseco.co.uk/llap/) from IoBot App on Serial and UDP port: 8888. Depending on received message it moves robot's arms, head and body or blink a led.

It is writen in Arduino, C+/C++ based programming language.

It uses Ethernet Standard Library included in Arduino IDE and VarSpeedServo external library - https://github.com/netlabtoolkit/VarSpeedServo.git



IoBot 3D Design Files
---------------------

IoBot 3D Design -  design for all printed parts needed to build the IoBot.

This design is made with Blender.

Blender is a free and open source 3D animation suite - http://www.blender.org/



IoBot Schematics
----------------

IoBot schematics images was created with Fritzing.

Fritzing is an open source software initiative to support designers and artists ready to move from physical prototyping to actual product - http://fritzing.org/home/



Licenses
---------

__IoBot App and IoBot Sketch for Arduino:__

Copyright 2015 Zygmunt Wojcik

IoBot App and IoBot Sketch for Arduino is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

IoBot App and IoBot Sketch for Arduino is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.  


IoBot App and IoBot Sketch for Arduino also uses modules or libraries from other projects:

Python and pySerial uses the Python License - http://opensource.org/licenses/PythonSoftFoundation.php  
Kivy uses the MIT License - http://opensource.org/licenses/MIT  
Ethernet and VarSpeedServo Library for Arduino uses the GNU LESSER GENERAL PUBLIC LICENSE v2.1 License - http://www.gnu.org/licenses/lgpl-2.1.txt



__IoBot 3D Design:__

IoBot 3D Design is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/.


__IoBot Schematics:__

IoBot Schematics is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License. To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/.