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
#include "badgeleds.h"

/* interrupts do not seem to like pinMode
volatile uint32_t myledcounter=0;
hw_timer_t * mytimer = NULL;
portMUX_TYPE mytimerMux = portMUX_INITIALIZER_UNLOCKED;


void IRAM_ATTR mytimerinterrupthandler()
{
  portENTER_CRITICAL_ISR(&mytimerMux);
  ledOn(myledcounter);
  myledcounter++;
  if (myledcounter>41) myledcounter=0;
  portEXIT_CRITICAL_ISR(&mytimerMux);
}

void setupledtimer(void)
{
  mytimer = timerBegin(0, 80, true);    // prescaler 80 -> 1 MHz counting rate
  timerAttachInterrupt(mytimer, &mytimerinterrupthandler, true);
  timerAlarmWrite(mytimer, 100000, true); // every 100000 counts -> 10 Hz interrupt rate, auto reload
  timerAlarmEnable(mytimer);            // and go
}
*/

// binding of led # 0..41 to led pin-pairs
struct led ledpins[]={
  { LME, LMA }, { LMA, LME },
  { LME, LMC }, { LMC, LME },
  { LMD, LMC }, { LMC, LMD },
  { LMD, LMA }, { LMA, LMD },
  { LMD, LMB }, { LMB, LMD },
  { LMC, LMB }, { LMB, LMC },
  { LMC, LMA }, { LMA, LMC },
  { LMB, LMA }, { LMA, LMB },
  { LMF, LMC }, { LMC, LMF },
  { LMF, LMD }, { LMD, LMF },
  { LMF, LMA }, { LMA, LMF },
  { LMF, LMB }, { LMB, LMF },
  { LMF, LME }, { LME, LMF },
  { LMG, LMF }, { LMF, LMG },
  { LMG, LMC }, { LMC, LMG },
  { LMG, LMA }, { LMA, LMG },
  { LMG, LME }, { LME, LMG },
  { LMG, LMB }, { LMB, LMG },
  { LMG, LMD }, { LMD, LMG },
  { LME, LMD }, { LMD, LME },
  { LME, LMB }, { LMB, LME }
};

void ledOff(void)
{
  pinMode(LMA,INPUT);
  pinMode(LMB,INPUT);
  pinMode(LMC,INPUT);
  pinMode(LMD,INPUT);
  pinMode(LME,INPUT);
  pinMode(LMF,INPUT);
  pinMode(LMG,INPUT);
}

void ledOn(int nr)
{
  ledOff();
  digitalWrite(ledpins[nr].h,HIGH);
  digitalWrite(ledpins[nr].l,LOW);
  pinMode(ledpins[nr].h,OUTPUT);
  pinMode(ledpins[nr].l,OUTPUT);
}
