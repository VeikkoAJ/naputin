#include <stdbool.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include "usb_keyboard.h"

// zeros as empty array places
uint8_t keys_ms[15][6] = {
    {KEY_ESC, KEY_TILDE, KEY_TAB, KEY_CAPS_LOCK, KEY_LEFT_SHIFT, KEY_LEFT_CTRL},
    {0x00, KEY_1, KEY_Q, KEY_A, KEY_INT_1, KEY_LEFT_GUI}, // slash equals nordic "<""
    {KEY_F1, KEY_2, KEY_W, KEY_S, KEY_Z, KEY_LEFT_ALT},
    {KEY_F2, KEY_3, KEY_E, KEY_D, KEY_X, KEY_DELTA},
    {KEY_F3, KEY_4, KEY_R, KEY_F, KEY_C, 0x00},
    {KEY_F4, KEY_5, KEY_T, KEY_G, KEY_V, KEY_SPACE},
    {KEY_F5, KEY_6, KEY_Y, KEY_H, KEY_B, 0x00},
    {KEY_F6, KEY_7, KEY_U, KEY_J, KEY_N, KEY_OMEGA},
    {KEY_F7, KEY_8, KEY_I, KEY_K, KEY_M, KEY_RIGHT_GUI},
    {KEY_F8, KEY_9, KEY_O, KEY_L, KEY_COMMA, KEY_RIGHT_ALT},
    {KEY_F9, KEY_0, KEY_P, KEY_SEMICOLON, KEY_PERIOD, KEY_SIGMA},
    {KEY_F10, KEY_MINUS, KEY_LEFT_BRACE, KEY_QUOTE, KEY_SLASH, KEY_RIGHT_CTRL},
    {KEY_F11, KEY_EQUAL, KEY_RIGHT_BRACE, KEY_BACKSLASH, KEY_RIGHT_SHIFT, KEY_LEFT},
    {KEY_F12, KEY_BACKSPACE, KEY_ENTER, 0x00, KEY_UP, KEY_DOWN},
    {KEY_DELETE, KEY_PAGE_UP, KEY_PAGE_DOWN, 0x00, 0x00, KEY_RIGHT}};

uint8_t keys_osx[15][6] = {
    {KEY_ESC, KEY_TILDE, KEY_TAB, KEY_CAPS_LOCK, KEY_LEFT_SHIFT, KEY_DELTA},
    {0x00, KEY_1, KEY_Q, KEY_A, KEY_INT_1, KEY_LEFT_CTRL}, // slash equals nordic "<""
    {KEY_F1, KEY_2, KEY_W, KEY_S, KEY_Z, KEY_LEFT_ALT},
    {KEY_F2, KEY_3, KEY_E, KEY_D, KEY_X, KEY_LEFT_GUI},
    {KEY_F3, KEY_4, KEY_R, KEY_F, KEY_C, 0x00},
    {KEY_F4, KEY_5, KEY_T, KEY_G, KEY_V, KEY_SPACE},
    {KEY_F5, KEY_6, KEY_Y, KEY_H, KEY_B, 0x00},
    {KEY_F6, KEY_7, KEY_U, KEY_J, KEY_N, KEY_RIGHT_GUI},
    {KEY_F7, KEY_8, KEY_I, KEY_K, KEY_M, KEY_RIGHT_ALT},
    {KEY_F8, KEY_9, KEY_O, KEY_L, KEY_COMMA, KEY_OMEGA},
    {KEY_F9, KEY_0, KEY_P, KEY_SEMICOLON, KEY_PERIOD, KEY_RIGHT_CTRL},
    {KEY_F10, KEY_MINUS, KEY_LEFT_BRACE, KEY_QUOTE, KEY_SLASH, KEY_RIGHT_CTRL},
    {KEY_F11, KEY_EQUAL, KEY_RIGHT_BRACE, KEY_BACKSLASH, KEY_RIGHT_SHIFT, KEY_LEFT},
    {KEY_F12, KEY_BACKSPACE, KEY_ENTER, 0x00, KEY_UP, KEY_DOWN},
    {KEY_DELETE, KEY_PAGE_UP, KEY_PAGE_DOWN, 0x00, 0x00, KEY_RIGHT}};

