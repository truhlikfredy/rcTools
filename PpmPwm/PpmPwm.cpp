#include "PpmPwm.h"
#include "Arduino.h"

volatile PpmPwm*  PpmPwm::self = NULL;


ISR(TIMER1_COMPA_vect) {
  PpmPwm::pwmIsr();
}


PpmPwm::PpmPwm(unsigned char ports, ...) {
  if (ports > MAX_PORTS) {
    ports = MAX_PORTS;
  }

  this->ports = ports;

  va_list arguments;
  va_start(arguments, ports);
  for (unsigned char i = 0; i < ports; i++) {
    this->portPins[i] = va_arg(arguments, int);
    this->portPwm[i]  = 1500;

    pinMode(     this->portPins[i], OUTPUT);
    digitalWrite(this->portPins[i], LOW   );
  }
  va_end(arguments);

  PpmPwm::self = this;
}


void PpmPwm::initializeTimer() {
  cli();                   //disable interrupts

  //clear registers
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  OCR1A = 2000;          //set the first interupt 33ms into future

  TCCR1B |= (1 << WGM12);  //CTC mode
  TCCR1B |= (1 << CS11);   //8 prescaler, TCNT1 will be incremented each 0.5us
  TIMSK1 |= (1 << OCIE1A); //enable TIMER1_COMPA_vect interupt, trigger when TCNT1 == OCR1A
  sei();                   //allow interrupts
}


void PpmPwm::pwmIsr(void) {
  static unsigned int  sumValues = 0;
  static unsigned char portIndex = 0;

  //if we are at port 1,2,3... then clear the previous output to LOW
  if (portIndex > 0) {
    digitalWrite(self->portPins[portIndex-1], LOW);
  }

  if (portIndex == (self->ports) ) {
    OCR1A=MAX_TIMEFRAME - sumValues;                       // set next interrupt delay
    sumValues=0;
    portIndex=0;
  }
  else {
    OCR1A = self->portPwm[portIndex];                      // set next interrupt delay
    sumValues+= self->portPwm[portIndex];
    digitalWrite(self->portPins[portIndex], HIGH);
    portIndex++;
  }
}



void PpmPwm::updatePortRaw(unsigned char portIndex, unsigned int value) {
  if (value >= 2000 && value <= 4000) {
    this->portPwm[portIndex] = value;
  }
}


void PpmPwm::updatePortMs(unsigned char portIndex, unsigned int value) {
  if (value > 0 && value <= 1000) {
    this->portPwm[portIndex] = (1000 + value) * 2;
  }
}


//void PpmPwm::updatePortPercentage(unsigned char portIndex, float percentage) {
//
//}


unsigned int PpmPwm::getPortRaw(unsigned char portIndex) {
  return this->portPwm[portIndex];
}


