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

    char *volatile const inputColumns[15] = {
        &PINF,
        &PINF,
        &PINF,
        &PINF,
        &PINF,
        &PINF,
        &PINB,
        &PINB,
        &PINB,
        &PIND,
        &PIND,
        &PINC,
        &PINC,
        &PIND,
        &PIND};

    const char inputColumnShifts[15] = {
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

    char *volatile const outputRows[6] = {
        &PORTD,
        &PORTD,
        &PORTB,
        &PORTB,
        &PORTB,
        &PORTB};

    const uint8_t outputRowShifts[6] = {
        1,
        0,
        7,
        3,
        2,
        1};

    CPU_PRESCALE(0);

    usb_init();
    while (!usb_configured())
        ;
    print("usb configured\n");

    // wait for system to become ready
    _delay_ms(1000);

    uint8_t i, b, i_key;
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
        // keyboard_modifier_keys = 0x00;
        // configure ports 0 = input, 1 = output, 1 = unused (as outputs)
        // columns (inputs): F0 F1 F4 F5 F6 F7 B6 B5 B4 D7 D6 C7 C6 D3 D2
        // rows (outputs): D1 D0 B7 B3 B2 B1
        DDRB = 0b10001111; // 3,2,1 outputs,    6,5,4       inputs,     7,0         unused
        DDRC = 0b00111111; //       outputs,    7,6         inputs,     5,4,3,2,1,0 unused
        DDRD = 0b01110011; // 1,0   outputs,    7,3,2       inputs,     4,5,6       unused
        DDRF = 0b00001100; //       outputs,    0,1,4,5,6,7 inputs,     2,3         unused

        // set all pins high
        PORTB = 0b1111111;
        PORTC = 0b1111111;
        PORTD = 0b1111111;
        PORTF = 0b1111111;

        // read all ports
        for (i = 0; i < 6; i++)
        {
            // set i:th row low
            *(outputRows[i]) &= ~(1 << outputRowShifts[i]);

            // check all columns
            for (b = 0; b < 15; b++)
            {
                state[i][b] = (*(inputColumns[b]) >> inputColumnShifts[b]) & 0x01;
            }
            // set i:th row back high
            *(outputRows[i]) |= (1 << outputRowShifts[i]);
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