/*
4
R
F

<

*/
int a = 0x53;

#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))

uint16_t idle_count = 0;
// KEY LEFT SHIFT = [0][4]
// KEY LEFT CTRL = [0][5]
// KEY LEFT GUI = [1][5]
// KEY LEFT ALT [2][5]

// KEY RIGHT ALT = [8][5]
// KEY RIGHT GUI = [9][5]
// KEY RIGHT CONTROL = [11][5]
// KEY RIGHT SHIFT = [12][4]

void jumpToBootLoader()
{
    cli();
    UDCON = 1;
    USBCON = (1 << FRZCLK); // disable USB
    UCSR1B = 0;
    EIMSK = 0;
    PCICR = 0;
    SPCR = 0;
    ACSR = 0;
    EECR = 0;
    ADCSRA = 0;
    TIMSK0 = 0;
    TIMSK1 = 0;
    TIMSK3 = 0;
    TIMSK4 = 0;
    UCSR1B = 0;
    TWCR = 0;
    DDRB = 0;
    DDRC = 0;
    DDRD = 0;
    DDRE = 0;
    DDRF = 0;
    TWCR = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    PORTE = 0;
    PORTF = 0;
    asm volatile("jmp 0x7E00");
}

int isModifierKey(int i, int b)
{
    uint8_t key = 0x00;
    if (b == 0 && i == 4)
    {

        key = KEY_LEFT_SHIFT;
    }
    else if (b == 0 && i == 5)
    {
        key = KEY_LEFT_CTRL;
    }
    else if (b == 1 && i == 5)
    {
        key = KEY_LEFT_GUI;
    }
    else if (b == 2 && i == 5)
    {
        key = KEY_LEFT_ALT;
    }
    else if (b == 8 && i == 5)
    {
        key = KEY_RIGHT_ALT;
    }
    else if (b == 9 && i == 5)
    {
        key = KEY_RIGHT_GUI;
    }
    else if (b == 11 && i == 5)
    {
        key = KEY_RIGHT_CTRL;
    }
    else if (b == 12 && i == 4)
    {
        key = KEY_RIGHT_SHIFT;
    }
    if (key != 0x00)
    {
        return 1;
    }
    return 0;
}

