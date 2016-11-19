/*==========================================================
 crc16.cpp

 Calculates the CRC.

 License: GNU General Public License V3
          (Because sdfatlib is licensed with this.)

 (C) Copyright 2016 Kazuhiro Yokota.

==========================================================*/

#include "crc16.h"

void crc16::init() {
  crc = 0xffff;
}

void crc16::calc(char data) {
  unsigned short crc16_table[] = {
    0x0000, 0x1081, 0x2102, 0x3183, 0x4204, 0x5285, 0x6306, 0x7387, 
    0x8408, 0x9489, 0xa50a, 0xb58b, 0xc60c, 0xd68d, 0xe70e, 0xf78f };

  crc = (crc >> 4) ^ crc16_table[(crc & 0xf) ^ (data & 0xf)];
  crc = (crc >> 4) ^ crc16_table[(crc & 0xf) ^ ((unsigned char)data >> 4)];
}

unsigned short crc16::get() {
  return ~crc;
}

