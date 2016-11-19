/*=========================================================
 Arduino TNC main module.

 License: GNU General Public License V3

 (C) Copyright 2016 Kazuhiro Yokota.
==========================================================*/
#include "ax25parse.h"
#include "reciver.h"
#include "config.h"

AX25Parse parse;
Reciver rx;

/*-----------------------------------------------------
+ setup()
+
+-----------------------------------------------------*/
void setup() {
  init_port();
  rx.init();
  
  cli();
  start_timer();
  sei();

  Serial.begin(9600);
  Serial.println("Arduino AX25 packet reciver.");
}

/*-----------------------------------------------------
+ loop()
+
+-----------------------------------------------------*/
void loop() {
  PORTD = (rx.reading()) ? PORTD | _BV(led1_pin) : PORTD & ~_BV(led1_pin);

  if (rx.available()) {
    recive_packet();
  }
}

/*-----------------------------------------------------
+ recive_packet()
+
+-----------------------------------------------------*/
void recive_packet() {
  unsigned short work = rx.get();
  unsigned short cmd = work & RD_MSG_MASK;
  unsigned short data = work & RD_DATA_MASK;
    
  if (cmd == 0) {
    parse.parse((char)data);
  }
  else if ((cmd & RD_FIRST_FRAME) != 0) {
    Serial.print("\r\n");
    Serial.print("Begin ");
    Serial.print(data);
    Serial.println(" Flags.");
    parse.begin();
  }
  else if ((cmd & RD_DELIMITER) != 0) {
    parse.terminal();
    parse.begin();
  }
  else if ((cmd & RD_END_FRAMES) != 0) {
    parse.terminal();
    Serial.print("End ");
    Serial.print(data);
    Serial.println(" Flags.");
  }
//    else if ((cmd & RD_SYNC) != 0) {
//      Serial.println("Sync.");
//    }
}

/*-----------------------------------------------------
+ init_port()
+
+-----------------------------------------------------*/
void init_port() {
  ADCSRA &= 0x7f;      // ADC OFF;
  PORTB = 0;
  DDRB = 0;
  PORTD = 0;
  DDRD = _BV(led1_pin) | _BV(led2_pin);
  DDRC = 0x0f;
}

/*-----------------------------------------------------
+ start_timer()
+
+-----------------------------------------------------*/
void start_timer() {
  TCCR1A = 0;
  TCCR1B = _BV(WGM13);

  char oldSREG = SREG;
  ICR1 = CYCLES;
  SREG = oldSREG;

  TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
  TCCR1B |= _BV(CS10);
  TIMSK1 = _BV(TOIE1);
}

/*-----------------------------------------------------
+ ISR(TIMER1_OVF_vect)
+
+-----------------------------------------------------*/
ISR(TIMER1_OVF_vect) {
  rx.task();
}


