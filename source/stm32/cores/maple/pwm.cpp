/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Perry Hung.
 * Copyright (c) 2011, 2012 LeafLabs, LLC.
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

/**
 * @file wirish/pwm.cpp
 * @brief Wiring-style pwmWrite().
 */

#include "pwm.h"

#include <libmaple/libmaple_types.h>
#include <libmaple/timer.h>

#include "boards.h"
#include "io.h"

void pwmWrite(uint8 pin, uint16 duty_cycle) {
    if (pin >= BOARD_NR_GPIO_PINS) {
        return;
    }
    timer_dev *dev = PIN_MAP[pin].timer_device;
    uint8 cc_channel = PIN_MAP[pin].timer_channel;
    ASSERT(dev && cc_channel);
    timer_set_compare(dev, cc_channel, duty_cycle);
}

/*
 * Roger Clark. Added new function to replicate more closely what the Arduino API does
 * Note. This implementation is currently slower than it could be, 
 * because pinMode needs to be called to set the special (new) mode of PWM
 * Some optimisation may be possible with pinMode or even in this function
 */
void analogWrite(uint8 pin, int duty_cycle8)
{
	pinMode(pin,PWM);
	pwmWrite(pin,duty_cycle8 * 257);// 257 maps 255 to 65535 (i.e 255*257 = 65535)
}
