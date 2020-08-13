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
 
#ifndef BADGEBUTTONS_H
  #define BADBUTTONS_H

#define BUTU 0
#define BUTD 1
#define BUTL 2
#define BUTR 3
#define BUTA 4
#define BUTB 5
#define MASKU (1<<BUTU)
#define MASKD (1<<BUTD)
#define MASKL (1<<BUTL)
#define MASKR (1<<BUTR)
#define MASKA (1<<BUTA)
#define MASKB (1<<BUTB)
#define MASKDIR (0x0F)

/* 
 * ESP gpio to physical button binding
 *    right-handed:   
 *    
 *      0      2
 *           3   4
 *      1      5
 *      
 */
#define BUTPIN0 25
#define BUTPIN1 26
#define BUTPIN2 39
#define BUTPIN3 34
#define BUTPIN4 36
#define BUTPIN5 35

#define BUTTONDEBOUNCE 20

void buttonsetrotation(void);
void buttoninit(void);
int buttonsalldirect(void);
int buttonsalldebounced(void);

#endif
