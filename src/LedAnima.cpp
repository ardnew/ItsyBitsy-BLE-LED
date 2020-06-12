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

#include "LedAnima.h"

LedAnima::LedAnima(AnimaMode const mode, bool const reverse, uint8_t const delay, uint16_t const numPixels)
:
  _mode(mode), _reverse(reverse), _frame(0U), _speed(1), _delay(delay), _numPixels(numPixels), _lastUpdate(0U)
{
  _length = 3;
  _color1 = COLOR_WHITE;
  _color2 = COLOR_BLACK;

  _ledColor = (LedColor *)calloc(numPixels, sizeof(*_ledColor));
}

void LedAnima::setMode(AnimaMode const mode, bool reverse, uint8_t const delay, uint8_t *anima)
{
  if (mode != _mode) {
    _mode       = mode;
    _frame      = 0U;
    _lastUpdate = 0U;
  }
  if (reverse != _reverse) {
    _reverse    = reverse;
    _lastUpdate = 0U;
  }
  if (delay != _delay) {
    _delay      = delay;
    _lastUpdate = 0U;
  }

  switch (mode) {
    case AnimaMode::Wheel:
    {
      _speed = anima[0];
      break;
    }

    case AnimaMode::Chase:
    {
      _speed            = anima[0];
      _length           = anima[1];
      _color1.u32.alpha = anima[2];
      _color1.u32.red   = anima[3];
      _color1.u32.green = anima[4];
      _color1.u32.blue  = anima[5];
      _color2.u32.alpha = anima[6];
      _color2.u32.red   = anima[7];
      _color2.u32.green = anima[8];
      _color2.u32.blue  = anima[9];
      break;
    }

    case AnimaMode::Fade:
    {
      _frame            = 0U;
      _reverse          = false;
      _speed            = anima[0];
      _length           = anima[1];
      _color1.u32.alpha = anima[2];
      _color1.u32.red   = anima[3];
      _color1.u32.green = anima[4];
      _color1.u32.blue  = anima[5];
      _color2.u32.alpha = anima[6];
      _color2.u32.red   = anima[7];
      _color2.u32.green = anima[8];
      _color2.u32.blue  = anima[9];
      for (int i = 0; i <= 9; ++i) {
        infof("anima[%d] = %02X", i, anima[i]);
      }
      break;
    }

    default:
      break;
  }
}

void LedAnima::setNumPixels(uint16_t const numPixels)
{
  if (numPixels != _numPixels) {

    LedColor *ledColor = (LedColor *)calloc(numPixels, sizeof(*ledColor));
    LedColor *ledColorPrev = _ledColor;

    _ledColor = (LedColor *)memcpy(ledColor, _ledColor, min(numPixels, _numPixels) * sizeof(*ledColor));

    free(ledColorPrev);

    _numPixels  = numPixels;
    _lastUpdate = 0U;
  }
}

void LedAnima::update(Adafruit_NeoPixel * const pix, timespan_t const now)
{
  // if sufficient time has not elapsed AND current time did not overflow/rollover
  if ((now - _lastUpdate < _delay) || (now < _lastUpdate))
    { return; }
  _lastUpdate = now;

  switch (_mode) {

    case AnimaMode::Wheel:
    {
      for (uint16_t i = 0U; i < _numPixels; ++i) {
        pix->setPixelColor(i, pix->gamma32(
          _ledColor[i].nextWheel(
            (i * 256U / _numPixels + _frame * _speed / 10U) & 0xFF
          ).color
        ));
      }
      break;
    }

    case AnimaMode::Chase:
    {
      uint32_t u1 = pix->gamma32(_color1.color);
      uint32_t u2 = pix->gamma32(_color2.color);
      uint32_t u;
      for (uint16_t i = 0U; i < _numPixels; ++i) {
        u = (0 == (i + _frame * _speed / 10U) % _length) ? u1 : u2;
        pix->setPixelColor(i, u);
      }
      break;
    }

    case AnimaMode::Scan:
      break;

    case AnimaMode::Fade:
    {
      uint16_t index = _frame * _speed / 10U;

      if ((!_reverse && (index >= _length)) || (_reverse && (index <= 1U))) {
        _reverse = !_reverse;
      }

      rgb_t rgb = {
        .u32 = {
          .blue  = (_color1.u32.blue  * (_length - index) + _color2.u32.blue  * index) / _length,
          .green = (_color1.u32.green * (_length - index) + _color2.u32.green * index) / _length,
          .red   = (_color1.u32.red   * (_length - index) + _color2.u32.red   * index) / _length,
          .alpha = 0xFF,
        },
      };

      //infof("frame = %d, index = %d (%08X <-> %08X): %08X", _frame, index, _color1.color, _color2.color, rgb.color);

      pix->fill(pix->gamma32(rgb.color));
      break;
    }

    default:
      break;
  }

  _frame += _reverse ? -1 : +1;

  pix->show();
}