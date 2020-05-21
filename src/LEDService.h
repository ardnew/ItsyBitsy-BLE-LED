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

#ifndef _LEDSERVICE_H
#define _LEDSERVICE_H

#include <Arduino.h>
#include <bluefruit.h>

#include "FastLED.h"

class LEDService {

protected:
  AdafruitBluefruit *_bluefruit;
  BLEDfu            *_bledfu;
  BLEDis            *_bledis;

  int8_t   _txPower;
  uint16_t _numPixels;
  uint8_t  _maxConnections;
  uint8_t  _numConnections;

  CFastLED *_fastLed;
  CRGB     *_led;

  BLEService        *_rgbService;
  BLECharacteristic *_rgbCharPixel;
  BLECharacteristic *_rgbCharCount;

  // convert 128-bit UUID to uint8_t[16] literal:
  //   $ perl -le 'print join ", ", map { "0x$_" } reverse((shift) =~ /([a-f0-9]{2})/ig)' 3f1d00c0-632f-4e53-9a14-437dd54bcccb
  //   0xcb, 0xcc, 0x4b, 0xd5, 0x7d, 0x43, 0x14, 0x9a, 0x53, 0x4e, 0x2f, 0x63, 0xc0, 0x00, 0x1d, 0x3f

  #define RGB_SERVICE_UUID128(id) \
    { 0xcb, 0xcc, 0x4b, 0xd5, 0x7d, 0x43, 0x14, 0x9a, 0x53, 0x4e, 0x2f, 0x63, (id), 0x00, 0x1d, 0x3f, }

  uint8_t const RGB_SERVICE_UUID128[16]            = RGB_SERVICE_UUID128(0xc0); // 3f1d00c0-632f-4e53-9a14-437dd54bcccb
  uint8_t const RGB_SERVICE_PIXEL_CHAR_UUID128[16] = RGB_SERVICE_UUID128(0xc1); // 3f1d00c1-632f-4e53-9a14-437dd54bcccb
  uint8_t const RGB_SERVICE_COUNT_CHAR_UUID128[16] = RGB_SERVICE_UUID128(0xc2); // 3f1d00c2-632f-4e53-9a14-437dd54bcccb

  typedef struct RgbCharPixelData {
    uint16_t start;
    uint16_t length;
    union {
      struct {
        uint8_t alpha;
        uint8_t red;
        uint8_t green;
        uint8_t blue;
      };
      uint32_t color;
    };
  };

  bool parseRgbPixelData(RgbCharPixelData * const pixel, uint8_t *data, uint16_t len);

public:
  LEDService(uint16_t const numPixels);

  bool begin(uint8_t const maxConnections, char const advName[]);
  bool advertise(void);

  void onConnect(uint16_t connHdl);
  void onDisconnect(uint16_t connHdl, uint8_t reason);
  void onRgbCharPixelWrite(uint16_t connHdl, BLECharacteristic *chr, uint8_t *data, uint16_t len);
};

extern LEDService *ledService;

#endif // _LEDSERVICE_H