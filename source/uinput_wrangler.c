#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/uinput.h>

#include "uinput_wrangler.h"

// much thanks to https://github.com/GrantEdwards/uinput-joystick-demo !

static int __fd = -1;

int uinput_setup()
{
    // open the uinput device
    __fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (__fd < 0)
    {
        perror("failed to open uinput");
        return -1;
    }
    // todo: error checking for the rest of these (if needed)
    // set up the inputs we provide
    // key inputs: a (A), b (B), x (C), y (D), lb (FX-L), rb (FX-R), start (START)
    ioctl(__fd, UI_SET_EVBIT, EV_KEY);
    ioctl(__fd, UI_SET_KEYBIT, BTN_A);
    ioctl(__fd, UI_SET_KEYBIT, BTN_B);
    ioctl(__fd, UI_SET_KEYBIT, BTN_X);
    ioctl(__fd, UI_SET_KEYBIT, BTN_Y);
    ioctl(__fd, UI_SET_KEYBIT, BTN_TL);
    ioctl(__fd, UI_SET_KEYBIT, BTN_TR);
    ioctl(__fd, UI_SET_KEYBIT, BTN_START);
    // analog inputs: l-x (VOL-L), l-y (VOL-R)
    ioctl(__fd, UI_SET_EVBIT, EV_ABS);
    struct uinput_abs_setup setup_x = {
        .code = ABS_X,
        .absinfo = { .minimum = -128, .maximum = 127 }
    };
    ioctl(__fd, UI_SET_ABSBIT, setup_x.code);
    ioctl(__fd, UI_ABS_SETUP, &setup_x);
    struct uinput_abs_setup setup_y = {
        .code = ABS_Y,
        .absinfo = { .minimum = -128, .maximum = 127 }
    };
    ioctl(__fd, UI_SET_ABSBIT, setup_y.code);
    ioctl(__fd, UI_ABS_SETUP, &setup_y);
    // set up the actual device
    struct uinput_setup dev_setup = {
        .name = "rainbow2hid",
        .id = {
            .bustype = BUS_USB,
            .vendor = 0x1337,
            .product = 0x6767,
            .version = 0x0002,
        }   
    };
    if (ioctl(__fd, UI_DEV_SETUP, &dev_setup))
    {
        perror("failed to set up uinput device");
        return -1;
    }
    if (ioctl(__fd, UI_DEV_CREATE))
    {
        perror("failed to create uinput device");
        return -1;
    }
    return 0;
}

int uinput_teardown()
{
    // should we be error checking here? i guess to be polite...
    // TODO error check here
    ioctl(__fd, UI_DEV_DESTROY);
    return 0;
}

void uinput_setbuttons(uint8_t buttmask)
{
    // batch set all buttons at once
    struct input_event ev[8] = {0}; // 7 buttons + 1 sync
#define SET_EV_KEY(num, codev, valuev) { \
    ev[num].type = EV_KEY; \
    ev[num].code = codev; \
    ev[num].value = valuev; \
}
    SET_EV_KEY(0, BTN_A, !!(buttmask & BUTTON_A));
    SET_EV_KEY(1, BTN_B, !!(buttmask & BUTTON_B));
    SET_EV_KEY(2, BTN_X, !!(buttmask & BUTTON_C));
    SET_EV_KEY(3, BTN_Y, !!(buttmask & BUTTON_D));
    SET_EV_KEY(4, BTN_TL, !!(buttmask & BUTTON_L));
    SET_EV_KEY(5, BTN_TR, !!(buttmask & BUTTON_R));
    SET_EV_KEY(6, BTN_START, !!(buttmask & BUTTON_S));
#undef SET_EV_KEY
    ev[7].type = EV_SYN;
    ev[7].code = SYN_REPORT;
    ev[7].value = 0;
    // send the event
    if (write(__fd, &ev, sizeof(ev)) < 0)
    {
        perror("failed to write key event");
    }
}

void uinput_setaxis(int8_t x, int8_t y)
{
    // batch set both axis at once
    struct input_event ev[3] = {0}; // 2 axis + 1 sync
    ev[0].type = EV_ABS;
    ev[0].code = ABS_X;
    ev[0].value = x;
    ev[1].type = EV_ABS;
    ev[1].code = ABS_Y;
    ev[1].value = y;
    ev[2].type = EV_SYN;
    ev[2].code = SYN_REPORT;
    ev[2].value = 0;
    // send the event
    if (write(__fd, &ev, sizeof(ev)) < 0)
    {
        perror("failed to write axis event");
    }
}
