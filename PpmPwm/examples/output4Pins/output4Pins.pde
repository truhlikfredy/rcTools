#define serialSpeed  9600

#define sensorPinA   A0
#define sensorPinB   A1

#include <DPad1.h>

unsigned int  directionsA[] = {999 , 206, 646, 308, 230, 65535, 364, 187, 481, 266 };

//You can specify different directions table and different noise difference 
//values  and different debounce iteractions for each instance:
//DPad left (sensorPinA, &directionsA[0], diffA, 100);
//DPad right(sensorPinB, &directionsA[0], 22,    250);

//but nobody is stopping you from using same tables and default values
DPad1 left (sensorPinA, &directionsA[0]);
DPad1 right(sensorPinB, &directionsA[0]);


void setup() {
  Serial.begin(serialSpeed     );
// Set sensor pins to pull ups if you are not using external pull-ups
//  digitalWrite(sensorPinA, HIGH); 
//  digitalWrite(sensorPinB, HIGH); 
}


void loop() { 
  left .aquireState();
  right.aquireState();
  
  if (left.isStateChanged() || right.isStateChanged()) {
    Serial.print(left.getState());
    Serial.print(" ");
    Serial.print(right.getState());
    Serial.println();
    Serial.flush();
  }
}


