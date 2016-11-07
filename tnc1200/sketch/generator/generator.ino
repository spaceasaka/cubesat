#include "TimerOne.h"
const int ptt_pin = 4;
const int led1_pin = 6;
const int led2_pin = 7;

// データ
unsigned char wave_data[] = {0x33, 0x33};

// 周波数
const unsigned short SAMPLE = 40000;
const unsigned short FM = 2200;
const unsigned short FS = 1200;
const unsigned short BOUND = 1200;

// ソフトDDSの増分
const unsigned short FM_DELTA = (unsigned short)(((unsigned long)FM << 16) / SAMPLE);
const unsigned short FS_DELTA = (unsigned short)(((unsigned long)FS << 16) / SAMPLE);
const unsigned short BOUND_DELTA = (unsigned short)(((unsigned long)BOUND << 16) / SAMPLE);

// DDSカウンタ
unsigned short bound_phase = 0;
unsigned short wave_phase = 0;
unsigned short wave_delta = 0;

// 送信カウンタ
int count = 0;
unsigned char *dp;
unsigned char data;
unsigned char sbit;

void setup() {
  init_dds(); 
  ADCSRA &= 0x7f;
  DDRC = 0x0f;
  pinMode(led1_pin, OUTPUT);
  pinMode(led2_pin, OUTPUT);

  reset_wave_data();
  Timer1.initialize();
  Timer1.attachInterrupt(timr_handle, 1000000 / SAMPLE);
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

void init_dds() {
  wave_phase = 0;
  bound_phase = 0;
  sbit = 0;
  wave();
}

void reset_wave_data() {
  dp = wave_data;
  count = sizeof(wave_data);
}

void bit_shift() {
  sbit = sbit << 1;
  if (sbit == 0) {
    data = *dp++;
    sbit = 1;
    count--;
    if (count == 0) reset_wave_data();
  }
  wave_delta = ((data & sbit) == 0) ? FS_DELTA : FM_DELTA;
}

void timr_handle() {
  wave();
  unsigned short next = bound_phase + BOUND_DELTA;
  if (next < bound_phase) bit_shift();
  bound_phase = next;
}

void wave() {
  static char wav[] = {
    8,  9, 10, 12, 13, 14, 14, 15, 15, 15, 14, 14, 13, 12, 10,  9,
    7,  6,  5,  3,  2,  1,  1,  0,  0,  0,  1,  1,  2,  3,  5,  6
  };
  PORTC = (PORTC & 0xf0) | wav[wave_phase / 2048];
  wave_phase += wave_delta;
}

