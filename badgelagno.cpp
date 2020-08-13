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
#include "badgelagno.h"

// serial port has already been opened, but we also need access to tft
#include "badgetft.h"
extern Adafruit_ST7789 tftdisplay;

// we use buttons..
#include "badgebuttons.h"


/*
 * game states
 * 0 waiting for player to press button A or B, signalling start of game by human (A) or computer (B)
 * 1 waiting for move by human
 * 2 "waiting" for move by computer
 * 3 human can't move next move by computer
 * 4 computer can't move - next move by human
 * 5 waiting for move by human (computer could not move previously)
 * 6 "waiting" for move by computer (human could not move previously)
 * 7 no more moves - display score and wait for press of A or B
 */
int lagnogamestate=0;
int lagnopreviousbuttons=0;

char playfield[64]={
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,1,2,0,0,0,
  0,0,0,2,1,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0
};

char playground[64]={
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,1,2,0,0,0,
  0,0,0,2,1,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0
};

// keep track of what is displayed so we only need to change whatever fields changes
char displayfield[64]={
  3,3,3,3,3,3,3,3,
  3,3,3,3,3,3,3,3,
  3,3,3,3,3,3,3,3,
  3,3,3,3,3,3,3,3,
  3,3,3,3,3,3,3,3,
  3,3,3,3,3,3,3,3,
  3,3,3,3,3,3,3,3,
  3,3,3,3,3,3,3,3
};

// a list of potential moves and number of flips that results in from which the best move (if any) can be selected
struct playerflipslistitem
{
  byte xmove;
  byte ymove;
  byte flips;
};
playerflipslistitem playerflipslist[64];

int lagnocursorx;
int lagnocursory;

void initfield()
{
  for (int i=0; i<64; i++) { playfield[i]=0; displayfield[i]=3; }
  playfield[27]=1;
  playfield[28]=2;
  playfield[35]=2;
  playfield[36]=1;
}

int playerscore(int p)
{
  int ps=0;
  for (int i=0; i<64; i++)
  {
    if (playfield[i]==p) ps++;
  }
  return(ps);
}

int flipalongline(char * field, int x, int y, int p, int xinc, int yinc)
{
  int flips=0;
  if ( ((x+xinc)<0) || ((x+xinc)>7) || ((y+yinc)<0) || ((y+yinc)>7)) return(0); // just hit the boundary without termination, unroll recursion without actually flipping
  else
  {
    int fv=field[(x+xinc)+((y+yinc)*8)];
    if (fv==0) return(0); // just hit an empty cell, unroll recursion without actually flipping
    else if (fv==p) return(1); // hit my own color, unroll recursion while implementing flips
    else // contine recursion in this direction
    {
      flips=flipalongline(field, (x+xinc), (y+yinc), p, xinc, yinc);
      if (flips)
      {
        field[(x+xinc)+((y+yinc)*8)]=p;
        return(flips+1);
      }
      else return(0);
    }
  }
}

int domove(char * field, int x, int y, int p)
{
  int totalflips=0;
  if (field[x+(y*8)]!=0) return(0); // can't do this move, so no flips
  for (int xinc=-1; xinc<=1; xinc++)
  {
    for (int yinc=-1; yinc<=1; yinc++)
    {
      if ((xinc!=0)||(yinc!=0)) 
      { 
        int lineflips=flipalongline(field, x, y, p, xinc, yinc); //if >=0, one more than the actual number of flips
        if (lineflips) { totalflips=totalflips+lineflips-1; }
      }
    }
  }
  if (totalflips) field[x+(y*8)]=p; // actually implement the move
  return(totalflips); // does not include the count for the move, so 0 means the move is impossible
}

void copyfield(char * source, char* destination)
{
  for (int i=0; i<64; i++) destination[i]=source[i];
}

int testmove(int x, int y, int p)
{
  copyfield(playfield,playground);
  return(domove(playground,x,y,p));
}

int lagnocmpfunc (const void * a, const void * b)
{
  return ( ((playerflipslistitem *)b)->flips - ((playerflipslistitem *)a)->flips );
}

// returns the NUMBER of moves attaining a maximum score OR 0 if there are no valid moves
int checkmoves(int p)
{
  for (int y=0; y<8; y++)
  {
    for (int x=0; x<8; x++)
    {
      playerflipslist[x+(8*y)].xmove=x;
      playerflipslist[x+(8*y)].ymove=y;
      playerflipslist[x+(8*y)].flips=testmove(x,y,p);
    }
  }
  qsort(playerflipslist, 64, sizeof(playerflipslistitem), lagnocmpfunc);
  if (playerflipslist[0].flips ==0) { return(0); } // no max scoring items
  for (int i=1; i<64; i++)
  {
    if (playerflipslist[0].flips!=playerflipslist[i].flips) return(i);
  }
  // should never happen
  return(-1);
}

void dumpfield(char * field)
{
  for (int y=0; y<8; y++)
  {
    for (int x=0; x<8; x++)
    {
      int fv=field[x+(y*8)];
      if (fv==1) Serial.print("1");
      else if (fv==2) Serial.print("2");
      else Serial.print("-");
    }
    Serial.println("");
  }
}

void drawlagnocursor(void)
{
   tftdisplay.drawRect(24*lagnocursorx+24, 24*lagnocursory+8, 24, 24, ST77XX_BLACK);
}

void clearlagnocursor(void)
{
   tftdisplay.drawRect(24*lagnocursorx+24, 24*lagnocursory+8, 24, 24, 0xE71C);
}

