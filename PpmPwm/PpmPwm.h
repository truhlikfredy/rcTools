/*
  PpmPwm.h - Include file with the interface
  Copyright (c) 2016 Anton Krug.
*/

#ifndef PPM_PWM_h
#define PPM_PWM_h

#define MAX_PORTS     8
#define MAX_TIMEFRAME 20000


#define TIMER1_RESOLUTION 65536UL

class PpmPwm {

public:

  PpmPwm(unsigned char ports, ...);

  void         updatePort(unsigned char portIndex, unsigned int value);
  unsigned int getPort(unsigned char portIndex);
  static void pwmIsr(void);

private:
  volatile static   PpmPwm* self;
  volatile unsigned int     portPwm [MAX_PORTS];
  volatile unsigned char    portIndex;
           unsigned char    ports;
           unsigned int     portPins[MAX_PORTS];

};

#endif
