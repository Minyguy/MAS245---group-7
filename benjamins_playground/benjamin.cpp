#define OC1 PB1
#define TIMER1_PWM_INIT _BV(WGM10) | _BV(WGM11) | _BV(COM1A1)
#define TIMER1_CLOCKSOURCE _BV(CS10) /* full clock */
/* _BV betyr bitvalue og er tilsvarer et bit, som er definert ved navn på de forskjellige hardwarene
   hvor f.ex COM1A1 kan 
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>


enum { UP, DOWN };


ISR (TIMER1_OVF_vect)       /* Note [2] */
{
    static uint16_t pwm;    /* Note [3] */
    static uint8_t direction;
    switch (direction)      /* Note [4] */
    {
        case UP:
            if (++pwm == TIMER1_TOP)
                direction = DOWN;
            break;
        case DOWN:
            if (--pwm == 0)
                direction = UP;
            break;
    }
    OCR1A = pwm;          /* Note [5] */
}

void timer_init(){
    /* Timer 1 is 10-bit PWM (8-bit PWM on some ATtinys). */
    TCCR1A = TIMER1_PWM_INIT;
    /*
     * Start timer 1.
     *
     * NB: TCCR1A and TCCR1B could actually be the same register, so
     * take care to not clobber it.
     */
    TCCR1B |= TIMER1_CLOCKSOURCE;

    /* Set PWM value to 0. */
    OCR1A = 0;

    /* Enable OC1 as output. */
    /* DDRB er et pre-determined variabelnavn definert i avr/io.h */
    DDRB = _BV (PB1);

    /* Enable timer 1 overflow interrupt. */
    TIMSK1 = _BV (TOIE1);
    sei ();
}


int main(){
    timer_init ();
    /* loop forever, the interrupts are doing the rest */
    for (;;)            /* Note [7] */
        sleep_mode();
    return (0);
}
