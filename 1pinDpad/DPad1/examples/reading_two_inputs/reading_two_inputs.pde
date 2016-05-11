#define serialSpeed  9600

#define sensorPinA   A0
#define sensorPinB   A1

#include <DPad1.h>

// This table was calculated with 20k pull up resistor, if you different setup, use
// calibrate snippet to read all your values for your resistors.
//                               0     1    2    3    4    5      6    7    8    9
unsigned int  directionsA[] = {1010 , 365, 485, 419, 608, 65535, 765, 262, 316, 284 };

//You can specify different directions table and different noise difference 
//values  and different debounce iteractions for each instance:
//DPad left (sensorPinA, &directionsA[0], diffA, 100);
//DPad right(sensorPinB, &directionsA[0], 22,    250);

//but nobody is stopping you from using same tables and default values
DPad1 left (sensorPinA, &directionsA[0]);
DPad1 right(sensorPinB, &directionsA[0]);


void setup() {
  Serial.begin(serialSpeed     );
// Set sensor pins to pull ups if you are not using external pull-ups, but then reacalculate
// the lookup table again with calibration sketch

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


