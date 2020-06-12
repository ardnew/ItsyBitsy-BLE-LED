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

#include "LedColor.h"

void LedColor::fill(Adafruit_NeoPixel * const pix)
{
  pix->clear();
  pix->fill(pix->gamma32(color().color));
  pix->show();
}

rgb_t LedColor::nextWheel(uint8_t const pos)
{
  uint8_t curr = 0xFF - pos;

  if (curr < 0x55) {
    setColor({
      .u32 = {
        .blue  = 0x00,
        .green = 0xFF - curr * 0x03,
        .red   = 0x03 * curr,
        .alpha = 0xFF,
      },
    });
  }
  else if (curr < 0xAA) {
    curr -= 0x55;
    setColor({
      .u32 = {
        .blue  = 0x03 * curr,
        .green = 0x00,
        .red   = 0xFF - curr * 0x03,
        .alpha = 0xFF,
      },
    });
  }
  else {
    curr -= 0xAA;
    setColor({
      .u32 = {
        .blue  = 0xFF - curr * 0x03,
        .green = 0x03 * curr,
        .red   = 0x00,
        .alpha = 0xFF,
      },
    });
  }

  return color();
}
