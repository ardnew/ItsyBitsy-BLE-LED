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

#ifndef _LEDANIMA_H
#define _LEDANIMA_H

#include "../ItsyBitsy-BLE-LED.h"

#include "LedColor.h"

enum class AnimaMode: uint8_t
{
  NONE,   // = 0
  Wheel,  // = 1
  Chase,  // = 2
  Scan,   // = 3
  Fade,   // = 4
  COUNT   // = 5
};

class LedAnima
{
protected:
  AnimaMode   _mode;
  bool        _reverse;
  uint16_t    _frame;
  uint8_t     _speed;
  uint8_t     _length;
  rgb_t       _color1;
  rgb_t       _color2;
  uint8_t     _delay;
  uint16_t    _numPixels;
  timespan_t  _lastUpdate;
  LedColor   *_ledColor;

public:
  LedAnima(uint16_t const numPixels): LedAnima(AnimaMode::NONE, false, 5U, numPixels) { }
  LedAnima(AnimaMode const mode, bool const reverse, uint8_t const delay, uint16_t const numPixels);

  static inline AnimaMode mode(uint8_t ord)
  {
    return (( ord >= (uint8_t)AnimaMode::NONE  )&&
            ( ord <  (uint8_t)AnimaMode::COUNT ))
      ? (AnimaMode)ord
      : AnimaMode::NONE;
  }

  static inline uint8_t ordinal(AnimaMode mode)
    { return (uint8_t)mode; }

  AnimaMode mode() { return _mode; }
  bool reverse() { return _reverse; }
  uint8_t delay() { return _delay; }

  void setNumPixels(uint16_t const numPixels);
  void setMode(AnimaMode const mode, bool const reverse, uint8_t const delay, uint8_t *anima);
  void setMode(uint8_t const mode, bool const reverse, uint8_t const delay, uint8_t *anima)
    { setMode(LedAnima::mode(mode), reverse, delay, anima); }

  void update(Adafruit_NeoPixel * const pix, timespan_t const now);
};

#endif // _LEDANIMA_H