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