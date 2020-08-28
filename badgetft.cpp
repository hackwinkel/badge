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
#include <SPI.h>
#include "badgetft.h"
#include "badgerotation.h"


#ifdef BADGEUSEHARDWARESPI
//hardware spi
//Adafruit_ST7789::Adafruit_ST7789(int8_t cs, int8_t dc, int8_t rst)
//Adafruit_ST7789 tftdisplay = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

//selectable hardware spi
//Adafruit_ST7789::Adafruit_ST7789(SPIClass *spiClass, int8_t cs, int8_t dc, int8_t rst)
SPIClass * hspi = new SPIClass(HSPI);
Adafruit_ST7789 tftdisplay = Adafruit_ST7789(hspi,TFT_CS, TFT_DC, TFT_RST);
#else
// software spi
//Adafruit_ST7789::Adafruit_ST7789(int8_t cs, int8_t dc, int8_t mosi, int8_t sclk, int8_t rst)
Adafruit_ST7789 tftdisplay = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
#endif

// uses rotation value from badgerotation
extern int rotation;

void tftinit(int color) 
{
#ifdef BADGEUSEHARDWARESPI  
  hspi->begin(TFT_SCLK,-1,TFT_MOSI,TFT_CS); //SCLK, MISO, MOSI, SS
#endif
  pinMode(TFT_BACKLIGHT,OUTPUT);
  digitalWrite(TFT_BACKLIGHT,HIGH);
  //tftdisplay.setSPISpeed(1);                // apparently does nothing for SW SPI nor for selectable HW SPI
  tftdisplay.init(240, 240);                // Init ST7789 240x240
  tftdisplay.setRotation(rotation);         // rotation value defined in badgerotation.h happens to be exactly the one needed by the 7789 library
  tftdisplay.fillScreen(color);
}
