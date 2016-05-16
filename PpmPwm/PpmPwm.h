/*
  PpmPwm.h - Include file with the interface
  Copyright (c) 2016 Anton Krug.
*/

#ifndef PPM_PWM_h
#define PPM_PWM_h

#define DEFAULT_TIMING  (1000*2)          //Default timing for unset port
#define MAX_PORTS       8                 //Limit maximum PWM to emulate fact that PPM source would be limited as well
#define MAX_TIMEFRAME   (20*1000*2)       //20ms timeframe in /8 pre-scaler ticks
#define MAX_INPUT_PINS  24


struct pinInterupt {
  unsigned char bitOffset;
  unsigned int  portPin;
  unsigned char interupt;
};


class PpmPwm {

public:

  PpmPwm(unsigned char ports, ...);

  static   void  pwmOutIsr(void);
  static   void  pwmInIsr( void);
           void  initializePwmOutput(void);
           void  initializePwmInput( void);
           void  updatePortRaw(         unsigned char portIndex, unsigned int value      );
           void  updatePortMs(          unsigned char portIndex, unsigned int value      );
           void  updatePortMsWithOffset(unsigned char portIndex, unsigned int value      );
           void  updatePortFloat(       unsigned char portIndex, float percentage        );
           void  updatePortPercentage(  unsigned char portIndex, unsigned char percentage);
  unsigned int   getPortRaw(            unsigned char portIndex                          );
  unsigned int   getPortMs(             unsigned char portIndex                          );
  unsigned int   getPortMsWithOffset(   unsigned char portIndex                          );
  unsigned char  getPortPercentage(     unsigned char portIndex                          );



private:
  volatile static   PpmPwm* self;
  volatile unsigned int     portPwm [MAX_PORTS];
           unsigned char    ports;
           unsigned int     portPins[MAX_PORTS];

   static pinInterupt pinInteruptsAvaiable[MAX_INPUT_PINS];
          pinInterupt pinInteruptsUsed[    MAX_PORTS     ];

};


#endif
