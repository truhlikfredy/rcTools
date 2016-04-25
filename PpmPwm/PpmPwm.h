/*
  PpmPwm.h - Include file with the interface
  Copyright (c) 2016 Anton Krug.
*/

#ifndef PPM_PWM_h
#define PPM_PWM_h

#define MAX_PORTS   8

class PpmPwm {

public:
  PpmPwm(unsigned char ports, ...);
  void Update(unsigned char portIndex, unsigned int value);

private:
           unsigned char ports;
           unsigned int  portPins[MAX_PORTS];
  volatile unsigned int  portPwm [MAX_PORTS];
  volatile unsigned char portIndex;

  void PwmIsr();


};

#endif
