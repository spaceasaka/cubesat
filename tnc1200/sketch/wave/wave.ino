#include <avr/io.h>
#include <avr/interrupt.h>

const int ptt_pin = 4;
const int led1_pin = 6;
const int led2_pin = 7;

const unsigned short SAMPLE = 40000;
const unsigned short FM = 2200;
const unsigned short FS = 1200;
const unsigned short FM_DELTA = (unsigned short)(((unsigned long)FM << 16) / SAMPLE);
const unsigned short FS_DELTA = (unsigned short)(((unsigned long)FS << 16) / SAMPLE);

const long CYCLES = (F_CPU / 2 / SAMPLE);

unsigned short phase;  // 16ビットのDDSカウンタ
unsigned short delta;

void setup() {
  phase = 0;
  delta = FM_DELTA;

  ADCSRA &= 0x7f;
  DDRC = 0x0f;
  pinMode(ptt_pin, OUTPUT);
  pinMode(led1_pin, OUTPUT);
  pinMode(led2_pin, OUTPUT);
  start_timer();
}

void loop() {
  digitalWrite(ptt_pin, HIGH);
  digitalWrite(led1_pin, HIGH);
  digitalWrite(led2_pin, LOW);
  delay(1000);
  digitalWrite(ptt_pin, LOW);
  digitalWrite(led1_pin, LOW);
  digitalWrite(led2_pin, HIGH);
  delay(1000);
}

void wave() {
  static char wav[] = {
    8,  9, 10, 12, 13, 14, 14, 15, 15, 15, 14, 14, 13, 12, 10,  9,
    7,  6,  5,  3,  2,  1,  1,  0,  0,  0,  1,  1,  2,  3,  5,  6
  };
  PORTC = (PORTC & 0xf0) | wav[phase / 2048];
  phase += delta;
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
  wave();
}

