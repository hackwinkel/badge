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
 
#ifndef BADGELEDS_H
  #define BADGELEDS_H

/* 42 LEDs according to PCB numbering, clockwise from 12 o'clock position (D34):
 *  D34=L01 D40=L02 D14=L03 D18=L04 D13=L05 D17=L06
 *  D33=L07 D39=L08 D22=L09 D27=L10 D21=L11 D26=L12
 *  D32=L13 D38=L14 D31=L15 D37=L16 D15=L17 D19=L18
 *  D8=L19  D11=L20 D35=L21 D41=L22 D24=L23 D29=L24
 *  D3=L25  D5=L26  D1=L27  D2=L28  D16=L29 D20=L30
 *  D36=L31 D42=L32 D4=L33  D6=L34  D25=L35 D30=L36
 *  D9=L37  D12=L38 D7=L39  D10=L40 D23=L41 D28=L42
 *  
 *  LMA = R9 = gpio 33
 *  LMB = R10 = gpio 22
 *  LMC = R11 = gpio 1
 *  LMD = R12 = gpio 21
 *  LME = R13 = gpio 13
 *  LMF = R14 = gpio 32
 *  LMG = R15 = gpio 27
 *  
 *        high ->
 *       LMA LMB LMC LMD LME LMF LMG
 *   LMA --- D31 D32 D33 D34 D35 D36 
 * l LMB D37 --- D21 D22 D23 D24 D25  
 * o LMC D38 D26 --- D13 D14 D15 D16
 * w LMD D39 D27 D17 --- D7  D8  D9
 *   LME D40 D28 D18 D10 --- D3  D4
 * | LMF D41 D29 D19 D11 D5  --- D1
 * V LMG D42 D30 D20 D12 D6  D2  ---
 * 
 *        high ->
 *       LMA LMB LMC LMD LME LMF LMG
 *   LMA --- L15 L13 L07 L01 L21 L31 
 * l LMB L16 --- L11 L09 L41 L23 L35  
 * o LMC L14 L12 --- L05 L03 L17 L29
 * w LMD L08 L10 L06 --- L39 L19 L37
 *   LME L02 L42 L04 L40 --- L25 L33
 * | LMF L22 L24 L18 L20 L26 --- L27
 * V LMG L32 L36 L30 L38 L34 L28 ---
 * 
 */
#define LMA 33
#define LMB 22
#define LMC 14
#define LMD 21
#define LME 13
#define LMF 32
#define LMG 27

struct led {
  int h;
  int l;
};

void ledOff(void);
void ledOn(int nr);

#endif
