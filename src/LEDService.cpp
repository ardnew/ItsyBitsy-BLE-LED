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

LEDService *ledService;

static void _onConnect(uint16_t connHdl)
  { ledService->onConnect(connHdl); }
static void _onDisconnect(uint16_t connHdl, uint8_t reason)
  { ledService->onDisconnect(connHdl, reason); }
static void _onRgbCharColorWrite(uint16_t connHdl, BLECharacteristic *chr, uint8_t *data, uint16_t len)
  { ledService->onRgbCharColorWrite(connHdl, chr, data, len); }
static void _onRgbCharStripWrite(uint16_t connHdl, BLECharacteristic *chr, uint8_t *data, uint16_t len)
  { ledService->onRgbCharStripWrite(connHdl, chr, data, len); }

RgbCharColorData::RgbCharColorData(
  uint16_t const start, uint16_t const length, rgb_t const rgb, uint8_t const alpha, uint8_t const bright)
:
  _start(start),
  _length(length),
  _rgb(rgb),
  _alpha(alpha),
  _bright(bright),
  _isValid(true),
  _data((uint8_t *)malloc(size()))
{
  _data[0] = (uint8_t)((_start  >> 8U) & 0xFF);
  _data[1] = (uint8_t)((_start  >> 0U) & 0xFF);
  _data[2] = (uint8_t)((_length >> 8U) & 0xFF);
  _data[3] = (uint8_t)((_length >> 0U) & 0xFF);
  _data[4] = _rgb.alpha;
  _data[5] = _rgb.red;
  _data[6] = _rgb.green;
  _data[7] = _rgb.blue;
  _data[8] = _alpha;
  _data[9] = _bright;
}

RgbCharColorData::RgbCharColorData(uint8_t * const data, uint16_t const len)
:
  _start(0U),
  _length(0U),
  _rgb({0U}),
  _alpha(0U),
  _bright(0U),
  _isValid(false),
  _data((uint8_t *)malloc(size()))
{
  if (len >= size()) {
    _start     = ((uint16_t)data[0] << 8U) | (uint16_t)data[1];
    _length    = ((uint16_t)data[2] << 8U) | (uint16_t)data[3];
    _rgb.color =
      ((uint16_t)data[4] << 24U) |
      ((uint16_t)data[5] << 16U) |
      ((uint16_t)data[6] <<  8U) |
      ((uint16_t)data[7] <<  0U) ;
    _alpha     = data[8];
    _bright    = data[9];
    memcpy(_data, data, size());
    _isValid = true;
  }
}

RgbCharStripData::RgbCharStripData(
  uint16_t const numPixels, uint16_t const colorOrder, uint16_t const pixelType)
:
  _numPixels(numPixels),
  _colorOrder(colorOrder),
  _pixelType(pixelType),
  _isValid(true),
  _data((uint8_t *)malloc(size()))
{
  _data[0] = (uint8_t)((_numPixels  >> 8U) & 0xFF);
  _data[1] = (uint8_t)((_numPixels  >> 0U) & 0xFF);
  _data[2] = (uint8_t)((_colorOrder >> 8U) & 0xFF);
  _data[3] = (uint8_t)((_colorOrder >> 0U) & 0xFF);
  _data[4] = (uint8_t)((_pixelType  >> 8U) & 0xFF);
  _data[5] = (uint8_t)((_pixelType  >> 0U) & 0xFF);
}

RgbCharStripData::RgbCharStripData(uint8_t * const data, uint16_t const len)
:
  _numPixels(0U),
  _colorOrder(0U),
  _pixelType(0U),
  _isValid(false),
  _data((uint8_t *)malloc(size()))
{
  if (len >= size()) {
    _numPixels  = ((uint16_t)data[0] << 8U) | (uint16_t)data[1];
    _colorOrder = ((uint16_t)data[2] << 8U) | (uint16_t)data[3];
    _pixelType  = ((uint16_t)data[4] << 8U) | (uint16_t)data[5];
    memcpy(_data, data, size());
    _isValid = true;
  }
}

LEDService::LEDService(
  uint16_t const dataPin, uint16_t const numPixels,
  uint16_t const colorOrder, uint16_t const pixelType, int8_t const txPower)
