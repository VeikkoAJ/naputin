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
    {0x00, KEY_1, KEY_Q, KEY_A, KEY_INT_1, 0x00}, // delta  // slash equals nordic "<""
    {KEY_F1, KEY_2, KEY_W, KEY_S, KEY_Z, KEY_LEFT_GUI},
    {KEY_F2, KEY_3, KEY_E, KEY_D, KEY_X, KEY_LEFT_ALT},
    {KEY_F3, KEY_4, KEY_R, KEY_F, KEY_C, 0x00}, // not wired
    {KEY_F4, KEY_5, KEY_T, KEY_G, KEY_V, KEY_SPACE},
    {KEY_F5, KEY_6, KEY_Y, KEY_H, KEY_B, 0x00}, // not wired
    {KEY_F6, KEY_7, KEY_U, KEY_J, KEY_N, KEY_RIGHT_ALT},
    {KEY_F7, KEY_8, KEY_I, KEY_K, KEY_M, KEY_RIGHT_GUI},
    {KEY_F8, KEY_9, KEY_O, KEY_L, KEY_COMMA, 0x00},          // omega
    {KEY_F9, KEY_0, KEY_P, KEY_SEMICOLON, KEY_PERIOD, 0x00}, // sigma
    {KEY_F10, KEY_MINUS, KEY_LEFT_BRACE, KEY_QUOTE, KEY_SLASH, KEY_RIGHT_CTRL},
    {KEY_F11, KEY_EQUAL, KEY_RIGHT_BRACE, KEY_BACKSLASH, KEY_RIGHT_SHIFT, KEY_LEFT},
    {KEY_F12, KEY_BACKSPACE, KEY_ENTER, 0x00, KEY_UP, KEY_DOWN},
    {KEY_DELETE, KEY_PAGE_UP, KEY_PAGE_DOWN, 0x00, 0x00, KEY_RIGHT}};

uint8_t keys_ms_2[15][6] = {
    // fill array with 0x00
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, KEY_HOME},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {KEY_INSERT, 0x00, 0x00, 0x00, 0x00, KEY_END}};

uint8_t keys_osx[15][6] = {
    {KEY_ESC, KEY_TILDE, KEY_TAB, KEY_CAPS_LOCK, KEY_LEFT_SHIFT, 0x00},
    {0x00, KEY_1, KEY_Q, KEY_A, KEY_INT_1, KEY_LEFT_CTRL}, // slash equals nordic "<""
    {KEY_F1, KEY_2, KEY_W, KEY_S, KEY_Z, KEY_LEFT_ALT},
    {KEY_F2, KEY_3, KEY_E, KEY_D, KEY_X, KEY_LEFT_GUI},
    {KEY_F3, KEY_4, KEY_R, KEY_F, KEY_C, 0x00},
    {KEY_F4, KEY_5, KEY_T, KEY_G, KEY_V, KEY_SPACE},
    {KEY_F5, KEY_6, KEY_Y, KEY_H, KEY_B, 0x00},
    {KEY_F6, KEY_7, KEY_U, KEY_J, KEY_N, KEY_RIGHT_GUI},
    {KEY_F7, KEY_8, KEY_I, KEY_K, KEY_M, KEY_RIGHT_ALT},
    {KEY_F8, KEY_9, KEY_O, KEY_L, KEY_COMMA, 0x00},
    {KEY_F9, KEY_0, KEY_P, KEY_SEMICOLON, KEY_PERIOD, 0x00}, // to be replaced with something else
    {KEY_F10, KEY_MINUS, KEY_LEFT_BRACE, KEY_QUOTE, KEY_SLASH, KEY_RIGHT_CTRL},
    {KEY_F11, KEY_EQUAL, KEY_RIGHT_BRACE, KEY_BACKSLASH, KEY_RIGHT_SHIFT, KEY_LEFT},
    {KEY_F12, KEY_BACKSPACE, KEY_ENTER, 0x00, KEY_UP, KEY_DOWN},
    {KEY_DELETE, KEY_PAGE_UP, KEY_PAGE_DOWN, 0x00, 0x00, KEY_RIGHT}};

uint8_t keys_osx_2[15][6] = {
    // fill array with 0x00
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};

uint8_t modifier_keys_ms[8][2] = {
    {0, 4},
    {0, 5},
    {2, 5},
    {3, 5},
    {7, 5},
    {8, 5},
    {11, 5},
    {12, 4}};

uint8_t modifier_keys_osx[8][2] = {
    {0, 4},
    {1, 5},
    {2, 5},
    {3, 5},
    {7, 5},
    {8, 5},
    {11, 5},
    {12, 4}};

uint8_t layer_2_selector_ms[2] = {1, 5};

uint8_t layer_2_selector_osx[2] = {0, 5};

int isLayer2Active(int c, int r, uint8_t layer_2_selector[2])
{
    if (c == layer_2_selector[0] && r == layer_2_selector[1])
    {
        return 1;
    }
    return 0;
}

int isModifierKey(int c, int r, uint8_t modifierKeys[8][2])
{
    for (int i = 0; i < 8; i++)
    {
        if (c == modifierKeys[i][0] && r == modifierKeys[i][1])
        {
            return 1;
        }
    }
    return 0;
};

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

uint8_t operatingSystemFlagAddress = 1;

uint8_t readOperatingSystemFlag(void)
{
    return eeprom_read_byte((uint8_t *)operatingSystemFlagAddress);
}

void writeOperatingSystemFlag(uint8_t mode)
{
    eeprom_write_byte((uint8_t *)operatingSystemFlagAddress, mode);
}