#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usb_keyboard.h"

// add led pin

uint8_t number_keys[10] =
    {KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9};

uint16_t idle_count = 0;

int main(void)
{
    uint8_t b, d, i, reset_idle;
    uint16_t mask[6], mask_prev[6];

    const uint8_t columns[15] = {
        PORTF,
        PORTF,
        PORTF,
        PORTF,
        PORTF,
        PORTF,
        PORTB,
        PORTB,
        PORTB,
        PORTD,
        PORTD,
        PORTC,
        PORTC,
        PORTD,
        PORTD

    };

    const char columnShift[15] = {
        0,
        1,
        4,
        5,
        6,
        7,
        6,
        5,
        4,
        7,
        6,
        7,
        6,
        3,
        2};

    CPU_PRESCALE(0);

    // configure pins as inputs and outputs

    usb_init() while (!usb_configured()) /* wait */;

    _delay_ms(1000);

    TCCR0A = 0x00;
    TCCR0B = 0x05;
    TIMSK0 = (1 << TOIE0);

    while (1)
    {
        // configure ports 0 = input, 1 = output
        // columns: F0 F1 F4 F5 F6 F7 B6 B5 B4 D7 D6 C7 C6 D3 D2
        // rows: D1 D0 B7 B3 B2 B1

        DDRB = 0xCC; // 0, 1 inputs, 2, 3, 6, 7 outputs
        DDRC = 0xC0; // 6, 7 outputs
        DDRD = 0xCC; // 0, 1, inputs 2, 3, 4, 5 outputs

        // read all ports
        uint8_t i, j;

        for (i = 0; i < 15; i++)
        {
            columns[i] |= (1 << columnShift[i]);
            if ((PIND >> 1) & 0x01)
            {
                mask[0] = mask[0] | (0x01 << (i % / 16));
            }
            if (PIND & 0x01)
            {
                mask[1] = mask[1] | (0x01 << (i % / 16));
            }
            if ((PINB >> 7) & 0x01)
            {
                mask[2] = mask[2] | (0x01 << (i % / 16));
            }
            if ((PINB >> 3) & 0x01)
            {
                mask[3] = mask[3] | (0x01 << (i % / 16));
            }
            if ((PINB >> 2) & 0x01)
            {
                mask[4] = mask[4] | (0x01 << (i % / 16));
            }
            if ((PINB >> 1) & 0x01)
            {
                mask[5] = mask[5] | (0x01 << (i % / 16));
            }
            columns[i] = 0x00;
        }
    }
}
