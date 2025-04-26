/**
 * @file    main.c
 * @author  nico verduin
 * @brief   example program to get started (i.e. blinky)
 * @version 1.0
 * @date    11-4-2023
 *
 */
#define  F_CPU 2000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "serialF0.h"
#define N 16

/**
 * @brief  mainline of the program
 * @return nothing as we never get there
 */
int main(void) {

    init_stream(F_CPU);
    sei();

    // Set port D as an output
    PORTD.DIRSET = PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm;
    
    int led_output =0;


    while (1) {
        PORTD.OUTSET = led_output;
        _delay_ms(250);

        PORTD.OUTCLR = led_output;

        printf("led=%d\n",led_output);

        led_output++;
        if (led_output == N) {
            led_output = 0;
        }
    }
}
