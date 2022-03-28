/* USB configuration inspired by USB Keyboard: 
* Example for Teensy USB Development Board
* http://www.pjrc.com/teensy/usb_keyboard.html
*
*/

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#define USB_SERIAL_PRIVATE_INCLUDE
#include "usb_keyboard.h"

#define STR_MANUFACTURER L"VJ"
#define STR_PRODUCT L"naputin"

#define VENDOR_ID 0x16C0
#define PRODUCT_ID 0x047C

#define SUPPORT_ENDPOINT_HALT

#define ENDPOINT_SIZE 32

#define KEYBOARD_INTRFACE 0
#define KEYBOARD_ENDPOINT 3
#define KEYBOARD_SIZE 8
#define KEYBOARD_BUFFER EP_DOUBLE_BUFFER

static const uint8_t PROGMEM endpoint_config_table[] = {
    0,
    0,
    1, EP_TYPE_INTERRUPT_IN, EP_SIZE(KEYBOARD_SIZE) | KEYBOARD_BUFFER,
    9
};

int main(void)
{

}