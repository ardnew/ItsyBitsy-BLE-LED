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

#include "LEDService.h"

#include "../ItsyBitsy-BLE-LED.h"

LEDService *ledService;

static void _onConnect(uint16_t connHdl)
  { ledService->onConnect(connHdl); }
static void _onDisconnect(uint16_t connHdl, uint8_t reason)
  { ledService->onDisconnect(connHdl, reason); }
static void _onRgbCharPixelWrite(uint16_t connHdl, BLECharacteristic *chr, uint8_t *data, uint16_t len)
  { ledService->onRgbCharPixelWrite(connHdl, chr, data, len); }

LEDService::LEDService(uint16_t const numPixels):
    _bledfu(nullptr), _bledis(nullptr),
    _txPower(BLUETOOTH_TX_POWER), _numPixels(numPixels),
    _maxConnections(0U), _numConnections(0U) {

  // ---------------------------------------------------------------------------
  // allocate/initialize variables and memory to known (possibly invalid) state

  _bluefruit = &Bluefruit;

  _fastLed = &FastLED;
  _led     = (CRGB *)calloc(numPixels, sizeof(CRGB));

  _rgbService   = new BLEService(RGB_SERVICE_UUID128);
  _rgbCharPixel = new BLECharacteristic(RGB_SERVICE_PIXEL_CHAR_UUID128);
  _rgbCharCount = new BLECharacteristic(RGB_SERVICE_COUNT_CHAR_UUID128);
}

bool LEDService::begin(uint8_t const maxConnections, char const advName[]) {

  if (nullptr == _fastLed)
    { return false; }

  // data pin must be a compile-time constant for template functionality
  _fastLed->addLeds<NEOPIXEL, NEOPIXEL_DATA_PIN>(_led, _numPixels);

  fill_rainbow(_led, _numPixels, 222/*starting hue*/);
  _fastLed->show();

  // ---------------------------------------------------------------------------
  // initialize the Bluefruit52Lib library interface

  if (nullptr == _bluefruit)
    { return false; }

  if (!_bluefruit->begin(maxConnections))
    { return false; }

  if (!_bluefruit->setTxPower(_txPower))
    { return false; }

  _bluefruit->setName(advName);
  _bluefruit->Periph.setConnectCallback(_onConnect);
  _bluefruit->Periph.setDisconnectCallback(_onDisconnect);

  // ---------------------------------------------------------------------------
  // configure the BLE [DEVICE FIRMWARE UPGRADE (DFU) SERVICE] for OTA updates

  _bledfu = new BLEDfu();
  if (ERROR_NONE != _bledfu->begin())
    { return false; }

  // ---------------------------------------------------------------------------
  // configure the BLE [DEVICE INFORMATION SERVICE (DIS)] for advertising data

  _bledis = new BLEDis();
  _bledis->setManufacturer(DEVICE_MFG);
  _bledis->setModel(DEVICE_MODEL);
  if (ERROR_NONE != _bledis->begin())
    { return false; }

  // ---------------------------------------------------------------------------
  // configure the BLE [RGB SERVICE] to host all Neopixel data and operations

  if (ERROR_NONE != _rgbService->begin())
    { return false; }

  // ---------------------------------------------------------------------------
  // configure the BLE [RGB SERVICE]:[PIXEL CHARACTERISTIC] to define color of
  // pixel ranges.

  _rgbCharPixel->setProperties(CHR_PROPS_READ | CHR_PROPS_WRITE | CHR_PROPS_WRITE_WO_RESP);
  _rgbCharPixel->setPermission(SECMODE_OPEN, SECMODE_OPEN);
  _rgbCharPixel->setFixedLen(8U);
  _rgbCharPixel->setWriteCallback(_onRgbCharPixelWrite);
  if (ERROR_NONE != _rgbCharPixel->begin())
    { return false; }

  // ---------------------------------------------------------------------------
  // configure the BLE [RGB SERVICE]:[COUNT CHARACTERISTIC] that broadcasts the
  // total number of pixels connected to and supported by the system.

  _rgbCharCount->setProperties(CHR_PROPS_READ | CHR_PROPS_INDICATE);
  _rgbCharCount->setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  _rgbCharCount->setFixedLen(2U);
  if (ERROR_NONE != _rgbCharCount->begin())
    { return false; }
  _rgbCharCount->write16(_numPixels);

  return true;
}

bool LEDService::advertise(void) {

  if (nullptr == _bluefruit)
    { return false; }

  if (!_bluefruit->Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE))
    { return false; }

  if (!_bluefruit->Advertising.addTxPower())
    { return false; }

  if (!_bluefruit->Advertising.addService(*_rgbService))
    { return false; }

  if (!_bluefruit->ScanResponse.addName())
    { return false; }

  _bluefruit->Advertising.restartOnDisconnect(true);
  _bluefruit->Advertising.setInterval(32, 244); // in unit of 0.625 ms
  _bluefruit->Advertising.setFastTimeout(30); // number of seconds in fast mode

  return _bluefruit->Advertising.start(0); // 0 = Don't stop advertising after n seconds
}

void LEDService::onConnect(uint16_t connHdl) {

  ++_numConnections;

  if (_rgbCharCount->indicateEnabled(connHdl))
    { _rgbCharCount->indicate16(connHdl, _numPixels); }

  if (_numConnections < _maxConnections)
    { _bluefruit->Advertising.start(0); }
}

void LEDService::onDisconnect(uint16_t connHdl, uint8_t reason) {

  --_numConnections;
}

void LEDService::onRgbCharPixelWrite(uint16_t connHdl, BLECharacteristic *chr, uint8_t *data, uint16_t len) {

  RgbCharPixelData pixel;
  if (parseRgbPixelData(&pixel, data, len)) {
    _fastLed->clear();
    fill_solid(_led, pixel.length, CRGB(pixel.color));
    _fastLed->show();
  }
}

 bool LEDService::parseRgbPixelData(RgbCharPixelData * const pixel, uint8_t *data, uint16_t len) {

  if (nullptr != pixel) {
    if (len >= sizeof(*pixel)) {
      pixel->start  = ((uint16_t)data[0] << 8U) | (uint16_t)data[1];
      pixel->length = ((uint16_t)data[2] << 8U) | (uint16_t)data[3];
      pixel->color  =
        ((uint16_t)data[4] << 24U) |
        ((uint16_t)data[5] << 16U) |
        ((uint16_t)data[6] <<  8U) |
        ((uint16_t)data[7] <<  0U) ;
      return true;
    }
  }
  return false;
}
