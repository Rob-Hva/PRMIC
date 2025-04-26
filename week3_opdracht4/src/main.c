/*!
 * \file    main.c
 * \author  your name (you@domain.com)
 * \brief   brief description of what program does
 * \version 0.1
 * \date    2023-06-26
 */

#define  F_CPU 2000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "serialF0.h"

int main(void) {

    init_stream(F_CPU);
    sei();

    PORTD.DIRCLR = PIN3_bm; // input pin for button
    PORTD.DIRSET = PIN0_bm; // output pin for led
    PORTD.OUTCLR = PIN0_bm; // led off

    while (1) {
        if (PORTD.IN & PIN3_bm)  {
           printf("ja\n");           
           PORTD.OUTTGL = PIN0_bm;
           _delay_ms(200);
        } else {
           printf("nee\n");
           _delay_ms(1000);

        }
    }
}

