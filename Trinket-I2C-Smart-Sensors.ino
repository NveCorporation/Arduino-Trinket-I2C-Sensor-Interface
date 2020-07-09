/**************************************************************************************
Uses a Trinket to display NVE I²C Smart Sensor data on a COM-11441 7-segment display
SDA=#0; SCL=#2; LED=#1; USB for programming=#3/#4 
Sensor #0 = AG958 Magnetometer Sensor Board; Sensor #1 = AG966 Angle Sensor Board
See https://www.nve.com/webstore/catalog for I2C sensors and breakout boards
**************************************************************************************/
#include <TinyWireM.h> //ATTiny85 I²C Master library
//Use Wire.h for Arduino and replace "TinyWireM." with "Wire."
int digit; byte i, active; //Digit counter; sensor #; active sensor#
int sensor[2], sensorOld[2]; //Sensor output

//Sensor characteristics (I²C address, # of bytes, decimal point display location):
const byte sensorAddr[]={16, 72}, bytes[]={1, 2}, decimal[]={0b010, 0b0100};
void setup() {TinyWireM.begin();}
void loop() {
for(i=0; i<2; i++) { //Scan sensors
  sensorOld[i] = sensor[i];
  TinyWireM.requestFrom(sensorAddr[i]>>1,bytes[i]);
  sensor[i] = TinyWireM.read(); //Read sensor
  if (bytes[i]==2) sensor[i]=sensor[i]<<8|TinyWireM.read(); //Read 2nd byte if applicable 
  if (abs(sensorOld[i]-sensor[i])>20*bytes[i]*bytes[i]) active=i; //Check sensor activity
//Display sensor output
  TinyWireM.beginTransmission(0x71); //Write to 7-segment display (I²C address 0x71)
  TinyWireM.write(0x76); //Clear display
  TinyWireM.write(0x77); //Set decimal point
  TinyWireM.write(decimal[active]); 
  if (bytes[active]==1) {
     TinyWireM.write(0x79);
     TinyWireM.write(1); } //Set cursor to 2nd digit for 1-byte sensor 
for(digit=bytes[active]*900-800; digit>0; digit/=10) { //Start at 1000s for 2-byte; 100s for 1-byte 
  TinyWireM.write((sensor[active]%(digit*10))/digit); } //Scan through display digits
  TinyWireM.endTransmission(); }
delay(100); //10 samples/second
}
