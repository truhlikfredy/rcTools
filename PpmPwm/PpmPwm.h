/*
  PpmPwm.h - Include file with the interface
  Copyright (c) 2016 Anton Krug.
*/

#ifndef PPM_PWM_h
#define PPM_PWM_h

#define MAX_PORTS     8
#define MAX_TIMEFRAME 40000         //20ms in /8 pre-scaler values

class PpmPwm {

public:

  PpmPwm(unsigned char ports, ...);

  static   void  initializeTimer(void);
  static   void  pwmIsr(void);
           void  updatePortRaw(unsigned char portIndex, unsigned int value);
           void  updatePortMs( unsigned char portIndex, unsigned int value);
  unsigned int   getPortRaw(   unsigned char portIndex);

private:
  volatile static   PpmPwm* self;
  volatile unsigned int     portPwm [MAX_PORTS];
           unsigned char    ports;
           unsigned int     portPins[MAX_PORTS];

};

#endif
