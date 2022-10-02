#include <stdbool.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#include "usb_keyboard.h"

// keyboard matrix for windows and macintosh layouts
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

void jumpToBootloader(void)
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