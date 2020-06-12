/*-----------------------------------------------------------------------------
 -                                                                            -
 - ItsyBitsy-BLE-LED                                                          -
 - Copyright (c) 2020 andrew                                                  -
 -                                                                            -
 - Permission is hereby granted, free of charge, to any person obtaining a    -
 - copy of this software and associated documentation files (the "Software"), -
 - to deal in the Software without restriction, including without limitation  -
 - the rights to use, copy, modify, merge, publish, distribute, sublicense,   -
 - and/or sell copies of the Software, and to permit persons to whom the      -
 - Software is furnished to do so, subject to the following conditions:       -
 -                                                                            -
 - The above copyright notice and this permission notice shall be included in -
 - all copies or substantial portions of the Software.                        -
 -                                                                            -
 - THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR -
 - IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   -
 - FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    -
 - THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER -
 - LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    -
 - FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        -
 - DEALINGS IN THE SOFTWARE.                                                  -
 -                                                                            -
 -----------------------------------------------------------------------------*/

#ifndef _ITSYBITSY_BLE_LED_H
#define _ITSYBITSY_BLE_LED_H

#include <Arduino.h>
#include <bluefruit.h>
#include <Adafruit_NeoPixel.h>

#define PRINTF_DEBUG_MAX_LEN 256 // undef or 0 to disable debug printing

#define DEVICE_NAME  "Nitelite"
#define DEVICE_MFG   "Adafruit Industries"
#define DEVICE_MODEL "ItsyBitsy nRF52840"

// nRF52840 supported Tx power settings (dBm):
//   -40 -20 -16 -12 -8 -4 0 +2 +3 +4 +5 +6 +7 +8
#define BLUETOOTH_TX_POWER     4
#define BLUETOOTH_CONN_MAX     3
#define BLUETOOTH_CHAR_LEN_MAX 20 // max size of characteristic

#define NEOPIXEL_DATA_PIN  5
#define NEOPIXEL_LENGTH_PX 30
#define NEOPIXEL_TYPE      NEO_KHZ800 // in Adafruit_NeoPixel.h
#define NEOPIXEL_ORDER     NEO_GRB    //

#if PRINTF_DEBUG_MAX_LEN
  #define PRINTF_DEBUG
  #define WAIT_FOR_SERIAL(timeout, baud) \
      while (!Serial && (millis() < (timeout))) \
        { continue; } Serial.begin(baud);
#else
  #define WAIT_FOR_SERIAL(timeout, baud) \
      /* debug code omitted */
#endif

typedef unsigned long timespan_t; // storage for arduino's millis()

typedef enum
{
  ilNONE = -1,
  ilInfo,  // = 0
  ilWarn,  // = 1
  ilError, // = 2
  ilCOUNT  // = 3
}
info_level_t;

void print(info_level_t level, const char *fmt, ...);

#define infof(fmt, ...) print(ilInfo, fmt, __VA_ARGS__)
#define warnf(fmt, ...) print(ilWarn, fmt, __VA_ARGS__)
#define errf(fmt, ...)  print(ilError, fmt, __VA_ARGS__)

#endif // _ITSYBITSY_BLE_LED_H