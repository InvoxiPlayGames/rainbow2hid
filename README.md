# rainbow2hid

Userspace Linux program to map a certain type of SDVX controller to act as a HID
gamepad by remapping its keyboard/mouse inputs.

I made this for personal usage and I'm sharing it to act as example code, please
don't ask me for support unless I know you.

## Usage on KDE

First we block KWin from processing inputs

1. Find the libinput event number for the controller with `sudo libinput list-devices`
2. Disable it with `qdbus org.kde.KWin /org/kde/KWin/InputDevice/event5 org.freedesktop.DBus.Properties.Set org.kde.KWin.InputDevice enabled false`

Then run `./rainbow2hid` and map SDVX to read from the new gamepad that appears.

Udev rule (put in `/etc/udev/rules.d/99-rainbow.rules`) to allow reading w/o
root:
* `KERNEL=="hidraw*", ATTRS{idVendor}=="303a", ATTRS{idProduct}=="0002", MODE="0666", TAG+="uaccess"`
