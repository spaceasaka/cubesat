/*==========================================================
 crc16.h



 License: GNU General Public License V3
          (Because sdfatlib is licensed with this.)

 (C) Copyright 2016 Kazuhiro Yokota.
==========================================================*/

#ifndef _crc16_h
#define _crc16_h

class crc16 {
protected:
  unsigned short crc;

public:
  void init();
  void calc(char *dp, int len);
  void calc(char data);
  unsigned short get();
};

#endif
