/*!
 * \file    main.c
 * \author  Rob Beaufort
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


    ///////////////////////////////
    //  Enter endless while-loop //
    ///////////////////////////////
    printf("Entering never ending while loop now.\n");
    while (1) {
        // Toggle pin 0 of port C (from high to low or low to high)
        PORTC.OUTTGL = PIN0_bm;
        _delay_ms(1000);
    }
}
