/*!
 * \file    main.c
 * \author  Rob Beaufort 
 * \brief   program that uses a accelerometor to measure an angle and dicide wich way
 *          and how fast the drawn balls on the tft-dispalay move.
 * \version 2.0
 * \date    10-12-2024
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

volatile uint8_t iflag = 0;
volatile uint8_t printflag = 0;

#define INT_DIV_DISPLAY 64
#define INT_DIV_PRINT   4
#define laagste_waarde  500
#define hoogste_waarde  1575
#define MAX_VALUE       2047
#define VCC             3.30
#define VREF            (((double) VCC) / 1.6)
#define resDivider      5

volatile uint8_t int_div_display = INT_DIV_DISPLAY;
volatile uint8_t int_div_print = INT_DIV_PRINT;


//Dit gedeelte zorgt er voor dat de spanning die de accelerometer uit komt een digitaal signaal wordt.
void init_adc(void)
{
  PORTA.DIRCLR     = PIN2_bm|PIN3_bm;
  ADCA.CH0.MUXCTRL = ADC_CH_MUXPOS_PIN2_gc |
                     ADC_CH_MUXNEG_GND_MODE3_gc;
  ADCA.CH0.CTRL    = ADC_CH_INPUTMODE_DIFF_gc;
  ADCA.REFCTRL     = ADC_REFSEL_INTVCC_gc;
  ADCA.CTRLB       = ADC_RESOLUTION_12BIT_gc |
                     ADC_CONMODE_bm;
  ADCA.PRESCALER   = ADC_PRESCALER_DIV16_gc;
  ADCA.CTRLA       = ADC_ENABLE_bm;
}

//Deze functie is voor het uitlezen van de adc en is gebaseerd op de practicum handleiding.
int16_t read_adc(void)
{
  int16_t res;

  ADCA.CH0.CTRL |= ADC_CH_START_bm;
  while ( !(ADCA.CH0.INTFLAGS & ADC_CH_CHIF_bm) ) ;
  res = ADCA.CH0.RES;
  ADCA.CH0.INTFLAGS |= ADC_CH_CHIF_bm;

  return res;
}

void ucg_init(ucg_t *ucg) {
  ucg_com_fnptr ucg_xmega_func = &ucg_com_xmega_cb;
  ucg_Init(ucg, ucg_dev_st7735_18x128x160, ucg_ext_st7735_18, ucg_xmega_func);

}


//Hier wordt de timer voor het laten knipperen van de LED en het laten bewegen van de disc's
void timer_init_display(void)
{
    TCE0.CTRLB = TC_WGMODE_NORMAL_gc;
    TCE0.CTRLA = TC_CLKSEL_DIV1_gc;
    TCE0.INTCTRLA = TC_OVFINTLVL_LO_gc;
    TCE0.PER = 31999;
}

//Hier is de timer die de waarder op het scherm laat verversen
void timer_init_printValues(void)
{
    TCE1.CTRLB = TC_WGMODE_NORMAL_gc;
    TCE1.CTRLA = TC_CLKSEL_DIV256_gc;
    TCE1.INTCTRLA = TC_OVFINTLVL_LO_gc;
    TCE1.PER = 31999;
}

//Hier worden alle waardes op het TFT display gezet
void Display_Values(ucg_t *ucg, int deg, double stapgroote, double speedX, double res, double vinp){
    
    char buffer[32];  
    sprintf(buffer, "%d", deg);
    ucg_DrawString(ucg, 60, 20, 0, buffer);

    sprintf(buffer, "%f", speedX);
    ucg_DrawString(ucg, 55, 40, 0, buffer);

}

//Hier worden alle waardes naar de SERIAL MONITOR geprint
void Print_Values( int deg, double stapgroote, double speedX, int res, double vinp, int16_t resValues[]){
    printf("graden: %d stapgroote: %f \n", deg, stapgroote);  
    printf("speedX: %f \n", speedX);
    printf("res: %4d  spanning: %5.3f V\n",  res, vinp);
    for (int i = 0; i < resDivider; i++){
        printf("resValues%d: %d\n", i, resValues[i]);
    }
}

//hier wordt de gemiddelde waarde berekent voor resDivider aantal metingen.
int getAvrgRes(int resValues[]){

    int total = 0;

    for (int i = 0; i < resDivider; i++){
        total = total + resValues[i];
    }

    int Avrg = total / resDivider;
    return Avrg;
}

int main(void)
{
    //Hier worden alle variables aangemaakt.
    int16_t resValues[resDivider-1];
    int avrgRes;
    double vinp;
    double speedX;
    double stapgroote = (hoogste_waarde - laagste_waarde) / 180.0;
    int deg = 0;
    
    //Hier worden alle functies aangeroepen.
    init_clock();
    timer_init_display();
    timer_init_printValues();
    init_stream(F_CPU);
    init_adc();
    
    //hier wordt port D0 als uitput gezet om de led te laten knipperen
    PORTD.DIRSET = PIN0_bm;
    PORTD.OUTCLR = PIN0_bm;

    sei();
    
    

    //Hier wordt alles ucg geinitialiseerd en worden er al direct dingen getekent met de ucg. 
    //Deze code is gebaseerd op de practicumhandleiding.
    ucg_t ucg;
    ucg_init(&ucg);
    ucg_SetFontMode(&ucg, UCG_FONT_MODE_TRANSPARENT);
    ucg_ClearScreen(&ucg);
    ucg_SetFont(&ucg, ucg_font_8x13_mr);
    ucg_SetColor(&ucg, 0, 255, 255, 0);
    ucg_SetRotate90(&ucg);

    ucg_SetColor(&ucg, 0, 255, 255, 255);
    ucg_DrawLine(&ucg, 30, 54, 130, 54);
    ucg_DrawLine(&ucg, 40, 58, 120, 58);
    ucg_DrawLine(&ucg, 10, 50, 150, 50);

    ucg_DrawString(&ucg, 2, 20, 0, "graden:");
    ucg_DrawString(&ucg, 2, 40, 0, "speedX:");

    //hier worden er kleuren gedefinieerd als C1, C2 en C3
    color_t c1, c2, c3;
    md_set_color(&c1, 245, 55, 135);
    md_set_color(&c2, 100, 255, 25);
    md_set_color(&c3, 20, 58, 249);

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

            //Hier worden de waardes op het TFT verwijderd door deze opnieuw in het zwart te printen
            ucg_SetColor(&ucg, 0, 0, 0, 0);
            Display_Values(&ucg, deg, stapgroote, speedX, avrgRes, vinp);

            //Hier worden alle waardes van de variabelen berekent
            for (int i = 0; i < resDivider; i++){
                resValues[i] = read_adc();
            }
            avrgRes = getAvrgRes(resValues);
            vinp = (double) avrgRes * VREF / (MAX_VALUE + 1);   
            deg = (avrgRes-laagste_waarde) / stapgroote;
            speedX = (deg-90)/ 30.0;

            //Hier worden de waardes in het wit op het TFT display gezet
            ucg_SetColor(&ucg, 0, 255, 255, 255);
            Display_Values(&ucg, deg, stapgroote, speedX, avrgRes, vinp);

            //hier worden alle ballen verplaatst
            md_move_disc(&disc1, -5*speedX, 0);
            md_move_disc(&disc2, -10*speedX, 0);
            md_move_disc(&disc3, -8*speedX, 0);

        }

        if (printflag){
            printflag = 0;

            Print_Values(deg, stapgroote, speedX, avrgRes, vinp, resValues);

        } 
    }
}

ISR(TCE0_OVF_vect){
    int_div_display--;

    if (int_div_display == 0) {
        iflag = 1;
        int_div_display = INT_DIV_DISPLAY;

        PORTD.OUTTGL = PIN0_bm;
    }
}
ISR(TCE1_OVF_vect){
    int_div_print--;

    if (int_div_print == 0){
        printflag = 1;
        int_div_print = INT_DIV_PRINT;
    }
}