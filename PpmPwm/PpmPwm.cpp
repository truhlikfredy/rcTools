#include "PpmPwm.h"
#include "Arduino.h"

volatile PpmPwm*  PpmPwm::self = NULL;


//Timer1 compare
ISR(TIMER1_COMPA_vect) {
  PpmPwm::pwmOutIsr();
}


////Pin change interrupt D8 - D13 pins
//ISR (PCINT0_vect) {
//  PpmPwm::pwmInIsr();
//}
//
////Pin change interrupt A0 - A5 pins
//ISR (PCINT1_vect) {
//  PpmPwm::pwmInIsr();
//}
//
//
////Pin change interrupt D0 - D7 pins
//ISR (PCINT1_vect) {
//  PpmPwm::pwmInIsr();
//}


PpmPwm::PpmPwm(unsigned char ports, ...) {
  if (ports > MAX_PORTS) {
    ports = MAX_PORTS;
  }

  this->ports = ports;

  va_list arguments;
  va_start(arguments, ports);
  for (unsigned char i = 0; i < ports; i++) {
    this->portPins[i] = va_arg(arguments, int);
    this->portPwm[i]  = DEFAULT_TIMING;            //set default timing for unset port
  }
  va_end(arguments);

  PpmPwm::self = this;

  pinInterupt pinInteruptsAvaiable[MAX_INPUT_PINS] {
    { .bitOffset = 0, .portPin = 8,  .interupt = 0},
    { .bitOffset = 1, .portPin = 9,  .interupt = 0},
    { .bitOffset = 2, .portPin = 10, .interupt = 0},
    { .bitOffset = 3, .portPin = 11, .interupt = 0},
    { .bitOffset = 4, .portPin = 12, .interupt = 0},
    { .bitOffset = 5, .portPin = 13, .interupt = 0},
    { .bitOffset = 6, .portPin = -1, .interupt = 0},
    { .bitOffset = 7, .portPin = -1, .interupt = 0},

    { .bitOffset = 0, .portPin = 14, .interupt = 1},
    { .bitOffset = 1, .portPin = 15, .interupt = 1},
    { .bitOffset = 2, .portPin = 16, .interupt = 1},
    { .bitOffset = 3, .portPin = 17, .interupt = 1},
    { .bitOffset = 4, .portPin = 18, .interupt = 1},
    { .bitOffset = 5, .portPin = 19, .interupt = 1},
    { .bitOffset = 6, .portPin = -1, .interupt = 1},
    { .bitOffset = 7, .portPin = -1, .interupt = 1},

    { .bitOffset = 0, .portPin = 0,  .interupt = 2},
    { .bitOffset = 1, .portPin = 1,  .interupt = 2},
    { .bitOffset = 2, .portPin = 2,  .interupt = 2},
    { .bitOffset = 3, .portPin = 3,  .interupt = 2},
    { .bitOffset = 4, .portPin = 4,  .interupt = 2},
    { .bitOffset = 5, .portPin = 5,  .interupt = 2},
    { .bitOffset = 6, .portPin = 6,  .interupt = 2},
    { .bitOffset = 7, .portPin = 7,  .interupt = 2}
  };

}


void PpmPwm::initializePwmOutput() {

  for (unsigned char i = 0; i < this->ports; i++) {
    pinMode(     this->portPins[i], OUTPUT);
    digitalWrite(this->portPins[i], LOW   );
  }

  cli();                   //disable interrupts

  //clear registers
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  OCR1A = 0xffff;          //set the first interupt 33ms into future

  TCCR1B |= (1 << WGM12);  //CTC mode
  TCCR1B |= (1 << CS11);   //8 prescaler, TCNT1 will be incremented each 0.5us
  TIMSK1 |= (1 << OCIE1A); //enable TIMER1_COMPA_vect interupt, trigger when TCNT1 == OCR1A
  sei();                   //allow interrupts
}


