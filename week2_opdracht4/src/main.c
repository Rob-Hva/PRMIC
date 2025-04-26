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

/*!
 * \brief  main starting point of the program.
 * \return int (with embedded this will never happen)
 */
int main()
{
    /////////////////////
    //  Initialization //
    /////////////////////

    // Initialize the stream to be able to print with the UART
    init_stream(F_CPU);
    // Enable global interrupts
    sei();

    // Set pin 0 on port C as output
    PORTC.DIRSET = PIN0_bm;
    PORTF.DIRSET = PIN0_bm;
    PORTF.DIRSET = PIN1_bm;
    int timer =0;

    while (1) {

        if ( timer < 4 ) {
            PORTF.OUTCLR = PIN0_bm; //groen uit
            PORTF.OUTSET = PIN1_bm; //rood aan
        } else if ( timer < 7 ) {
            PORTF.OUTCLR = PIN1_bm; //rood uit
            PORTF.OUTSET = PIN0_bm; //groen aan
        } else if ( timer < 8 ){
            //PORTF.OUTCLR = PIN0_bm; //groen uit
            PORTF.OUTSET = PIN1_bm; //rood aan
        } else {
            timer = 0; //reset
        }

        timer++;
        printf("%d",timer);
        _delay_ms(1000);
    }
}
