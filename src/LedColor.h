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

#ifndef _LEDCOLOR_H
#define _LEDCOLOR_H

#include "../ItsyBitsy-BLE-LED.h"

#define FLOAT32_ABSTOL 5.0e-6  // 32-bit

#define COLOR_BLACK       (rgb_t){ .u32 = { blue:   0, green:   0, red:   0, alpha: 255 } }
#define COLOR_NAVY        (rgb_t){ .u32 = { blue: 123, green:   0, red:   0, alpha: 255 } }
#define COLOR_DARKGREEN   (rgb_t){ .u32 = { blue:   0, green: 125, red:   0, alpha: 255 } }
#define COLOR_DARKCYAN    (rgb_t){ .u32 = { blue: 123, green: 125, red:   0, alpha: 255 } }
#define COLOR_MAROON      (rgb_t){ .u32 = { blue:   0, green:   0, red: 123, alpha: 255 } }
#define COLOR_PURPLE      (rgb_t){ .u32 = { blue: 123, green:   0, red: 123, alpha: 255 } }
#define COLOR_OLIVE       (rgb_t){ .u32 = { blue:   0, green: 125, red: 123, alpha: 255 } }
#define COLOR_LIGHTGREY   (rgb_t){ .u32 = { blue: 198, green: 195, red: 198, alpha: 255 } }
#define COLOR_DARKGREY    (rgb_t){ .u32 = { blue: 123, green: 125, red: 123, alpha: 255 } }
#define COLOR_BLUE        (rgb_t){ .u32 = { blue: 255, green:   0, red:   0, alpha: 255 } }
#define COLOR_GREEN       (rgb_t){ .u32 = { blue:   0, green: 255, red:   0, alpha: 255 } }
#define COLOR_CYAN        (rgb_t){ .u32 = { blue: 255, green: 255, red:   0, alpha: 255 } }
#define COLOR_RED         (rgb_t){ .u32 = { blue:   0, green:   0, red: 255, alpha: 255 } }
#define COLOR_MAGENTA     (rgb_t){ .u32 = { blue: 255, green:   0, red: 255, alpha: 255 } }
#define COLOR_YELLOW      (rgb_t){ .u32 = { blue:   0, green: 255, red: 255, alpha: 255 } }
#define COLOR_WHITE       (rgb_t){ .u32 = { blue: 255, green: 255, red: 255, alpha: 255 } }
#define COLOR_ORANGE      (rgb_t){ .u32 = { blue:   0, green: 165, red: 255, alpha: 255 } }
#define COLOR_GREENYELLOW (rgb_t){ .u32 = { blue:  41, green: 255, red: 173, alpha: 255 } }
#define COLOR_PINK        (rgb_t){ .u32 = { blue: 198, green: 130, red: 255, alpha: 255 } }

union rgb_t
{
  struct __attribute__((packed))
  {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t alpha;
  } u32;
  uint32_t color;
};

class SRGB
{
private:
  inline static uint8_t clip(int16_t const c)
    { return (c < 0) ? 0U : ( (c > 255) ? 255U : c ); }
  inline static int16_t interp(
      int16_t const x,
      int16_t const x1, int16_t const y1,
      int16_t const x2, int16_t const y2)
  {
    float scale = (float)(x - x1);
    float numer = (float)(y2 - x2);
    float denom = (float)(y1 - x1);
    if (fabsf(denom) < FLOAT32_ABSTOL) {
      denom = FLOAT32_ABSTOL;
    }
    return (int16_t)roundf(scale * numer / denom) + x2;
  }

protected:
  int16_t _alpha;
  int16_t _red;
  int16_t _green;
  int16_t _blue;

public:
  SRGB(void): SRGB(COLOR_WHITE) { }
  SRGB(rgb_t const &rgb):
    _alpha(rgb.u32.alpha), _red(rgb.u32.red),
    _green(rgb.u32.green), _blue(rgb.u32.blue) { }

  void set(rgb_t const &rgb) // widens storage (int16_t)
  {
    _alpha = rgb.u32.alpha;
    _red   = rgb.u32.red;
    _green = rgb.u32.green;
    _blue  = rgb.u32.blue;
  }

  void set(uint32_t const color)
  {
    _alpha = (uint16_t)((color >> 24U) & 0xFF);
    _red   = (uint16_t)((color >> 16U) & 0xFF);
    _green = (uint16_t)((color >>  8U) & 0xFF);
    _blue  = (uint16_t)((color >>  0U) & 0xFF);
  }

  rgb_t rgb(void) const // narrows storage (uint8_t, clipped color components)
  {
    return (rgb_t){
      .u32 = {
        .blue  = SRGB::clip(_blue),
        .green = SRGB::clip(_green),
        .red   = SRGB::clip(_red),
        .alpha = SRGB::clip(_alpha),
      },
    };
  }

  inline bool operator==(SRGB const &srgb) const {
    return ( _alpha == srgb._alpha ) && ( _red  == srgb._red  ) &&
           ( _green == srgb._green ) && ( _blue == srgb._blue ) ;
  }
  inline bool operator!=(SRGB const &srgb) const {
    return ( _alpha != srgb._alpha ) || ( _red  != srgb._red  ) ||
           ( _green != srgb._green ) || ( _blue != srgb._blue ) ;
  }
};

class LedColor
{
protected:
  SRGB  _srgb;

public:
  LedColor(void): _srgb() { }
  LedColor(rgb_t const &rgb): _srgb(rgb) { }

  rgb_t color(void) { return _srgb.rgb(); }
  void setColor(rgb_t const &rgb) { _srgb.set(rgb); }
  void setColor(uint32_t const color) { _srgb.set(color); }

  rgb_t nextWheel(uint8_t const pos);

  void fill(Adafruit_NeoPixel * const pix);
};

#endif // _LEDCOLOR_H