/*==========================================================
 ax25sender.cpp

 License: GNU General Public License V3
          (Because sdfatlib is licensed with this.)

 (C) Copyright 2016 Kazuhiro Yokota.
==========================================================*/
#include "ax25sender.h"
#include "string.h"

void AX25Sender::init() {
  field = idNon;
  nPreFlags = 46;
  nPostFlags = 5;
  set_destination("CQ", 0x60);
  set_source("JI1EVQ", 0x61);
  control = 0x03;
  PID = 0xf0;
  bound_phase = 0;
  wave_phase = 0;
  wave_delta = 0;
  wave();
}

/*-----------------------------------------------------
+ set_destination(addr, ssid)
+ あて先コールサインの書き込み
+-----------------------------------------------------*/
void AX25Sender::set_destination(char *addr, char ssid) {
  for (int i = 0; i < sizAddress - 1; i++) {
    char val = (*addr == '\0') ? (' ' << 1) : *addr++ << 1;
    dest[i] = val;
  }
  dest[sizAddress - 1] = ssid;
}

/*-----------------------------------------------------
+ set_source(addr, ssid)
+ 送信元コールサインの書き込み
+-----------------------------------------------------*/
void AX25Sender::set_source(char *addr, char ssid) {
  for (int i = 0; i < sizAddress - 1; i++) {
    char val = (*addr == '\0') ? (' ' << 1) : *addr++ << 1;
    sour[i] = val;
  }
  sour[sizAddress - 1] = ssid;
}

/*-----------------------------------------------------
+ sending()
+ 送信中
+-----------------------------------------------------*/
bool AX25Sender::sending() {
  return field != idNon;
}

/*-----------------------------------------------------
+ transmit(char *msg, int len)
+ 送信要求
+-----------------------------------------------------*/
void AX25Sender::transmit(char *msg, int len) {
  bound_phase = 0;
  wave_phase = 0;
  wave_delta = FS_DELTA;

  infomation = msg;
  length = len;
  crc.init();
  crc.calc(dest, sizAddress);
  crc.calc(sour, sizAddress);
  crc.calc(control);
  crc.calc(PID);
  crc.calc(msg, len);
  fcs = crc.get();

/* field を idNon 以外にすると task() により送信が始まる。 */
  count = nPreFlags;
  field = idPreFlags;
}

/*-----------------------------------------------------
+ task()
+ ax25フレーム生成シーケンサ
+-----------------------------------------------------*/
void AX25Sender::task() {
  unsigned short next = bound_phase + BOUND_DELTA;
  if (field == idNon) wave_phase = 0;
  else if (next < bound_phase) {
    switch (field) {
    case idPreFlags:
      flag_send();
      if (count == 0) {
        dp = dest;
        crc.init();
        data = *dp++;
        count = sizAddress;
        field = idDestination;
      }
      break;
      
    case idDestination:
      byte_send();
      if (count == 0) {
        dp = sour;
        data = *dp++;
        count = sizAddress;
        field = idSource;
      }
      break;
      
    case idSource:
      byte_send();
      if (count == 0) {
        data = control;
        count = 1;
        field = idControl;
      }
      break;
      
    case idControl:
      byte_send();
      if (count == 0) {
        data = PID;
        count = 1;
        field = idPID;
      }
      break;
      
    case idPID:
      byte_send();
      if (count == 0) {
        dp = infomation;
        data = *dp++;
        count = length;
        field = idInformation;
      }
      break;
      
    case idInformation:
      byte_send();
      if (count == 0) {
        count = 1;
        field = idFCS;
      }
      break;
      
    case idFCS:
      fcs_send();
      if (count == 0) {
        count = nPostFlags;
        field = idPostFlags;
      }
      break;

    case idPostFlags:
      flag_send();
      if (count == 0) {
        field = idNon;
      }
      break;

    default:
      field = idNon;
    }
  }
  bound_phase = next;
  wave();
}

/*-----------------------------------------------------
+ flag_send()
+ フラグ送信
+-----------------------------------------------------*/
void AX25Sender::flag_send() {
  static char sbit = 1;
  bit_send_nrzi(0x7e & sbit);
  sbit = sbit << 1;
  if (sbit == 0) {
    sbit = 1;
    count--;
  }
}

/*-----------------------------------------------------
+ byte_send()
+ 1バイトデータ送信
+-----------------------------------------------------*/
void AX25Sender::byte_send() {
  static char sbit = 1;
  if (cnt_marks >= 5) bit_send_nrzi(0);
  else {
    bit_send_nrzi(data & sbit);
    sbit = sbit << 1;
    if (sbit == 0) {
      sbit = 1;
      data = *dp++;
      count--;
    }
  }
}

/*-----------------------------------------------------
+ fcs_send()
+ FCS送信
+-----------------------------------------------------*/
void AX25Sender::fcs_send() {
  static short sbit = 1;
  if (cnt_marks >= 5) bit_send_nrzi(0);
  else {
    bit_send_nrzi(fcs & sbit);
    sbit = sbit << 1;
    if (sbit == 0) {
      sbit = 1;
      count--;
    }
  }
}

/*-----------------------------------------------------
+ bit_send_nrzi(bit)
+ ビット送信
+-----------------------------------------------------*/
void AX25Sender::bit_send_nrzi(int bit) {
  if (bit == 0) {
    wave_delta = (wave_delta != FS_DELTA) ? FS_DELTA : FM_DELTA;
    //wave_delta = FS_DELTA;
    cnt_marks = 0;
  }
  else {
    //wave_delta = FM_DELTA;
    cnt_marks++;
  }
}

/*-----------------------------------------------------
+ wave()
+ 波形生成
+-----------------------------------------------------*/
void AX25Sender::wave() {
  static char wav[] = {
    8,  9, 10, 12, 13, 14, 14, 15, 15, 15, 14, 14, 13, 12, 10,  9,
    7,  6,  5,  3,  2,  1,  1,  0,  0,  0,  1,  1,  2,  3,  5,  6
  };
  PORTC = (PORTC & 0xf0) | wav[wave_phase / 2048];
  wave_phase += wave_delta;
}