void movelagnocursor(int xdiff, int ydiff)
{
  clearlagnocursor();
  lagnocursorx+=xdiff;
  if (lagnocursorx<0) lagnocursorx=0;
  if (lagnocursorx>7) lagnocursorx=7;
  lagnocursory+=ydiff;
  if (lagnocursory<0) lagnocursory=0;
  if (lagnocursory>7) lagnocursory=7;
  drawlagnocursor();
}

void initlagno(void)
{
  initfield();
  for (int i=0; i<=8; i++)
  {
    tftdisplay.drawLine(24, 24*i+8, 216, 24*i+8,0xE71C);
    tftdisplay.drawLine(24*i+24, 8, 24*i+24, 200,0xE71C);
  }
  lagnocursorx=0;
  lagnocursory=0;
  drawlagnocursor();
  lagnogamestate=0;
  tftdisplay.setCursor(0, 220);
  tftdisplay.setTextWrap(true);
  tftdisplay.setTextSize(2);
  tftdisplay.setTextColor(ST77XX_GREEN);
  tftdisplay.print("humAn");
  tftdisplay.setTextColor(ST77XX_BLACK);
  tftdisplay.print(" / ");
  tftdisplay.setTextColor(ST77XX_RED);
  tftdisplay.print("Badge");
  tftdisplay.setTextColor(ST77XX_BLACK);
  tftdisplay.print(" starts");
}

// score!=0 -> also calculate & draw score
void drawlagno(int s)
{
  for (int x=0; x<8; x++)
  {
    for (int y=0; y<8; y++)
    {
      if (playfield[x+(y*8)]!=displayfield[x+(y*8)])
      {
        if (playfield[x+(y*8)]==1)
        {
          tftdisplay.drawCircle(x*24+36, y*24+20, 10, ST77XX_BLACK);
          tftdisplay.fillCircle(x*24+35, y*24+19, 10, ST77XX_GREEN);
        }
        else if (playfield[x+(y*8)]==2)
        {
          tftdisplay.drawCircle(x*24+36, y*24+20, 10, ST77XX_BLACK);
          tftdisplay.fillCircle(x*24+35, y*24+19, 10, ST77XX_RED);
        }
        else
        {
          tftdisplay.fillRect(x*24+25, y*24+9, 22, 22, ST77XX_WHITE);
        }
        displayfield[x+(y*8)]=playfield[x+(y*8)];
      }
    }
  }
  if (s)
  {
    tftdisplay.fillRect(0, 220, 250, 40, ST77XX_WHITE);
    tftdisplay.setCursor(0, 220);
    tftdisplay.setTextWrap(true);
    tftdisplay.setTextSize(2);
    tftdisplay.setTextColor(ST77XX_GREEN);
    tftdisplay.print(playerscore(1));
    tftdisplay.setTextColor(ST77XX_BLACK);
    tftdisplay.print(" / ");
    tftdisplay.setTextColor(ST77XX_RED);
    tftdisplay.print(playerscore(2));
    switch (lagnogamestate)
    {
      case 0: tftdisplay.setTextColor(ST77XX_BLACK); tftdisplay.print(" START"); break;
      case 1: tftdisplay.setTextColor(ST77XX_GREEN); tftdisplay.print(" waiting..."); break;
      case 2: tftdisplay.setTextColor(ST77XX_RED); tftdisplay.print(" thinking..."); break;
      case 3: tftdisplay.setTextColor(ST77XX_GREEN); tftdisplay.print(" no move"); break;
      case 4: tftdisplay.setTextColor(ST77XX_RED); tftdisplay.print(" no move"); break;
      case 5: tftdisplay.setTextColor(ST77XX_BLACK); tftdisplay.print(" RESTART"); break;
    }
  }
}

void dolagno(void)
{
  int moves;
  int currentbuttons=buttonsalldebounced();
  int buttonflanks=(~lagnopreviousbuttons)&currentbuttons;
  lagnopreviousbuttons=currentbuttons;
  if (buttonflanks & MASKU) movelagnocursor(0,-1);
  if (buttonflanks & MASKD) movelagnocursor(0,1);
  if (buttonflanks & MASKL) movelagnocursor(-1,0);
  if (buttonflanks & MASKR) movelagnocursor(1,0);
  switch (lagnogamestate)
  {
    case 0:
      if (buttonflanks & MASKA) { lagnogamestate=1; drawlagno(1); }
      else if (buttonflanks & MASKB) { lagnogamestate=2; drawlagno(1); }
      break;
    case 1:// players turn
      if (buttonflanks & (MASKA|MASKB) )
      {
        if (testmove(lagnocursorx, lagnocursory, 1))
        {
          domove(playfield, lagnocursorx, lagnocursory, 1);
          if (checkmoves(2)) lagnogamestate=2;
          else lagnogamestate=4;
          drawlagno(1);
        }
      }
      break;
    case 2:// computers turn
      moves=checkmoves(2);
      if (moves)
      {
        int mymove=random(moves);
        domove(playfield, playerflipslist[mymove].xmove, playerflipslist[mymove].ymove, 2);
        if (checkmoves(1)) lagnogamestate=1;
        else lagnogamestate=3;
      }
      else lagnogamestate=4;
      drawlagno(1);
      break;
    case 3:// players turn but can't so automatically skip to computer
      if (checkmoves(2)) lagnogamestate=2;
      else lagnogamestate=5;
      drawlagno(1);
      break;
    case 4:// computers turn but can't so automatically skip to player
      if (checkmoves(1)) lagnogamestate=1;
      else lagnogamestate=5;
      drawlagno(1);
      break;
    default:
      if (buttonflanks & MASKA) { initfield(); lagnogamestate=1; drawlagno(1); }
      else if (buttonflanks & MASKB) { initfield(); lagnogamestate=2; drawlagno(1); }
      break;
  }
}