void PpmPwm::initializePwmInput() {

  unsigned char intRegister[3];

  for (unsigned char i = 0; i < this->ports; i++) {

    //Prepare the pin change interrupt registers
    for (unsigned char j = 0 ; j < MAX_INPUT_PINS; j++) {
      if (pinInteruptsAvaiable[j].portPin == this->portPins[i]) {
        pinInteruptsUsed[i] = pinInteruptsAvaiable[j];
        intRegister[pinInteruptsAvaiable[j].interupt] |= 1 << pinInteruptsAvaiable[j].bitOffset;
      }
    }

    pinMode(this->portPins[i], INPUT);
  }

  cli();                   //disable interrupts

  //set the pin change interrupt registers
  PCMSK0 = intRegister[0];
  PCMSK1 = intRegister[1];
  PCMSK2 = intRegister[2];

  //enable interupts depending on what masks we populated
  PCIFR |= ((intRegister[0]>0)?1:0) | ((intRegister[1]>0)?2:0) | ((intRegister[0]>0)?4:0);

  sei();                   //allow interrupts
}


void PpmPwm::pwmOutIsr(void) {
  static unsigned int  sumTicks  = 0;
  static unsigned char portIndex = 0;

  //if we are at portIndex 1,2,3... then clear the previous output to LOW
  if (portIndex > 0) {
    digitalWrite(self->portPins[portIndex-1], LOW);
  }

  if (portIndex == self->ports ) {
    //if index is after last port then wait rest of time to equal 20ms for all ports
    //ports is counted from 1 while portIndex from 0
    OCR1A = MAX_TIMEFRAME - sumTicks;                  // set next interrupt delay
    sumTicks  = 0;
    portIndex = 0;
  }
  else {
    //for ports 0...self->ports-1 set given port high for his self->portPwm duration
    OCR1A     = self->portPwm[portIndex];              // set next interrupt delay
    sumTicks += OCR1A;                                 // count up how many ticks all ports took together
    digitalWrite(self->portPins[portIndex], HIGH);
    portIndex++;
  }
}



void PpmPwm::pwmInIsr(void) {
  static unsigned int  sumTicks  = 0;
  static unsigned char portIndex = 0;

  for (unsigned char i = 0; i < self->ports; i++) {
  }

}


void PpmPwm::updatePortRaw(unsigned char portIndex, unsigned int value) {
  if (value >= 2000 && value <= 4000 && portIndex<this->ports) {
    this->portPwm[portIndex] = value;
  }
}


void PpmPwm::updatePortMs(unsigned char portIndex, unsigned int value) {
  if (value > 0 && value <= 1000 && portIndex<this->ports) {
    this->portPwm[portIndex] = (1000 + value) * 2;
  }
}


void PpmPwm::updatePortFloat(unsigned char portIndex, float percentage) {
  if (percentage >= 0 && percentage <= 1.0 && portIndex<this->ports) {
    this->portPwm[portIndex] = (percentage*2000)+2000;
  }
}


void PpmPwm::updatePortPercentage(unsigned char portIndex, unsigned char percentage) {
  if (percentage >= 0 && percentage <= 100 && portIndex<this->ports) {
    this->portPwm[portIndex] = map(percentage, 0, 100, 2000, 4000);
  }
}

unsigned int PpmPwm::getPortRaw(unsigned char portIndex) {
  if (portIndex<this->ports) {
    return this->portPwm[portIndex];
  }
  else {
    return -1;
  }
}


unsigned int PpmPwm::getPortMs(unsigned char portIndex) {
  if (portIndex<this->ports) {
    return (this->portPwm[portIndex] / 2) - 1000;
  }
  else {
    return -1;
  }
}


unsigned int PpmPwm::getPortMsWithOffset(unsigned char portIndex) {
  if (portIndex<this->ports) {
    return this->portPwm[portIndex] / 2;
  }
  else {
    return -1;
  }
}
