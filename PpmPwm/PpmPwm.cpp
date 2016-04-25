#include "PpmPwm.h"

#include <TimerOne.h>           //https://github.com/PaulStoffregen/TimerOne

volatile PpmPwm* self;


void PpmPwmIsr(void) {
  static unsigned int sumValues = 0;

  if (self->portIndex == (self->ports+1) ) {
    Timer1.setPeriod(20000-sumValues);
    digitalWrite(self->portPins[self->portIndex-1], LOW);
    sumValues=0;
    self->portIndex=0;
  }
  else {
    if (self->portIndex > 0) {
      digitalWrite(self->portPins[self->portIndex-1], LOW);
    }
    digitalWrite(self->portPins[self->portIndex], HIGH);

    Timer1.setPeriod(self->portPwm[self->portIndex]);
    sumValues+= self->portPwm[self->portIndex];
    self->portIndex++;
  }
}


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
  Timer1.attachInterrupt(PpmPwmIsr);
  Timer1.attachInterrupt(this->PwmIsr);
  self = this;
}


PpmPwm::PpmPwm() {
}

void PpmPwm::PwmIsr(void) {

}



void PpmPwm::Update(unsigned char portIndex, unsigned int value) {
  this->portPwm[portIndex] = value;
}







