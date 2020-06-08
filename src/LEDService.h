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

#include "../ItsyBitsy-BLE-LED.h"

#include <Arduino.h>
#include <bluefruit.h>
#include <Adafruit_NeoPixel.h>

typedef union rgb_t
{
  struct
  {
    uint8_t alpha;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
  };
  uint32_t color;
};

class RgbCharColorData
{
protected:
  uint16_t  _start;
  uint16_t  _length;
  rgb_t     _rgb;
  uint8_t   _alpha;
  uint8_t   _bright;
  uint8_t  *_data;
  bool      _isValid;

public:
  RgbCharColorData(uint16_t const start, uint16_t const length, rgb_t const rgb, uint8_t const alpha, uint8_t const bright);
  RgbCharColorData(uint8_t * const data, uint16_t const len);

  static inline constexpr uint16_t size() { return sizeof(_start) + sizeof(_length) + sizeof(_rgb) + sizeof(_alpha) + sizeof(_bright); }
  inline uint16_t start() { return _start; }
  inline uint16_t length() { return _length; }
  inline rgb_t rgb() { return _rgb; }
  inline uint8_t alpha() { return _alpha; }
  inline uint8_t bright() { return _bright; }
  inline uint8_t *data() { return _data; }
  inline bool isValid() { return _isValid; }
};

class RgbCharStripData
{
protected:
  uint16_t  _numPixels;
  uint16_t  _colorOrder;
  uint16_t  _pixelType;
  uint8_t  *_data;
  bool      _isValid;

public:
  RgbCharStripData(uint16_t const numPixels, uint16_t const colorOrder, uint16_t const pixelType);
  RgbCharStripData(uint8_t * const data, uint16_t const len);

  static inline constexpr uint16_t size() { return sizeof(_numPixels) + sizeof(_colorOrder) + sizeof(_pixelType); }
  inline uint16_t numPixels() { return _numPixels; }
  inline uint16_t colorOrder() { return _colorOrder; }
  inline uint16_t pixelType() { return _pixelType; }
  inline uint8_t *data() { return _data; }
  inline bool isValid() { return _isValid; }
};

class LEDService
{
protected:
  AdafruitBluefruit *_bluefruit;
  BLEDfu            *_bledfu;
  BLEDis            *_bledis;

  uint16_t _dataPin;
  uint16_t _numPixels;
  uint16_t _colorOrder;
  uint16_t _pixelType;
  int8_t   _txPower;
  uint8_t  _maxConnections;
  uint8_t  _numConnections;

  Adafruit_NeoPixel *_neopixel;

  BLEService        *_rgbService;
  BLECharacteristic *_rgbCharColor;
  BLECharacteristic *_rgbCharStrip;

  // convert 128-bit UUID to uint8_t[16] literal:
  //   $ perl -le 'print join ", ", map { "0x$_" } reverse((shift) =~ /([a-f0-9]{2})/ig)' 3f1d00c0-632f-4e53-9a14-437dd54bcccb
  //   0xcb, 0xcc, 0x4b, 0xd5, 0x7d, 0x43, 0x14, 0x9a, 0x53, 0x4e, 0x2f, 0x63, 0xc0, 0x00, 0x1d, 0x3f

  #define RGB_SERVICE_UUID128(id) \
    { 0xcb, 0xcc, 0x4b, 0xd5, 0x7d, 0x43, 0x14, 0x9a, 0x53, 0x4e, 0x2f, 0x63, (id), 0x00, 0x1d, 0x3f, }

  uint8_t const RGB_SERVICE_UUID128[16]            = RGB_SERVICE_UUID128(0xc0); // 3f1d00c0-632f-4e53-9a14-437dd54bcccb
  uint8_t const RGB_SERVICE_STRIP_CHAR_UUID128[16] = RGB_SERVICE_UUID128(0xc1); // 3f1d00c1-632f-4e53-9a14-437dd54bcccb
  uint8_t const RGB_SERVICE_COLOR_CHAR_UUID128[16] = RGB_SERVICE_UUID128(0xc2); // 3f1d00c2-632f-4e53-9a14-437dd54bcccb
  uint8_t const RGB_SERVICE_ANIMA_CHAR_UUID128[16] = RGB_SERVICE_UUID128(0xc3); // 3f1d00c2-632f-4e53-9a14-437dd54bcccb

public:
  LEDService(
    uint16_t const dataPin    = NEOPIXEL_DATA_PIN,
    uint16_t const numPixels  = NEOPIXEL_LENGTH_PX,
    uint16_t const colorOrder = NEOPIXEL_ORDER,
    uint16_t const pixelType  = NEOPIXEL_TYPE,
    int8_t   const txPower    = BLUETOOTH_TX_POWER
  );

  bool begin(
    char    const advName[],
    uint8_t const maxConnections = BLUETOOTH_CONN_MAX
  );
  bool advertise(void);

  void onConnect(uint16_t connHdl);
  void onDisconnect(uint16_t connHdl, uint8_t reason);
  void onRgbCharColorWrite(uint16_t connHdl, BLECharacteristic *chr, uint8_t *data, uint16_t len);
  void onRgbCharStripWrite(uint16_t connHdl, BLECharacteristic *chr, uint8_t *data, uint16_t len);
};

extern LEDService *ledService;

#endif // _LEDSERVICE_H