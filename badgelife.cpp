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
#include "badgelife.h"

// serial port has already been opened, but we also need access to tft
#include "badgetft.h"
extern Adafruit_ST7789 tftdisplay;

int currentlifebuffer=0;

byte * lifebuffer[2];

void allocatelifebuffers(void)
{
  lifebuffer[0]=(byte *)ps_malloc(lifeXsize*lifeYsize*sizeof(byte));
  lifebuffer[1]=(byte *)ps_malloc(lifeXsize*lifeYsize*sizeof(byte));
}

void swaplifebuffers(void)
{
  currentlifebuffer=1-currentlifebuffer;
}

void initializelifebuffer(void)
{
  if (lifebuffer[currentlifebuffer]!=NULL)
  {
    for (int y=0; y<lifeYsize;  y++)
    {
      for (int x=0; x<lifeXsize; x++)
      {
        if (random(2)) //either on or off
        {
          lifebuffer[currentlifebuffer][x+lifeXsize*y]=1+random(6); // with a choice of 6 colors
        }
        else
        {
          lifebuffer[currentlifebuffer][x+lifeXsize*y]=0; // off
        }
      }
    }
  }
}

struct colorcountitem
{
  byte color;
  byte count;
};

int lifecmpfunc (const void * a, const void * b)
{
  return ( ((colorcountitem *)b)->count - ((colorcountitem *)a)->count );
}

byte getnextstateofacell(int i, int x, int y)
{
  int xt,yt;
  colorcountitem colors[7]={{0,0},{1,0},{2,0},{3,0},{4,0},{5,0},{6,0}};
  int neighbourcount=0;
  int currentstate=lifebuffer[i][x+lifeXsize*y];
  
  for (int yy=y-1; yy<=y+1; yy++)
  {
    if (yy<0) { yt=lifeYsize-1; }
    else if (yy>=lifeYsize) { yt=0; }
    else { yt=yy; }
    for (int xx=x-1; xx<=x+1; xx++)
    {
      if (xx<0) { xt=lifeXsize-1; }
      else if (xx>=lifeXsize) { xt=0; }
      else { xt=xx; }
      if ((xt!=x)||(yt!=y))
      {
        if (lifebuffer[i][xt+lifeXsize*yt])
        {
          neighbourcount++;
          colors[lifebuffer[i][xt+lifeXsize*yt]].count++;
        }
      }
    }
  }
  if (currentstate)
  {
    if ((neighbourcount!=2)&&(neighbourcount!=3)) { return(0); }
    else { return(lifebuffer[i][x+lifeXsize*y]); }
  }
  else
  {
    if (neighbourcount==3)
    {
      //a new cell is born with the most common color along its neighbours
      qsort(colors, 7, sizeof(colorcountitem), lifecmpfunc);
      int choosefrom=1;
      for (int j=1; j<7; j++)
      {
        if (colors[j].count==colors[0].count) { choosefrom++; }
      }
      return(colors[random(choosefrom)].color);
    }
    else { return(0); }
  }
}

void iteratelife(void)
{
  if ((lifebuffer[0]!=NULL)&&(lifebuffer[1]!=NULL))
  {
    for (int y=0; y<lifeYsize;  y++)
    {
      for (int x=0; x<lifeXsize; x++)
      {
        lifebuffer[1-currentlifebuffer][x+lifeXsize*y]=getnextstateofacell(currentlifebuffer, x, y);
      }
    }
  }
}
uint16_t colorcode2color[]={0x0000,0xf800,0xffe0,0x07e0,0x07ff,0x001f,0xf81f};


void drawlife(void)
{
  if ((lifebuffer[0]!=NULL)&&(lifebuffer[1]!=NULL))
  {
    for (int y=0; y<lifeYsize;  y++)
    {
      for (int x=0; x<lifeXsize; x++)
      {
        tftdisplay.drawPixel(x, y, colorcode2color[lifebuffer[currentlifebuffer][x+lifeXsize*y]]);
      }
    }
  }
}
