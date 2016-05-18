# Sensor-board data packet
```netstrings
12 : US1 US2 IR1 IR2 IR3 SPEED DIS1 DIS2 DIS3 DIS4 LIGHT CHECKSUM ,
```
All bytes in the body, i.e. US1 to CHECKSUM are unsigned chars.<br/>
```12``` is the number of elements in the packet, ```:``` is the start delimiter and ```,``` is the end delimiter.<br/>
US1: Ultrasonic No.1 / front-center<br/>
US2: Ultrasonic No.2 / front-right<br/>
IR1: Infrared No.1 / side-front<br/>
IR2: Infrared No.2 / side-back<br/>
IR3: Infrared No.3 / rear-back<br/>
SPEED: Wheel encoder in cm/s<br/>
DIS1-4: Distance in cm, four-byte int<br/>
LIGHT: Light sensor reading of the surrounding brightness<br/>
CHECKSUM: calculated by XOR all bytes<br/>

# Vehicle-control data packet
```netstrings
4 : SPEED ANGLE LIGHT CHECKSUM ,
```
All bytes in the body, i.e. SPEED to CHECKSUM are unsigned chars.<br/>
```4``` is the number of elements in the packet, ```:``` is the start delimiter and ```,``` is the end delimiter.<br/>
SPEED: four different types - 0 backwards, 1 stopped, 2 forward slowly, 3 cruise speed<br/>
ANGLE: in degrees, centered around 90<br/>
LIGHT: bit 0 (rightmost bit) for brake, bit 1 for reverse, bit 2 for flashing left and bit 3 for flashing right<br/>
CHECKSUM: calculated by XOR all bytes<br/>
