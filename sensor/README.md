# Tilt sensor

This is basically to send the accellerometer data down to the main controller via RS-422. 422 was chosen because it can be run a long distance and is relatively immune to noise vs connecting the sensor directly via SPI. 

- Built using MPLAB-X v5.45, XC8 v2.31 for a PIC18F14K50. 
- Outputs a message every 250ms. The message format is as follows:

`X:6 Y:5 Z:249 S:1`

X, Y, and Z are the accellerometer data, S is the result of the built in accellerometer test. 
Accellerometer data varies between +/- 511, for a total range of +/- 2g. 
- The serial interface is 9600 8N1. It is half duplex, transmit only. The 485 xcvr chip is in TX mode continuously. 
