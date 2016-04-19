# Sensor data packet
```netstrings
7:(US1 byte)(US2 byte)(IR1 byte)(IR2 byte)(IR3 byte)(Speed byte)(Checksum byte),
```
US1: Ultrasonic No. 1 -> Byte<br />
US2: Ultrasonic No. 2 -> Byte<br />
IR1: Infrared No. 1 -> Byte<br />
IR2: Infrared No. 2 -> Byte<br />
IR3: Infrared No. 3 -> Byte<br />
Speed: Wheel encoder, cm/s -> Byte<br />
CHECKSUM: All values XORed -> Byte<br />

# Control data packet
```netstrings
3:(Engine speed byte)(Steering angle byte)(Checksum byte),
```
Engine speed -> Byte<br />
Steering angle -> Byte<br />
CHECKSUM: All values XORed -> Byte<br />
