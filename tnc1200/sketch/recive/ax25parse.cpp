/*========================================================
 ax25parse.cpp

 Get a AX25 frame from byte stream.

 License: GNU General Public License V3

 (C) Copyright 2016 Kazuhiro Yokota.
==========================================================*/
#include "ax25parse.h"
#include <arduino.h>

/*-----------------------------------------------------
+ begin()
+
+-----------------------------------------------------*/
void AX25Parse::begin()
{
   crc.init();
   count = 0;
   field = idDestination;
}

/*-----------------------------------------------------
+ parse(data)
+
+-----------------------------------------------------*/
void AX25Parse::parse(char data)
{
  switch (field) {
  case idDestination:
    crc.calc(data);
    dest[count] = (unsigned char)data >> 1;
    count++;
    if (count >= sizAddress) {
      dssid = data;
      dest[sizAddress - 1] = '\0';
      Serial.print(dest);
      Serial.print("<-");
      count = 0;
      field = idSource;
    }
    break;

  case idSource:
    crc.calc(data);
    sour[count] = (unsigned char)data >> 1;
    count++;
    if (count >= sizAddress) {
      sssid = data;
      sour[sizAddress - 1] = '\0';
      Serial.print(sour);
      Serial.print(": ");
      field = idControl;
    }
    break;

  case idControl:
    crc.calc(data);
    cont = data;
    field = idPID;
    break;

  case idPID:
    crc.calc(data);
    PID = data;
    count = 0;
    wp = 0;
    field = idInformation;
    break;

  case idInformation:
    fifo[wp++] = data;
    wp &= 0x03;
    count++;
    if (count >= 2) {
      rp = 0;
      field = idInformation2;
    }
    break;

  case idInformation2:
    fifo[wp++] = data;
    wp &= 0x03;
    data = fifo[rp++];
    rp &= 0x03;
    crc.calc(data);
    Serial.write(data);
    break;
  }
}

/*-----------------------------------------------------
+ terminal()
+
+-----------------------------------------------------*/
void AX25Parse::terminal()
{
  unsigned short fcs = (unsigned char)fifo[rp++];
  rp &= 0x03;
  fcs = (fifo[rp] << 8) | fcs;
  if (fcs == crc.get()) {
    Serial.println(" :OK");
  }
  else {
    Serial.println(" :ERR");
  }
}

