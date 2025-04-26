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

double power(double x, int n){
    
    double y_temp = 1;

    if(n>1){
        for (int i = n; i > 1; i--){
        y_temp = y_temp * x;
        n--;
        }
    }else if (n>0){
        return 0;
    }

    return (y_temp);
}

int main(){

    double x = INPUT_START;
    double y = 0;
    double y1, y2, y3, y4;

    init_stream(F_CPU);
    sei();

    while (1) {
        _delay_ms(1000);
        printf("Druk een toets voor het berekenen van waarden:\n");
        getchar();
        
        printf("nu komt de grafiek van x^2\n");
        printf("x, y\n");
        for (int i = 0; i < INPUT_N; i++)
        {
            y = square(x);
            printf("%f,",x);
            printf("%f\n",y);
            x = x + INPUT_STEP;
        }

        printf("nu komt de grafiek van x^n\n");
        printf("x, y1, y2, y3, y4\n");
        for (int i = 0; i < INPUT_N; i++)
        {
            y1 = power(x,2);
            y2 = power(x,3);
            y3 = power(x,4);
            y4 = power(x,5);
            printf("%f, ",x);
            printf("%f, ",y1);
            printf("%f, ",y2);
            printf("%f, ",y3);
            printf("%f\n",y4);
            x = x + INPUT_STEP;
        }
    }
}
