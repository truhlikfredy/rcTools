#include <PpmPwm.h>

PpmPwm pwm(4,9,10,11,12);

//same as updatePortMs but you need add 1000ms to each value and then both
//of them multiply by 2 range 2000-4000 is equvalent to 1000ms - 2000ms 
//which means 0% - 100% of possible range
void setup() {
  pwm.updatePortRaw(0,2000);   //will set chanel to 1000ms
  pwm.updatePortRaw(1,2666);   //will set chanel to 1333ms 
  pwm.updatePortRaw(2,3332);   //will set chanel to 1666ms 
  pwm.updatePortRaw(3,4000);   //will set chanel to 2000ms 
  pwm.initializePwmOutput();
}

void loop() {
}