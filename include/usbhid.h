#include <stdint.h>

#define KB_BOOT_KEYS 6
typedef struct _kb_boot_report_t
{
    uint8_t modifiers;
    uint8_t reserved;
    uint8_t keys[KB_BOOT_KEYS];
} kb_boot_report_t;

typedef struct _mouse_report_t
{
    uint8_t buttons;
    int8_t x_delta;
    int8_t y_delta;
} mouse_report_t;
