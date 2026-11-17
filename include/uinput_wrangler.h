#include <stdint.h>

#define BUTTON_A 0x01
#define BUTTON_B 0x02
#define BUTTON_C 0x04
#define BUTTON_D 0x08
#define BUTTON_L 0x10
#define BUTTON_R 0x20
#define BUTTON_S 0x40

int uinput_setup();
void uinput_setaxis(int8_t x, int8_t y);
void uinput_setbuttons(uint8_t buttmask);
int uinput_teardown();
