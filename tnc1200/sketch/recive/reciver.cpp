/*==========================================================
 recive.cpp

 Recive byte data from FSK demodular (NJM2211) on Arduino TNC.

 License: GNU General Public License V3
          (Because sdfatlib is licensed with this.)

 (C) Copyright 2016 Kazuhiro Yokota.
==========================================================*/
#include "reciver.h"
#include "config.h"

/*-----------------------------------------------------
+ init()
+ 受信モジュールの初期化
+-----------------------------------------------------*/
void Reciver::init() {
  bound_phase = 0;
  bit_contiguous = 0;
  nbits = 0;
  clear();
  state = idsNoSignal;
}

/*-----------------------------------------------------
+ available()
+ データが読み取り可能ならばTrueを返す。
+-----------------------------------------------------*/
bool Reciver::available() {
  return rd != wr;
}

/*-----------------------------------------------------
+ clear()
+ バッファ内のデータを消去する。
+-----------------------------------------------------*/
void Reciver::clear() {
  rd = 0;
  wr = 0;
}

/*-----------------------------------------------------
+ get()
+ バッファからバイトデータを読み取る。
+-----------------------------------------------------*/
unsigned short Reciver::get() {
  unsigned short data = text[rd++];
  if (rd >= R_BUF_SIZ) rd = 0;
  return data;
}

/*-----------------------------------------------------
+ reading()
+ フレームを読み取り中ならばTrueを返す。
+-----------------------------------------------------*/
bool Reciver::reading() {
  return state == idsFrame;
}

/*-----------------------------------------------------
+ put()
+ バッファにバイトデータを書き込む。
+-----------------------------------------------------*/
void Reciver::put(unsigned short data) {
  text[wr++] = data;
  if (wr >= R_BUF_SIZ) wr = 0;
}

/*-----------------------------------------------------
+ frame_parser()
+ フレーム読み取り用ステートマシン。
+-----------------------------------------------------*/
void Reciver::frame_parser() {
  switch (state) {
  case idsNoSignal:
    if (isflag) {
      nPreFlags = 1;
      nPostFlags = 0;
      state = idsPreFlags;
    }
    break;

  case idsPreFlags:
    if (isflag) nPreFlags++;
    else if (nPreFlags > N_FLAGS_MIN - 1) {
      put(RD_FIRST_FRAME | nPreFlags);
      data = shift_reg;
      state = idsFrame;
    }
    else {
      put(RD_ERROR);
      state = idsNoSignal;
    }
    break;

  case idsFrame:
    put((unsigned short)data);
    if (isflag) {
      nPostFlags = 1;
      state = idsPostFlags;
    }
    else {
      data = shift_reg;
    }
    break;

  case idsPostFlags:
    if (isflag) nPostFlags++;
    else {
      data = shift_reg;
      if (nPostFlags == 1) {
        put(RD_DELIMITER);
        state = idsFrame;
      }
      else {
        put(RD_END_FRAMES | nPostFlags);
        state = idsNoSignal;
      }
    }
    break;

  default:
    state = idsNoSignal;
  }
}

/*-----------------------------------------------------
+ task()
+ タイマにより周期的に呼び出さなければならない。
+-----------------------------------------------------*/
void Reciver::task() {
  if (afsk_tone_change()) {
    bound_phase = 0x7fff;
    bit_contiguous = 0;
  }
  unsigned short next = bound_phase + BOUND_DELTA;
  if (next < bound_phase) bit_stuff();
  bound_phase = next;
}

/*-----------------------------------------------------
+ bit_stuff()
+ ビットの流れを読み取る。
+-----------------------------------------------------*/
void Reciver::bit_stuff() {
  static unsigned int marks_cnt = 0;

  isflag = false;
  if (bit_contiguous == 0) {
    if (marks_cnt < 5) picup(SPACE);
    else if (marks_cnt == 5) ;
    else if (marks_cnt == 6) {
        isflag = true;
        picup(SPACE);
    }
    else state = idsNoSignal;
    marks_cnt = 0;
  }
  else {
    picup(MARK);
    marks_cnt = bit_contiguous;
  }
  bit_contiguous++;
}

/*-----------------------------------------------------
+ picup(bit)
+ マーク、スペースを読み取る。
+-----------------------------------------------------*/
void Reciver::picup(int bit) {
  shift_reg = shift_reg >> 1;
  if (bit != 0) shift_reg |= 0x80; 
  nbits = ++nbits & 7;
  if (isflag) {
    if (nbits != 0) {
      put(RD_SYNC);
      nbits = 0;
    }
  }
  if (nbits == 0) frame_parser();
}

/*-----------------------------------------------------
+ afsk_tone_change()
+ NJM2211のOUT端子が変化したか。
+-----------------------------------------------------*/
bool Reciver::afsk_tone_change() {
  static char c = 0;
  char v = PINB & _BV(afsk_pin);
  if (v == c) return false;
  c = v;
  return true;
}
