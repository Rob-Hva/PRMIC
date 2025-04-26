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
#define MAX_STARS 20
#define DELAY 300

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


    while (1) {
        for (int i=0; i<MAX_STARS; i++) {
            for (int n=0; n<=i ; n++){
                printf("*");
            }
            printf("\n");
            _delay_ms(DELAY);
        }
        for (int i=0; i<MAX_STARS; i++){
            for (int n=MAX_STARS-i; n>0 ; n--){
                printf("*");
            }
            printf("\n");
            _delay_ms(DELAY);
        }
    }
}