int main(void)
{
    uint8_t reset_idle;
    uint8_t bootloaderCounter = 0;
    bool state[6][15] = {0};
    bool state_prev[6][15] = {0};
    bool sum[6][15] = {0};

    char *volatile const inputColumns[15] = {
        &PIND,  // D
        &PIND,  // D
        &PINC,  // C
        &PINC,  // C
        &PIND,  // D
        &PIND,  // D
        &PINB,  // B
        &PINB,  // B
        &PINB,  // B
        &PINF,  // F
        &PINF,  // F
        &PINF,  // F
        &PINF,  // F
        &PINF,  // F
        &PINF}; // F

    const uint8_t inputColumnShifts[15] = {
        2,  // 2
        3,  // 3
        6,  // 6
        7,  // 7
        6,  // 6
        7,  // 7
        4,  // 4
        5,  // 5
        6,  // 6
        7,  // 7
        6,  // 6
        5,  // 5
        4,  // 4
        1,  // 1
        0}; // 0

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
    // wait for system to become ready
    _delay_ms(1000);
    // Configure timer 0 to generate a timer overflow interrupt every
    // 256*1024 clock cycles, or approx 61 Hz when using 16 MHz clock
    TCCR0A = 0x00;
    TCCR0B = 0x05;
    TIMSK0 = (1 << TOIE0);

    // configure ports 0 = input, 1 = output, 1 = unused (as outputs)
    // columns (inputs): F0 F1 F4 F5 F6 F7 B6 B5 B4 D7 D6 C7 C6 D3 D2
    // rows (outputs): D1 D0 B7 B3 B2 B1 B0
    DDRB = 0b10001111; // 3,2,1,0 outputs,    6,5,4       inputs,     7         unused
    DDRC = 0b00111111; //         outputs,    7,6         inputs,     5,4,3,2,1,0 unused
    DDRD = 0b00110011; // 1,0     outputs,    7,6,3,2     inputs,     4,5,        unused
    DDRF = 0b00001100; //         outputs,    0,1,4,5,6,7 inputs,     2,3         unused

    // startup done blink indicator
    PORTB |= 0x01;
    _delay_ms(100);
    PORTB &= ~(0x01);
    _delay_ms(50);
    PORTB |= 0x01;
    _delay_ms(100);
    PORTB &= ~(0x01);
    while (1)
    {
        print("\nmain loop started\n");
        // set all pins high expect led pin
        PORTB = 0xfe;
        PORTC = 0xff;
        PORTD = 0xff;
        PORTF = 0xff;

        // set capslock led high
        PORTB |= (keyboard_leds >> 1 & 0x01);

        // read all ports
        for (int i = 0; i < 6; i++)
        {
            // set i:th row low
            *(outputRows[i]) &= ~(0x01 << outputRowShifts[i]);
            // check all columns
            for (int b = 0; b < 15; b++)
            {
                state[i][b] = (*(inputColumns[b]) >> inputColumnShifts[b]) & 0x01;
            }
            // set i:th row back high
            *(outputRows[i]) |= (0x01 << outputRowShifts[i]);
        }
        // sum logic for tracking changes
        for (int i = 0; i < 6; i++)
        {
            for (int b = 0; b < 15; b++)
            {
                if (state[i][b] && state_prev[i][b])
                {
                    reset_idle = 1;
                    if (isModifierKey(i, b))
                    {
                        // reversed because of pull up
                        keyboard_modifier_keys &= ~(keys_ms[b][i]);
                    }
                    else
                    {
                        for (int d = 0; d < 6; d++)
                        {
                            if (keyboard_keys[d] == keys_ms[b][i])
                            {
                                // reversed because of pull up
                                keyboard_keys[d] = 0x00;
                                d = 5;
                            }
                        }
                    }
                }
                if (!state[i][b] && !state_prev[i][b])
                {
                    if (isModifierKey(i, b))
                    {
                        // reversed because of pull up
                        keyboard_modifier_keys |= keys_ms[b][i];
                    }
                    else
                    {
                        for (int d = 0; d < 6; d++)
                        {
                            if (keyboard_keys[d] == 0x00)
                            {
                                // reversed because of pull up
                                keyboard_keys[d] = keys_ms[b][i];
                                d = 5;
                            }
                        }
                    }
                }
                state_prev[i][b] = state[i][b];
            }
        }

        if (!state[0][0] & !state[0][14])
        {
            for (int d = 0; d < 3; d++)
            {
                PORTB |= 0x01;
                _delay_ms(100);
                PORTB &= ~(0x01);
                _delay_ms(100);
            }
            bootloaderCounter++;
            if (bootloaderCounter > 2)
            {
                jumpToBootLoader();
            }
        }
        else
        {
            bootloaderCounter = 0;
        }
        _delay_ms(1);
        if (reset_idle)
        {
            cli();
            idle_count = 0;
            sei();
            _delay_ms(2);
        }
        usb_keyboard_send();
    }
}
// This interrupt routine is run approx 61 times per second.
// A very simple inactivity timeout is implemented, where we
// will send a space character.
ISR(TIMER0_OVF_vect)
{
    idle_count++;
    if (idle_count > 61 * 8)
    {
        idle_count = 0;
        usb_keyboard_send();
    }
}
