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
 


// Include SPIFFS for slideshow and web server
#define FS_NO_GLOBALS
#include <FS.h>
#include "SPIFFS.h"

/*
 * badge stuff
 */
#include "badgeslide.h"
#include "badgeleds.h"
#include "badgerotation.h"
#include "badgetft.h"
#include "badgewifi.h"
#include "badgebuttons.h"
#include "badgelagno.h"
#include "badgelife.h"

// rotation setup in badgerotation
extern int rotation;
// async webserver listening on port 80 setup in badgewifi
// extern WebServer webserver;
// set if uploads during slideshow and games should be allowed
extern int allowconcurrentupload;

// tft setup in badgetft
extern Adafruit_ST7789 tftdisplay;

// lagno fields
extern char playfield[64];
extern char playground[64]; 


int mode=0;
uint32_t lastslidetime=0;
int ledcount=0;
uint32_t lastledtime=0;

void setup()
{
  Serial.begin(115200);
  
  if (!SPIFFS.begin(true)) Serial.println("An Error has occurred while mounting SPIFFS");
  /*
   * quickly list the files in SPIFFS for debugging
   */
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  while(file)
  {
      Serial.println(file.name());
      file = root.openNextFile();
  }
  
  //start with rotation 0 to read buttons held at boot time to determine which configuration to use
  rotationinit(0);
  buttoninit();
  buttonsetrotation();
  int b=buttonsalldirect();
/* 
 * correspondance buttons to bits:   
 *    
 *      4      0        A       U
 *           2   3            L   R
 *      5      1        B       D
 *      
 */
  if (b & 0x10) // configuration
  { 
    mode=0;
    rotationinit(ROTATION_STANDING); 
    buttonsetrotation();
  }
  else if (b & 0x20) // upload
  { 
    mode=1;
    rotationinit(ROTATION_STANDING); 
    buttonsetrotation();
  }
  else
  {
    switch (b & MASKDIR)
    {
      case 1: // righthanded lagno
        mode=2;
        rotationinit(ROTATION_RIGHTHANDED); 
        buttonsetrotation();
        break;
      case 2: // lefthanded lagno
        mode=2;
        rotationinit(ROTATION_LEFTHANDED); 
        buttonsetrotation();
        break;
      case 9: // righthanded game 2
        mode=3;
        rotationinit(ROTATION_RIGHTHANDED); 
        buttonsetrotation();
        break;
      case 10: // lefthanded game 2
        mode=3;
        rotationinit(ROTATION_LEFTHANDED); 
        buttonsetrotation();
        break;
      case 5: // righthanded game 3
        mode=4;
        rotationinit(ROTATION_RIGHTHANDED); 
        buttonsetrotation();
        break;
      case 6: // lefthanded game 3
        mode=4;
        rotationinit(ROTATION_LEFTHANDED); 
        buttonsetrotation();
        break;
      case 4: // something hanging
        mode=5;
        rotationinit(ROTATION_HANGING); 
        buttonsetrotation();
        break;
      case 8: // something standing
        mode=6;
        rotationinit(ROTATION_STANDING); 
        buttonsetrotation();
        break;
      default: // slideshow hanging
        mode=7;
        rotationinit(ROTATION_HANGING); 
        buttonsetrotation();
        slideinit();
        break;
    }
  }

  switch(mode)
  {
    case 0: // configuration
      tftinit(ST77XX_RED);
      if (setupwifi(1)) setupwebserver(1); // run config webserver in access-point mode
      break;
    case 1: // upload
      tftinit(ST77XX_GREEN);
      if (setupwifi(0)) setupwebserver(0); // run upload webserver
      break;
    case 2: // lagno
      tftinit(ST77XX_WHITE);
      if (allowconcurrentupload)
      {
        if (setupwifi(0)) setupwebserver(0); // run upload webserver
      }
      initlagno();
      drawlagno(0);
      break;
    case 3: // game 2
    case 4: // game 3
    case 5: // something hanging life
      tftinit(ST77XX_BLACK);
      if (allowconcurrentupload)
      {
        if (setupwifi(0)) setupwebserver(0); // run upload webserver
      }
      allocatelifebuffers();
      initializelifebuffer();
      drawlife();
      break;
    
    case 6: // something standing
    default: // slideshow
      tftinit(ST77XX_BLACK);
      if (allowconcurrentupload)
      {
        if (setupwifi(0)) setupwebserver(0); // run upload webserver
      }
      lastslidetime=-100000; // continue with slideshow immediately
      break;
  }
}


void loop()
{
  uint32_t now=millis();
  if (now-lastledtime>100)
  {
    ledOn(ledcount);
    ledcount++;
    if (ledcount>=42) ledcount=0;
    lastledtime=now;
  }
  switch(mode)
  {
    case 0: // configuration
    case 1: // upload
      //using asynchronous webserver - so nothing here except leds
      break;
    case 2: // lagno
      dolagno();
      break;
    case 3: // game 2
    case 4: // game 3
    case 5: // something hanging
      iteratelife();
      swaplifebuffers();
      drawlife();
      break;
    case 6: // something standing
    default: // slideshow - combination with webserver causes problems
      if ((now-lastslidetime)>5000)
      {
        if (showslide(random(16))==0) tftdisplay.fillScreen(ST77XX_RED); //failure to load
        lastslidetime=now;
      }
      break;
  }
  delay(10); // no hurry
}
