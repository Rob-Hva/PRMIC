/*!
 * \file    main.c
 * \author  Caspar Treijtel (c.treijtel@hva.nl)
 * \brief   Simple demo program for using the ucglib library by using the
 *          ucg_com_hal callback function. This program runs on the xmega
 *          microcontroller.
 * \version 0.1
 * \date    2023-10-02
 */
#define F_CPU 32000000UL

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "clock.h"
#include "serialF0.h"
#include "ucglib_xmega.h"
#include "moving_discs.h"

// Interrupt flag for letting the main program know that
// an interrupt has occurred.
volatile uint8_t iflag = 0;

// Variable used to slow down the interrupts. Only after
// INT_DIV interrupts, will the flag be set.
#define INT_DIV 16
volatile uint8_t int_div = INT_DIV;



void ucg_init(ucg_t *ucg) {
  ucg_com_fnptr ucg_xmega_func = &ucg_com_xmega_cb;
  ucg_Init(ucg, ucg_dev_st7735_18x128x160, ucg_ext_st7735_18, ucg_xmega_func);

}


void timer_init(void)
{
    TCE0.CTRLB = TC_WGMODE_NORMAL_gc;
    TCE0.CTRLA = TC_CLKSEL_DIV1_gc;
    TCE0.INTCTRLA = TC_OVFINTLVL_LO_gc;
    TCE0.PER = 31999;
}


int main(void)
{
    init_clock();
    timer_init();
    init_stream(F_CPU);

    // Set pin D0 (external LED) to output
    PORTD.DIRSET = PIN0_bm;
    // Turn off external LED
    PORTD.OUTCLR = PIN0_bm;

    // Enable interrupts globally
    sei();

    // Initialize the data structure of the UCG library and
    // draw some stuff on the display
    ucg_t ucg;
    ucg_init(&ucg);
    ucg_SetFontMode(&ucg, UCG_FONT_MODE_TRANSPARENT);
    ucg_ClearScreen(&ucg);
    ucg_SetFont(&ucg, ucg_font_8x13_mr);
    ucg_SetColor(&ucg, 0, 255, 255, 0);
    ucg_SetRotate90(&ucg);
    ucg_DrawString(&ucg, 2, 20, 0, "TEST DISPLAY [HAL]");

    ucg_SetColor(&ucg, 0, 255, 180, 180);
    ucg_DrawLine(&ucg, 20, 50, 140, 50);
    ucg_DrawLine(&ucg, 30, 54, 130, 54);
    ucg_DrawLine(&ucg, 40, 58, 120, 58);

    ucg_DrawString(&ucg, 10, 40, 0, "XMEGA256A3U SERIES");


    // For drawing moving discs on the screen a small simple
    // library is used. First some colors are defined using RGB
    // values.
    color_t c1, c2, c3;
    md_set_color(&c1, 245, 55, 135);
    md_set_color(&c2, 100, 255, 25);
    md_set_color(&c3, 20, 58, 249);

    // Now some discs are declared and initialized with the
    // colors that were defined above. Each disc gets an initial
    // position on the display.
    disc_t disc1, disc2, disc3;
    md_init_disc(&disc1, &ucg, 1, 8, &c1);
    md_init_disc(&disc2, &ucg, 2, 10, &c2);
    md_init_disc(&disc3, &ucg, 3, 6, &c3);
    md_set_disc_position(&disc1, 25, 75);
    md_set_disc_position(&disc2, X_LINES, 95);
    md_set_disc_position(&disc3, 0, 115);


    while (1) {
        if (iflag) {
            iflag = 0;

            // OPDRACHT OVER INTERRUPTS AND TIMING
            //
            // Optie 2: heartbeat signal in main
            //
            // Show 'heartbeat' signal, blinking
            // external LED on pin D0
            //
            // PORTD.OUTTGL = PIN0_bm;
            // ~Optie 2

            // Move discs
            md_move_disc(&disc1, 5, 0);
            md_move_disc(&disc2, -10, 0);
            md_move_disc(&disc3, 8, 0);

            // Print postions of the discs on the terminal (uart)
            md_print_disc_position(&disc1);
            md_print_disc_position(&disc2);
            md_print_disc_position(&disc3);
            printf("\n");
        }
    }
}

ISR(TCE0_OVF_vect)
{
    // Interrupt has occurred, decrease the interrupt divider
    int_div--;

    if (int_div == 0) {
        // Ok, enough interrupts have occurred, now the flag can be set
        iflag = 1;
        // Reset the interrupt divider for the next interrupts
        int_div = INT_DIV;

        // OPDRACHT OVER INTERRUPTS AND TIMING
        //
        // Optie 1: heartbeat signal in ISR
        //
        // Show 'heartbeat' signal, blinking
        // external LED on pin D0
        //
        // PORTD.OUTTGL = PIN0_bm;
        // ~Optie 1
    }
}
