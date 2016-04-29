#include "PpmPwm.h"

#include "Arduino.h"

//https://github.com/PaulStoffregen/TimerOne

static unsigned short pwmPeriod;
static unsigned char clockSelectBits;


//#if defined(KINETISK)
//#define F_TIMER F_BUS
//#elif defined(KINETISL)
//#define F_TIMER (F_PLL/2)
//#endif

void setPeriod(unsigned long microseconds) {
//  const unsigned long cycles = (F_CPU / 2000000) * microseconds;
//  if (cycles < TIMER1_RESOLUTION) {
//    clockSelectBits = _BV(CS10);
//    pwmPeriod = cycles;
//  } else
//  if (cycles < TIMER1_RESOLUTION * 8) {
//    clockSelectBits = _BV(CS11);
//    pwmPeriod = cycles / 8;
//  } else
//  if (cycles < TIMER1_RESOLUTION * 64) {
//    clockSelectBits = _BV(CS11) | _BV(CS10);
//    pwmPeriod = cycles / 64;
//  } else
//  if (cycles < TIMER1_RESOLUTION * 256) {
//    clockSelectBits = _BV(CS12);
//    pwmPeriod = cycles / 256;
//  } else
//  if (cycles < TIMER1_RESOLUTION * 1024) {
//    clockSelectBits = _BV(CS12) | _BV(CS10);
//    pwmPeriod = cycles / 1024;
//  } else {
//    clockSelectBits = _BV(CS12) | _BV(CS10);
//    pwmPeriod = TIMER1_RESOLUTION - 1;
//  }
//  ICR1 = pwmPeriod;
//  TCCR1B = _BV(WGM13) | clockSelectBits;
  ICR1 = 5050;
  TCCR1B = _BV(WGM13) |  _BV(CS12) | _BV(CS10);
}


void initialize(unsigned long microseconds=1000000) {
  TCCR1B = _BV(WGM13);        // set mode as phase and frequency correct pwm, stop the timer
//  TCCR1B = 0x11;        // set mode as phase and frequency correct pwm, stop the timer
  TCCR1A = 0;                 // clear control register A
//  TCCR1C = 0x00;
//  setPeriod(microseconds);
  TCCR1B = _BV(WGM13) |  _BV(CS12) | _BV(CS10);
  ICR1 = 35000;
  TCNT1 = 0;
  TIMSK1 = _BV(TOIE1);
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
    this->portPwm[i]  = 0;

    pinMode(     this->portPins[i], OUTPUT);
    digitalWrite(this->portPins[i], LOW   );
  }
  va_end(arguments);

  this->portIndex=0;

  initialize();
  PpmPwm::self = this;
}


volatile PpmPwm*  PpmPwm::self = NULL;


ISR(TIMER1_OVF_vect) {
  PpmPwm::pwmIsr();
}


void PpmPwm::pwmIsr(void) {
  digitalWrite(9,  !digitalRead(9));
//  static unsigned int sumValues = 0;
//
//  if (self->portIndex == (self->ports+1) ) {
////    setPeriod(MAX_TIMEFRAME - sumValues);
//    digitalWrite(self->portPins[self->portIndex-1], LOW);
//    sumValues=0;
//    self->portIndex=0;
//  }
//  else {
//    if (self->portIndex > 0) {
//      digitalWrite(self->portPins[self->portIndex-1], LOW);
//    }
//    digitalWrite(self->portPins[self->portIndex], HIGH);
//
////    setPeriod(20000);
//
//    sumValues+= self->portPwm[self->portIndex];
//    self->portIndex++;
//  }
  ICR1 = 5500;
  TCNT1 = 0;
//  TCCR1B = _BV(WGM13) |  _BV(CS12) | _BV(CS10);
//  TCCR1B = (TCCR1B & 0b11111000) | _BV(WGM13) |  _BV(CS12) | _BV(CS10);


}



void PpmPwm::updatePort(unsigned char portIndex, unsigned int value) {
  this->portPwm[portIndex] = value;
}

//void PpmPwm::updatePortPercentage(unsigned char portIndex, float percentage) {
//
//}


unsigned int PpmPwm::getPort(unsigned char portIndex) {
  return this->portPwm[portIndex];
}






