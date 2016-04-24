#include "DPad1.h"

#include <Arduino.h>


DPad1::DPad1(unsigned char port, unsigned int *directionsPtr, unsigned char diff, unsigned int debounce) {
  this->port           = port;
  this->diff           = diff;
  this->directionsPtr  = directionsPtr;
  this->debounce       = debounce;
    
  this->globalState    = 255;
  this->globalStateOld = 255;
  this->previousState  = 255;
  this->iterations     = 0;
}


unsigned char DPad1::findMatch(unsigned int input) {
  //find if the input is in interval X and X+diff for each entry in the lookup table
  for (unsigned char i=0; i<10; i++) {
    if (*(directionsPtr+i)<=input && (*(directionsPtr+i)+diff)>=input) {
      return(i);
    }    
  }
  return(0);
}


unsigned char DPad1::aquireState() {
  globalStateOld = globalState;

  unsigned char state = findMatch(analogRead(port));
  if (state != previousState) {
    iterations = 0;
  } else if ( state==previousState && iterations<65535 ) {
    iterations ++;
  }
  previousState = state;

  if (iterations > debounce) {
    globalState = state;
  }
  return globalState;
}


bool DPad1::isStateChanged() {
  return globalState != globalStateOld;
}


unsigned char DPad1::getState() {
  return globalState;
}


