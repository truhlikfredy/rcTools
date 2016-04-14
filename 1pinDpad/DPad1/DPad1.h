/*
  DPad.h - Include file with interface to support directional pad with only 1 input
  Copyright (c) 2016 Anton Krug.
*/

#ifndef DPAD1_h
#define DPAD1_h

#define DEFAULT_VALUE_VARIATION     11
#define DEFAULT_DEBOUNCE_ITERATIONS 150

class DPad1 {

public:
  DPad1(unsigned char port, 
        unsigned int  *directionsPtr, 
        unsigned char diff           = DEFAULT_VALUE_VARIATION, 
        unsigned int  debounce       = DEFAULT_DEBOUNCE_ITERATIONS);

  void aquireState();
  bool isStateChanged();
  unsigned char getState();
  

private:
  unsigned char port;
  unsigned int  *directionsPtr;
  unsigned char diff;
  unsigned char globalState;
  unsigned char globalStateOld;
  unsigned int  debounce;

  unsigned char previousState;
  unsigned int  iterations;
  
  unsigned char findMatch(unsigned int input);

};

#endif