:
  _bledfu(nullptr),
  _bledis(nullptr),
  _dataPin(dataPin),
  _numPixels(numPixels),
  _colorOrder(colorOrder),
  _pixelType(pixelType),
  _txPower(txPower),
  _maxConnections(0U),
  _numConnections(0U)
{
  // ---------------------------------------------------------------------------
  // allocate/initialize variables and memory to known (possibly invalid) state

  _bluefruit = &Bluefruit;
  _bledfu    = new BLEDfu();
  _bledis    = new BLEDis();

  _neopixel = new Adafruit_NeoPixel(_numPixels, _dataPin, _colorOrder | _pixelType);

  _rgbService   = new BLEService(RGB_SERVICE_UUID128);
  _rgbCharColor = new BLECharacteristic(RGB_SERVICE_COLOR_CHAR_UUID128);
  _rgbCharStrip = new BLECharacteristic(RGB_SERVICE_STRIP_CHAR_UUID128);
}

bool LEDService::begin(
  char const advName[], uint8_t const maxConnections)
{
  if (nullptr == _neopixel)
    { return false; }

  _neopixel->begin();
  _neopixel->setBrightness(0xFF);
  _neopixel->show();

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

  if (ERROR_NONE != _bledfu->begin())
    { return false; }

  // ---------------------------------------------------------------------------
  // configure the BLE [DEVICE INFORMATION SERVICE (DIS)] for advertising data

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

  _rgbCharColor->setProperties(CHR_PROPS_READ | CHR_PROPS_WRITE | CHR_PROPS_WRITE_WO_RESP);
  _rgbCharColor->setPermission(SECMODE_OPEN, SECMODE_OPEN);
  _rgbCharColor->setFixedLen(RgbCharColorData::size());
  _rgbCharColor->setWriteCallback(_onRgbCharColorWrite);
  if (ERROR_NONE != _rgbCharColor->begin())
    { return false; }

  // ---------------------------------------------------------------------------
  // configure the BLE [RGB SERVICE]:[STRIP CHARACTERISTIC] that broadcasts the
  // physical properties of the strip connected to and supported by the system.

  _rgbCharStrip->setProperties(CHR_PROPS_READ | CHR_PROPS_WRITE | CHR_PROPS_WRITE_WO_RESP | CHR_PROPS_INDICATE);
  _rgbCharStrip->setPermission(SECMODE_OPEN, SECMODE_OPEN);
  _rgbCharStrip->setFixedLen(RgbCharStripData::size());
  _rgbCharStrip->setWriteCallback(_onRgbCharStripWrite);
  if (ERROR_NONE != _rgbCharStrip->begin())
    { return false; }

  // initialize for reading by connected clients
  RgbCharStripData stripData(_numPixels, _colorOrder, _pixelType);
  _rgbCharStrip->write(stripData.data(), RgbCharStripData::size());

  return true;
}

bool LEDService::advertise(void)
{
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

void LEDService::onConnect(uint16_t connHdl)
{
  ++_numConnections;

  if (_rgbCharStrip->indicateEnabled(connHdl)) {
    RgbCharStripData stripData(_numPixels, _colorOrder, _pixelType);
    _rgbCharStrip->indicate(connHdl, stripData.data(), RgbCharStripData::size());
  }

  if (_numConnections < _maxConnections)
    { _bluefruit->Advertising.start(0); }
}

void LEDService::onDisconnect(uint16_t connHdl, uint8_t reason)
{
  --_numConnections;
}

void LEDService::onRgbCharColorWrite(
  uint16_t connHdl, BLECharacteristic *chr, uint8_t *data, uint16_t len)
{
  RgbCharColorData color(data, len);
  if (color.isValid()) {
    _neopixel->clear();
    _neopixel->fill(_neopixel->gamma32(color.rgb().color));
    _neopixel->show();
  }
}

void LEDService::onRgbCharStripWrite(
  uint16_t connHdl, BLECharacteristic *chr, uint8_t *data, uint16_t len)
{
  RgbCharStripData strip(data, len);
  if (strip.isValid()) {

    if (strip.numPixels() != _numPixels) {
      _numPixels = strip.numPixels();
      _neopixel->updateLength(_numPixels);
    }

    bool shouldChangeType = false;

    if (strip.colorOrder() != _colorOrder) {
      _colorOrder = strip.colorOrder();
      shouldChangeType = true;
    }
    if (strip.pixelType() != _pixelType) {
      _pixelType = strip.pixelType();
      shouldChangeType = true;
    }

    if (shouldChangeType) {
      _neopixel->updateType(_colorOrder | _pixelType);
    }
  }
}
