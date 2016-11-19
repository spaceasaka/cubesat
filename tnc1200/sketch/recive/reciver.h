/*==========================================================
 recive.h

 Recive byte data from FSK demodular (NJM2211) on Arduino TNC.

 License: GNU General Public License V3
          (Because sdfatlib is licensed with this.)

 (C) Copyright 2016 Kazuhiro Yokota.
==========================================================*/
#ifndef _RECIVER_H
#define _RECIVER_H

#include "config.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define SPACE 0
#define MARK 0x80

#define N_FLAGS_MIN 4
#define R_BUF_SIZ   8

#define RD_DATA         0x0000
#define RD_FIRST_FRAME  0x0100
#define RD_DELIMITER    0x0200
#define RD_END_FRAMES   0x0400
#define RD_SYNC         0x4000
#define RD_ERROR        0x8000
#define RD_DATA_MASK    0x00ff
#define RD_MSG_MASK     0xff00

enum idState {
  idsNoSignal, idsPreFlags, idsFrame, idsPostFlags
};

class Reciver {
private:
  idState state;
  int bit_contiguous;
  int nbits;
  bool isflag;
  unsigned char nPreFlags;
  unsigned char nPostFlags;
  unsigned char data;
  unsigned char shift_reg;

  unsigned short bound_phase;

  unsigned short text[R_BUF_SIZ];
  int rd;
  int wr;

public:
  void init();
  void task();
  bool available();
  void clear();
  unsigned short get();
  bool reading();

private:
  void put(unsigned short data);
  void frame_parser();
  void bit_stuff();
  void picup(int);
  bool afsk_tone_change();
};
#endif

