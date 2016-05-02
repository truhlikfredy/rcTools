/*
  PpmPwm.h - Include file with the interface
  Copyright (c) 2016 Anton Krug.
*/

#ifndef PPM_PWM_h
#define PPM_PWM_h

#define MAX_PORTS     8                 //Limit maximum PWM to emulate fact that PPM source would be limited as well
#define MAX_TIMEFRAME (40*1000)         //20ms timeframe in /8 pre-scaler ticks

class PpmPwm {

public:

  PpmPwm(unsigned char ports, ...);

  static   void  initializeTimer(void);
  static   void  pwmIsr(void);
           void  updatePortRaw(       unsigned char portIndex, unsigned int value      );
           void  updatePortMs(        unsigned char portIndex, unsigned int value      );
           void  updatePortFloat(     unsigned char portIndex, float percentage        );
           void  updatePortPercentage(unsigned char portIndex, unsigned char percentage);
  unsigned int   getPortRaw(          unsigned char portIndex                          );


private:
  volatile static   PpmPwm* self;
  volatile unsigned int     portPwm [MAX_PORTS];
           unsigned char    ports;
           unsigned int     portPins[MAX_PORTS];

};

#endif
