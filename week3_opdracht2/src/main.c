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

//constanten
#define INPUT_N 100
#define INPUT_STEP 0.01
#define INPUT_START -0.5

//functies
double square(double x){
    return(x*x);
}

int main(){

    double x = INPUT_START;
    double y = 0;

    init_stream(F_CPU);
    sei();

    while (1) {
        _delay_ms(1000);
        printf("Druk een toets voor het berekenen van waarden:\n");
        getchar();
        printf("hello there!\n");

        printf("x,y\n");
        for (int i = 0; i < INPUT_N; i++)
        {
            y = square(x);
            printf("%f,",x);
            printf("%f\n",y);
            x = x + INPUT_STEP;
        }
        _delay_ms(100);
    }
}
