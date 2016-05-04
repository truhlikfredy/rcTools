#include <PpmPwm.h>

PpmPwm pwm(4,9,10,11,12);

void setup() {
  pwm.updatePortMs(0,0);      //will set chanel to 1000ms
  pwm.updatePortMs(1,333);    //will set chanel to 1333ms 
  pwm.updatePortMs(2,666);    //will set chanel to 1666ms 
  pwm.updatePortMs(3,1000);   //will set chanel to 2000ms 
  pwm.initializePwmOutput();
}

void loop() {
}