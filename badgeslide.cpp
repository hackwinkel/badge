/*
 * Copyright (c) 2020 hackwinkel
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
 
#include <Arduino.h>
#include "badgeslide.h"

// uses SPIFFS (indirectly) so that is included in .h and mounted in setup()

// space for 16 images. filenames ar just numbers, not their upload names
const char * filenames[]={
  "/01.jpg",
  "/02.jpg",
  "/03.jpg",
  "/04.jpg",
  "/05.jpg",
  "/06.jpg",
  "/07.jpg",
  "/08.jpg",
  "/09.jpg",
  "/10.jpg",
  "/11.jpg",
  "/12.jpg",
  "/13.jpg",
  "/14.jpg",
  "/15.jpg",
  "/16.jpg"
};
// serial port has already been opened, but we also need access to tft
#include "badgetft.h"
extern Adafruit_ST7789 tftdisplay;

bool sliderenderblock(int16_t xpos, int16_t ypos, uint16_t width, uint16_t height, uint16_t* bitmap)
{
  if ( ypos >= tftdisplay.height() ) return(0); // no point in continuing beyond end of screen
  if ( xpos >= tftdisplay.width() ) return(0); // no point in rendering beyond right of screen, but continue with the image
  tftdisplay.drawRGBBitmap(xpos, ypos, bitmap, width, height);
  return(1);// continue with the next block
}

void slideinit(void)
{
    TJpgDec.setJpgScale(1);
    TJpgDec.setCallback(sliderenderblock); // callback from the jpeg decoder that actually puts something on the tft
}

int showslide(int nr)
{
  uint16_t width=0, height=0;
  TJpgDec.getFsJpgSize(&width, &height, filenames[nr]); // filename must be preceded with "/"
  if ((width<1)||(height<1)) // jpeg not understood
  {
    Serial.print(filenames[nr]);
    Serial.println("does not seem to be a valid BASELINE jpeg");
    Serial.print("Width = "); Serial.print(width); Serial.print(", height = "); Serial.println(height);
    Serial.println("Please only upload BASELINE jpegs, PROGRESSIVE jpegs do not work");
    return(0);
  }  
  TJpgDec.drawFsJpg(0, 0, filenames[nr]);// filename must be preceded with "/"
  return(1);
}
