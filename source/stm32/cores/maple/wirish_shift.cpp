/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2012 LeafLabs, LLC.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *****************************************************************************/

#include "wirish.h"

void shiftOut(uint8 dataPin, uint8 clockPin, uint8 bitOrder, uint8 value) {
    digitalWrite(clockPin, LOW);
    for (int i = 0; i < 8; i++) {
        int bit = bitOrder == LSBFIRST ? i : (7 - i);
        digitalWrite(dataPin, (value >> bit) & 0x1);
		gpio_toggle_bit(PIN_MAP[clockPin].gpio_device, PIN_MAP[clockPin].gpio_bit);// togglePin(clockPin);
		gpio_toggle_bit(PIN_MAP[clockPin].gpio_device, PIN_MAP[clockPin].gpio_bit);// togglePin(clockPin);  
    }
}

uint32_t shiftIn( uint32_t ulDataPin, uint32_t ulClockPin, uint32_t ulBitOrder )
{
  uint8_t value = 0 ;
  uint8_t i ;

  
  for ( i=0 ; i < 8 ; ++i )
  {
    digitalWrite( ulClockPin, HIGH ) ;

    if ( ulBitOrder == LSBFIRST )
    {
      value |= digitalRead( ulDataPin ) << i ;
    }
    else
    {
      value |= digitalRead( ulDataPin ) << (7 - i) ;
    }

    digitalWrite( ulClockPin, LOW ) ;
  }

  return value ;
}
