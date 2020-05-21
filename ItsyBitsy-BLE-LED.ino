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

void setup(void) {

  WAIT_FOR_SERIAL(5000, 115200);

  Serial.printf("initializing...\n");

  ledService = new LEDService(NEOPIXEL_LENGTH_PX);

  bool ok = ledService->begin(BLUETOOTH_CONN_MAX, DEVICE_NAME);
  if (ok) {
    ok = ledService->advertise();
    if (!ok) {
      Serial.printf("failed to initialize BLE advertising data");
    }
  } else {
    Serial.printf("failed to initialized BLE library resources");
  }

  if (!ok) { while(1) { delay(10000); } }

  Serial.printf("done\n");

}

void loop(void) {

}
