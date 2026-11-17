#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <hidapi/hidapi.h>

#include "usbhid.h"
#include "uinput_wrangler.h"
#include "usb_hid_keys.h"

#define RAINBOW_ESP_VID 0x303A
#define RAINBOW_ESP_PID 0x0002

void recurse_print_info(struct hid_device_info *info)
{
    if (info == NULL)
        return;
    printf("%04X %04X\n", info->vendor_id, info->product_id);
    recurse_print_info(info->next);
}

int main(int argc, char **argv)
{
    // enumerate the devices
    hid_init();
    struct hid_device_info *info = hid_enumerate(0, 0);
    if (info == NULL)
    {
        fprintf(stderr, "error enumerating devices: %S\n", hid_error(NULL));
        return -1;
    }
    struct hid_device_info *start = info;
    char *found_path = NULL;
    while (info != NULL)
    {
        if (info->vendor_id == RAINBOW_ESP_VID && info->product_id == RAINBOW_ESP_PID)
        {
            printf("found device at %s\n", info->path);
            found_path = strdup(info->path);
            break;
        }
        info = info->next; 
    }
    // free the enumeration
    hid_free_enumeration(start);
    // check if we found smth
    if (found_path == NULL)
    {
        fprintf(stderr, "couldn't find a device\n");
        return -1;
    }
    // try to open the device
    hid_device * device = hid_open_path(found_path);
    if (device == NULL)
    {
        fprintf(stderr, "error opening device: %S\n", hid_error(NULL));
        return -1;
    }
    if (uinput_setup() != 0)
    {
        fprintf(stderr, "failed to set up fake gamepad\n");
        return -1;
    }
    // do hidy stuff 💀
    uint8_t read_buf[512];
    int8_t x = 0;
    int8_t y = 0;
    int r = 0;
    do {
        r = hid_read(device, read_buf, sizeof(read_buf));
        if (r > 0)
        {
            uint8_t report_num = read_buf[0];
            
            if (report_num == 0x01)
            {
                // try to sanity check before casting
                if (r < (1 + sizeof(kb_boot_report_t)))
                {
                    fprintf(stderr, "got %i packet, expected at least %i\n", r, (1 + sizeof(kb_boot_report_t)));
                }
                // keyboard input
                kb_boot_report_t *kb = (kb_boot_report_t *)(read_buf + 1);
                //printf("kb: %02x %02x %02x %02x %02x %02x\n", kb->keys[0], kb->keys[1], kb->keys[2], kb->keys[3], kb->keys[4], kb->keys[5]);
                uint8_t btnmask = 0;
                for (int i = 0; i < KB_BOOT_KEYS; i++)
                {
                    if (kb->keys[i] == KEY_A)
                        btnmask |= BUTTON_A;
                    if (kb->keys[i] == KEY_B)
                        btnmask |= BUTTON_B;
                    if (kb->keys[i] == KEY_C)
                        btnmask |= BUTTON_C;
                    if (kb->keys[i] == KEY_D)
                        btnmask |= BUTTON_D;
                    if (kb->keys[i] == KEY_L)
                        btnmask |= BUTTON_L;
                    if (kb->keys[i] == KEY_R)
                        btnmask |= BUTTON_R;
                    if (kb->keys[i] == KEY_S)
                        btnmask |= BUTTON_S;
                }
                uinput_setbuttons(btnmask);
            }
            else if (report_num == 0x02)
            {
                // try to sanity check before casting
                if (r < (1 + sizeof(mouse_report_t)))
                {
                    fprintf(stderr, "got %i packet, expected at least %i\n", r, (1 + sizeof(mouse_report_t)));
                }
                // mouse input
                mouse_report_t *m = (mouse_report_t *)(read_buf + 1);
                //printf("m : %02x %02x\n", m->x_delta, m->y_delta);
                x += m->x_delta;
                y += m->y_delta;
                uinput_setaxis(x, y);
            }
            else
            {
                fprintf(stderr, "unexpected report type %02x\n", read_buf[0]);
            }
        }
    }
    while (r != -1);
    uinput_teardown();
    hid_close(device);
    free(found_path);
    return 0;
}
