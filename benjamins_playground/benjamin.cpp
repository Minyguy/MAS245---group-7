#define OC1 PB1
#define OCR OCR1A
#define TIMSK TIMSK1
#define COM1A1 COM11
#define TIMER1_PWM_INIT _BV(WGM10) | _BV(WGM11) | _BV(COM1A1)
#define TIMER1_CLOCKSOURCE _BV(CS10) /* full clock */


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
    OCR = pwm;          /* Note [5] */
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
    OCR = 0;

    /* Enable OC1 as output. */
    /* DDRB er et pre-determined variabelnavn definert i avr/io.h */
    DDRB = _BV (OC1);

    /* Enable timer 1 overflow interrupt. */
    TIMSK = _BV (TOIE1);
    sei ();
}


int main(){
    timer_init ();
    /* loop forever, the interrupts are doing the rest */
    for (;;)            /* Note [7] */
        sleep_mode();
    return (0);
}
