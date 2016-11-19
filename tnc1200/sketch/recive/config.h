#ifndef CONFIG_H
#define CONFIG_H

#define SAMPLE  40000
#define FS      2200
#define FM      1200
#define BOUND   1200

#define CYCLES  (F_CPU / 2 / SAMPLE)

#define FM_DELTA     ((unsigned short)(((unsigned long)FM << 16) / SAMPLE))
#define FS_DELTA     ((unsigned short)(((unsigned long)FS << 16) / SAMPLE))
#define BOUND_DELTA  ((unsigned short)(((unsigned long)BOUND << 16) / SAMPLE))

const int afsk_pin = 1;  // pb1
const int ptt_pin = 4;   // pd4
const int led1_pin = 6;  // pd6
const int led2_pin = 7;  // pd7

#define clr_led1    PORTD &=  ~_BV(led1_pin);
#define set_led1    PORTD |= _BV(led1_pin);
#define clr_led2    PORTD &= ~_BV(led2_pin);
#define set_led2    PORTD |= _BV(led2_pin);
#define clr_ptt     PORTD &= ~_BV(ptt_pin);
#define set_ptt     PORTD |= _BV(ptt_pin);

#endif
