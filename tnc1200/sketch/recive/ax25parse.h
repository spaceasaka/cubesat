/*==========================================================
 ax25parse.h

 Get a AX25 frame from byte stream.

 License: GNU General Public License V3

 (C) Copyright 2016 Kazuhiro Yokota.
==========================================================*/
#ifndef _AX25_PARSE_H
#define _AX25_PARSE_H

#include "crc16.h"
#define sizAddress 7

enum idField {
  idNon, idPreFlags, idDestination, idSource, idControl, idPID, 
  idInformation, idInformation2, idFCS, idPostFlags, idEnd
};

class AX25Parse {
private:
  idField field;
  int     count;
  char    dest[sizAddress];
  char    dssid;
  char    sour[sizAddress];
  char    sssid;
  char    cont;
  char    PID;
  short   FCS;
  char    fifo[4];
  int     wp;
  int     rp;
  crc16   crc;

public:
  void begin();
  void parse(char data);
  void terminal();
};
#endif

