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
#include "tftbangspi.h"

//c==0 -> command, c==1 -> data
void bangbyte(int b, int c)
{
  digitalWrite(CS,LOW);
  digitalWrite(DC,c);
  digitalWrite(DO,(b>>7)&1);
  digitalWrite(CL,HIGH);
  digitalWrite(CL,LOW);
  digitalWrite(DO,(b>>6)&1);
  digitalWrite(CL,HIGH);
  digitalWrite(CL,LOW);
  digitalWrite(DO,(b>>5)&1);
  digitalWrite(CL,HIGH);
  digitalWrite(CL,LOW);
  digitalWrite(DO,(b>>4)&1);
  digitalWrite(CL,HIGH);
  digitalWrite(CL,LOW);
  digitalWrite(DO,(b>>3)&1);
  digitalWrite(CL,HIGH);
  digitalWrite(CL,LOW);
  digitalWrite(DO,(b>>2)&1);
  digitalWrite(CL,HIGH);
  digitalWrite(CL,LOW);
  digitalWrite(DO,(b>>1)&1);
  digitalWrite(CL,HIGH);
  digitalWrite(CL,LOW);
  digitalWrite(DO,b&1);
  digitalWrite(CL,HIGH);
  digitalWrite(CL,LOW);
  digitalWrite(DC,LOW);
  digitalWrite(CS,HIGH);
}

void setuptft(void)
{
  digitalWrite(RE,LOW);
  pinMode(RE,OUTPUT);
  digitalWrite(CS,HIGH);
  pinMode(CS,OUTPUT);
  digitalWrite(DC,LOW);
  pinMode(DC,OUTPUT);
  digitalWrite(CL,LOW);
  pinMode(CL,OUTPUT);
  digitalWrite(DO,LOW);
  pinMode(DO,OUTPUT);
  //Serial.println("tft reset");
  digitalWrite(RE,LOW);
  delay(1);
  //Serial.println("tft reset clear");
  digitalWrite(RE,HIGH);
  delay(200);
  //Serial.println("tft sw reset");
  bangbyte(0x01,0);
  delay(200);
  //Serial.println("tft wake");
  bangbyte(0x11,0);
  delay(10);
  //Serial.println("tft colmode");
  bangbyte(0x3a,0);
  bangbyte(0x55,1);
  delay(10);
  //Serial.println("tft memory ctl");
  bangbyte(0x36,0);
  bangbyte(0x08,1);
  delay(10);
  //Serial.println("tft caset");
  bangbyte(0x2a,0);
  bangbyte(0x00,1);
  bangbyte(0x00,1);
  bangbyte(0x00,1);
  bangbyte(0xf0,1);
  delay(10);
  //Serial.println("tft raset");
  bangbyte(0x2b,0);
  bangbyte(0x00,1);
  bangbyte(0x00,1);
  bangbyte(0x00,1);
  bangbyte(0xf0,1);
  delay(10);
  //Serial.println("tft inverted");
  bangbyte(0x21,0);
  delay(10);
  //Serial.println("tft full");
  bangbyte(0x13,0);
  delay(10);
  //Serial.println("tft on");
  bangbyte(0x29,0);
  delay(10);
}
