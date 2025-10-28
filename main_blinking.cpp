#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>



int main(){

// Set PC5 (pin 28 on PDIP) as output
    DDRC |= (1 << DDC5);

    while (1) {
        // Turn LED ON (PC5 HIGH)
        PORTC |= (1 << PC5);
        _delay_ms(50);

        // Turn LED OFF (PC5 LOW)
        PORTC &= ~(1 << PC5);
        _delay_ms(50);
    }
  return 0;
}