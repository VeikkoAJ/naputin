#include <stdbool.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usb_keyboard_debug.h"
#include "print.h"

// add caps-lock led pin

uint8_t keys[15][6] = {
    {KEY_ESC, KEY_TILDE, KEY_TAB, KEY_CAPS_LOCK, 0x00, 0x00},
    {0x00, KEY_1, KEY_Q, KEY_A, KEY_BACKSLASH, 0x00},
    {KEY_F1, KEY_2, KEY_W, KEY_S, KEY_Z, 0x00},
    {KEY_F2, KEY_3, KEY_E, KEY_D, KEY_X, KEY_DELTA},
    {KEY_F3, KEY_4, KEY_R, KEY_F, KEY_C, 0x00},
    {KEY_F4, KEY_5, KEY_T, KEY_G, KEY_V, KEY_SPACE},
    {KEY_F5, KEY_6, KEY_Y, KEY_H, KEY_B, 0x00},
    {KEY_F6, KEY_7, KEY_U, KEY_J, KEY_N, KEY_OMEGA},
    {KEY_F7, KEY_8, KEY_I, KEY_K, KEY_M, 0x00},
    {KEY_F8, KEY_9, KEY_O, KEY_L, KEY_COMMA, 0x00},
    {KEY_F9, KEY_0, KEY_P, KEY_SEMICOLON, KEY_PERIOD, 0x00},
    {KEY_F10, KEY_MINUS, KEY_LEFT_BRACE, KEY_QUOTE, KEY_SLASH, 0x00},
    {KEY_F11, KEY_EQUAL, KEY_RIGHT_BRACE, KEY_BACKSLASH, KEY_RIGHT_SHIFT, KEY_LEFT},
    {KEY_F12, KEY_BACKSPACE, KEY_ENTER, 0x00, KEY_UP, KEY_DOWN},
    {KEY_PAUSE, KEY_INSERT, KEY_DELETE, 0x00, 0x00, KEY_RIGHT}};

// KEY LEFT CTRL = [5][0]
// KEY LEFT SHIFT = [4][0]
// KEY LEFT ALT = [2][5]
// KEY LEFT GUI = [1][5]
// KEY RIGHT CONTROL = [11][5]
// KEY RIGHT SHIFT = [10][5]
// KEY RIGHT ALT = [8][5]
// KEY RIGHT GUI = [7][5]

#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))

uint16_t idle_count = 0;

int main(void)
{
    uint8_t reset_idle;
    bool state[6][15] = {0};
    bool state_prev[6][15] = {0};
    bool sum[6][15] = {0};
    bool keyboard_modifier_keys[6] = {0};

    char *volatile const columns[15] = {
        &PORTF,
        &PORTF,
        &PORTF,
        &PORTF,
        &PORTF,
        &PORTF,
        &PORTB,
        &PORTB,
        &PORTB,
        &PORTD,
        &PORTD,
        &PORTC,
        &PORTC,
        &PORTD,
        &PORTD};

    const char columnShift[15] = {
        0,  // F
        1,  // F
        4,  // F
        5,  // F
        6,  // F
        7,  // F
        6,  // B
        5,  // B
        4,  // B
        7,  // D
        6,  // D
        7,  // C
        6,  // C
        3,  // D
        2}; // D

    CPU_PRESCALE(0);

    usb_init();
    while (!usb_configured())
        ;
    print("usb configured\n");

    // wait for system to become ready
    _delay_ms(1000);

    uint8_t i, b;
    while (1)
    {
        print("\nmain loop started\n");
        // reset states
        for (i = 0; i < 6; i++)
        {
            for (b = 0; i < 15; i++)
            {
                state[i][b] = false;
            }
        }
        i_key = 0;
        keyboard_modifier_keys = 0x00;
        // configure ports 0 = input, 1 = output, 1 = unused (as outputs)
        // columns: F0 F1 F4 F5 F6 F7 B6 B5 B4 D7 D6 C7 C6 D3 D2
        // rows: D1 D0 B7 B3 B2 B1
        DDRB = 0b01110001; // 3,2,1 inputs, 6,5,4 outputs, 7,0 unused
        DDRC = 0b11111111; // 7,6 outputs, 5,4,3,2,1,0 unused
        DDRD = 0b11111100; // 1,0 inputs 7,3,2 outputs, 4,5,6 unused
        DDRF = 0b11111111; // inputs, 0,1,4,5,6,7 outputs, 2,3 unused

        // set all pins low
        PORTB = 0b0000000;
        PORTC = 0b0000000;
        PORTD = 0b0000000;
        PORTF = 0b0000000;

        // read all ports
        for (b = 0; b < 15; b++)
        {
            *(columns[b]) = *(columns[b]) | (0x01 << columnShift[b]); // set pin high
            if ((PIND >> 1) & 0x01)
            {
                state[0][b] = true;
            }
            if (PIND & 0x01)
            {
                state[1][b] = true;
            }
            if ((PINB >> 7) & 0x01)
            {
                state[2][b] = true;
            }
            if ((PINB >> 3) & 0x01)
            {
                state[3][b] = true;
            }
            if ((PINB >> 2) & 0x01)
            {
                state[4][b] = true;
            }
            if ((PINB >> 1) & 0x01)
            {
                state[5][b] = true;
            }
            *(columns[b]) = *(columns[b]) & ~(0x01 << columnShift[b]); // set pin low
        }
        // add sum logic for changes

        print("\nstate:\n");
        for (i = 0; i < 6; i++)
        {

            for (b = 0; b < 15; b++)
            {
                if (state[i][b])
                {
                    print("1, ");
                }
                else
                {
                    print("0, ");
                }
            }
            print("\n");
        }
        // usb_keyboard_send();
        _delay_ms(1000);
    }
}
