# a macropad firmware
Firmware for RP2040-based macropad.

Custom key presses for 2 layer keypad. 10 total keys, one layer select key; 18 unique functions. At the moment it's not much of a macropad, more just a programmable keyboard. To do fancier stuff, can program obscure keypresses into the pad, and set up the OS to run scripts based on this. I don't fancy writing USB drivers to handle this more elegantly.

## Build

Requires:
- docker

```bash
./build.sh
```

Resulting uf2 file will be copied to root dir.
