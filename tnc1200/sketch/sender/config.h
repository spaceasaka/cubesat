#ifndef CONFIG_H
#define CONFIG_H

const int ptt_pin = 4;  // pc1
const int led1_pin = 6;  // pd3
const int led2_pin = 7;  // pd4

#define clr_led1    PORTD &=  ~_BV(led1_pin);
#define set_led1    PORTD |= _BV(led1_pin);
#define clr_led2    PORTD &= ~_BV(led2_pin);
#define set_led2    PORTD |= _BV(led2_pin);
#define clr_ptt     PORTD &= ~_BV(ptt_pin);
#define set_ptt     PORTD |= _BV(ptt_pin);

#endif
