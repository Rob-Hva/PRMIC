#define F_CPU 2000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "serialF0.h"

#define MAX_VALUE   2047                                   // only 11 bits are used
#define VCC         3.30
#define VREF        (((double) VCC) / 1.6)                 // is 2.06125

void init_adc(void)
{
  PORTA.DIRCLR     = PIN2_bm|PIN3_bm;                      // PA3 can be used for offset
  ADCA.CH0.MUXCTRL = ADC_CH_MUXPOS_PIN2_gc |               // PA2 to + channel 0
                     ADC_CH_MUXNEG_GND_MODE3_gc;           // GND to - channel 0
  ADCA.CH0.CTRL    = ADC_CH_INPUTMODE_DIFF_gc;             // channel 0 differential
  ADCA.REFCTRL     = ADC_REFSEL_INTVCC_gc;
  ADCA.CTRLB       = ADC_RESOLUTION_12BIT_gc |
                     ADC_CONMODE_bm;                       // signed conversion
  ADCA.PRESCALER   = ADC_PRESCALER_DIV16_gc;
  ADCA.CTRLA       = ADC_ENABLE_bm;
}

int16_t read_adc(void)                                     // return a signed
{
  int16_t res;                                             // is also signed

  ADCA.CH0.CTRL |= ADC_CH_START_bm;
  while ( !(ADCA.CH0.INTFLAGS & ADC_CH_CHIF_bm) ) ;
  res = ADCA.CH0.RES;
  ADCA.CH0.INTFLAGS |= ADC_CH_CHIF_bm;

  return res;
}

int main(void)
{
  int16_t res;                                             // is also signed
  double   vinp;

  init_stream(F_CPU);
  init_adc();
  sei();

  while (1) {
    res = read_adc();
    vinp = (double) res * VREF / (MAX_VALUE + 1);          // no offset
    printf("res: %4d  spanning: %5.3f V\n",  res, vinp);
    _delay_ms(200);
  }
}
