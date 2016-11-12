/*==========================================================
 ax25sender.h

 License: GNU General Public License V3
          (Because sdfatlib is licensed with this.)

 (C) Copyright 2016 Kazuhiro Yokota.
==========================================================*/
#ifndef _TRANCEMITTER_H
#define _TRANCEMITTER_H

#include "crc16.h"
#include "config.h"
#include <avr/io.h>
#include <avr/interrupt.h>

// 周波数
const unsigned short SAMPLE = 40000;
const unsigned short FS = 2200;
const unsigned short FM = 1200;
const unsigned short BOUND = 1200;
const long CYCLES = (F_CPU / 2 / SAMPLE);

// ソフトDDSの増分
const unsigned short FM_DELTA = (unsigned short)(((unsigned long)FM << 16) / SAMPLE);
const unsigned short FS_DELTA = (unsigned short)(((unsigned long)FS << 16) / SAMPLE);
const unsigned short BOUND_DELTA = (unsigned short)(((unsigned long)BOUND << 16) / SAMPLE);

#define sizAddress 7

enum idField {
  idNon, idPreFlags, idDestination, idSource, idControl, idPID, 
  idInformation, idInformation2, idFCS, idPostFlags, idEnd
};

class AX25Sender {
private:
  idField field;
  int     nPreFlags;
  int     nPostFlags;
  char    dest[sizAddress];
  char    sour[sizAddress];
  char    control;
  char    PID;
  unsigned short   fcs;
  crc16   crc;

  char *infomation;
  int length;
  char *dp;
  char data;
  int count;
  int cnt_marks;

  unsigned short bound_phase;
  unsigned short wave_phase;
  unsigned short wave_delta;

public:
  void init();
  void set_destination(char *addr, char ssid);
  void set_source(char *addr, char ssid);
  bool sending();
  void transmit(char *msg, int len);
  void task();

private:
  void flag_send();
  void byte_send();
  void fcs_send();
  void bit_send_nrzi(int);
  void wave();
};
#endif
