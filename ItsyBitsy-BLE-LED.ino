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

#include "ItsyBitsy-BLE-LED.h"

#include "src/LEDService.h"

void setup(void)
{

  WAIT_FOR_SERIAL(5000, 115200);

  ledService = new LEDService();

  bool ok =
    ledService->begin(DEVICE_NAME) &&
    ledService->advertise();

  if (!ok) { while(1) { delay(10000); } }
}

void loop(void)
{

}

void print(info_level_t level, const char *fmt, ...)
{
#ifdef PRINTF_DEBUG
  static const char *DEBUG_LEVEL_PREFIX[ilCOUNT] = {
    "[ ] ", "[*] ", "[!] "
  };
  static char buff[PRINTF_DEBUG_MAX_LEN] = { 0 };

  va_list arg;
  va_start(arg, fmt);
  vsnprintf(buff, PRINTF_DEBUG_MAX_LEN, fmt, arg);
  va_end(arg);

  Serial.print(DEBUG_LEVEL_PREFIX[level]);
  Serial.println(buff);
#else
  (void)level;
  (void)fmt;
#endif
}
