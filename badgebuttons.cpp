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
#include "badgerotation.h"
#include "badgebuttons.h"

/*
 * uses millis() to debounce button presses
 * uses rotation values from badgerotation
 *     
 */
extern int rotation;

int buttonbinding[]={BUTPIN0,BUTPIN1,BUTPIN2,BUTPIN3,BUTPIN4,BUTPIN5};

void buttonsetrotation(void)
{
  switch(rotation)
  {
    case ROTATION_STANDING:
      buttonbinding[BUTA]=BUTPIN1;
      buttonbinding[BUTB]=BUTPIN0;
      buttonbinding[BUTU]=BUTPIN3;
      buttonbinding[BUTL]=BUTPIN5;
      buttonbinding[BUTR]=BUTPIN2;
      buttonbinding[BUTD]=BUTPIN4;
      break;
    case ROTATION_LEFTHANDED:
      buttonbinding[BUTA]=BUTPIN1;
      buttonbinding[BUTB]=BUTPIN0;
      buttonbinding[BUTU]=BUTPIN5;
      buttonbinding[BUTL]=BUTPIN4;
      buttonbinding[BUTR]=BUTPIN3;
      buttonbinding[BUTD]=BUTPIN2;
      break;
    case ROTATION_HANGING:
      buttonbinding[BUTA]=BUTPIN1;
      buttonbinding[BUTB]=BUTPIN0;
      buttonbinding[BUTU]=BUTPIN3;
      buttonbinding[BUTL]=BUTPIN5;
      buttonbinding[BUTR]=BUTPIN2;
      buttonbinding[BUTD]=BUTPIN4;
      break;
    default: // ROTATION_RIGHTHANDED
      buttonbinding[BUTA]=BUTPIN0;
      buttonbinding[BUTB]=BUTPIN1;
      buttonbinding[BUTU]=BUTPIN2;
      buttonbinding[BUTL]=BUTPIN3;
      buttonbinding[BUTR]=BUTPIN4;
      buttonbinding[BUTD]=BUTPIN5;
  }
}

// variables for debouncing
static int buttonspreviously=0;
static int buttonsstate=0;
static uint32_t buttontimecounts[6];
void buttoninit(void)
{
  pinMode(BUTPIN0,INPUT);
  pinMode(BUTPIN1,INPUT);
  pinMode(BUTPIN2,INPUT);
  pinMode(BUTPIN3,INPUT);
  pinMode(BUTPIN4,INPUT);
  pinMode(BUTPIN5,INPUT);
  uint32_t now=millis();
  for (int i=0; i<6; i++) buttontimecounts[i]=now;
}

int buttonsalldirect(void)
{
  int rv=0;
  for (int i=0; i<6; i++)
  {
    if (digitalRead(buttonbinding[i])==0) rv |= (1<<i); // set corresponding bit
  }
  return(rv);
}

int buttonsingledirect(int i)
{
  if (digitalRead(buttonbinding[i])==0) return(1<<i);
  else return(0);
}

int buttonsalldebounced(void)
{
  uint32_t now=millis();
  int current=buttonsalldirect();
  int mask;
  
  for (int i=0; i<6; i++)
  {
    mask=(1<<i);
    if (current&mask)
    {
      if ((buttonspreviously&mask)==0) buttontimecounts[i]=now;
      if ((now-buttontimecounts[i])>BUTTONDEBOUNCE) buttonsstate|=mask; // set corresponding bit
     }
    else
    {
      if (buttonspreviously&mask) buttontimecounts[i]=now;
      if ((now-buttontimecounts[i])>BUTTONDEBOUNCE) buttonsstate&=(63-mask); // clear corresponding bit
    }
  }
  buttonspreviously=current;
  return(buttonsstate);
}

int buttonsingledebounced(int i)
{
  uint32_t now=millis();
  int current=buttonsingledirect(i);
  int mask=(1<<i);
  if (current)
  {
    if ((buttonspreviously&mask)==0) buttontimecounts[i]=now;
    if ((now-buttontimecounts[i])>BUTTONDEBOUNCE) buttonsstate|=mask; // set corresponding bit
    buttonspreviously|=mask;
  }
  else
  {
    if (buttonspreviously&mask) buttontimecounts[i]=now;
    if ((now-buttontimecounts[i])>BUTTONDEBOUNCE) buttonsstate&=(63-mask); // clear corresponding bit
    buttonspreviously&=(63-mask);
  }
  return(buttonsstate&mask);
}
