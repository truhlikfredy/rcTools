#include "PpmPwm.h"

#include <TimerOne.h>           //https://github.com/PaulStoffregen/TimerOne


PpmPwm::PpmPwm(unsigned char ports, ...) {
  if (ports > MAX_PORTS) {
    ports = MAX_PORTS;
  }

  this->ports = ports;

  va_list arguments;
  va_start(arguments, ports);
  for (unsigned char i = 0; i < ports; i++) {
    this->portPwm[i] = va_arg(arguments, int);

    pinMode(     this->portPwm[i], OUTPUT);
    digitalWrite(this->portPwm[i], LOW   );
    //std::cout << this->portPwm[i] << " ";
  }
  //std::cout << std::endl;
  va_end(arguments);

  this->portIndex=0;

  Timer1.initialize(1000*1000);
  Timer1.attachInterrupt(PpmPwm::PwmIsr);
}


void PpmPwm::Update(unsigned char portIndex, unsigned int value) {
  this->portPwm[portIndex] = value;
}


void PpmPwm::PwmIsr(void) {
  static unsigned int sumValues = 0;

  if (this.portIndex == (this->ports+1) ) {
    Timer1.setPeriod(20000-sumValues);
    digitalWrite(this->ports[this->portIndex-1], LOW);
    sumValues=0;
    this->portIndex=0;
  }
  else {
    if (this->portIndex > 0) {
      digitalWrite(this->ports[this->portIndex-1], LOW);
    }

    digitalWrite(this->ports[this->portIndex], HIGH);
    Timer1.setPeriod(this->portPwm[this->portIndex]);
    sumValues+= this->portPwm[this->portIndex];
    this->portIndex++;
  }
}





