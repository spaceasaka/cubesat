#include "ax25sender.h"
#include "config.h"

AX25Sender tx;
char msg[] = "AMATEUR RADIO SATELITE TEST"; 

bool running;

void setup() {
  clr_led1
  clr_led2

  ADCSRA &= 0x7f;
  DDRC = 0x0f;
  DDRD = _BV(led1_pin) | _BV(led2_pin) | _BV(ptt_pin);

  tx.init();
  start_timer();
  running = false;
}

void loop() {
  if (!tx.sending()) {
    if (!running) {
      set_led1
      set_ptt
      delay(500);
      tx.transmit(msg, sizeof(msg));
      running = true;
    }
    else {
      clr_ptt
      clr_led1
      delay(2000);
      running = false;
    }
  }
}

void start_timer() {
  TCCR1A = 0;
  TCCR1B = _BV(WGM13);

  char oldSREG = SREG;				
  cli();
  ICR1 = CYCLES;
  SREG = oldSREG;
  
  TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
  TCCR1B |= _BV(CS10);
  TIMSK1 = _BV(TOIE1);
}

ISR(TIMER1_OVF_vect) {
  tx.task();
}

