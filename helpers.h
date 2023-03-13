extern uint8_t keys_ms[15][6];
extern uint8_t keys_ms_2[15][6];
extern uint8_t keys_osx[15][6];
extern uint8_t keys_osx_2[15][6];
extern uint8_t modifier_keys_ms[8][2];
extern uint8_t modifier_keys_osx[8][2];
extern uint8_t layer_2_selector_ms[2];
extern uint8_t layer_2_selector_osx[2];

int isLayer2Active(int c, int r, uint8_t layer_2_selector_ms[2]);
int isModifierKey(int i, int b, uint8_t modifier_keys[8][2]);

uint8_t readOperatingSystemFlag(void);
void writeOperatingSystemFlag(uint8_t os);

void jumpToBootloader(void);
