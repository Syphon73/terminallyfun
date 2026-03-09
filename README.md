# CHIP-8 Emulator

A CHIP-8 emulator written in C++ using [Raylib](https://www.raylib.com/) for graphics and input.

## Build

Requires `g++` and `raylib` (with its dependencies) installed.

```bash
bash build.bash
```

## Run

```bash
./chip8
```

Loads `testROM/5-quirks.ch8` by default. Change the ROM path in `main.cpp` and rebuild to try other ROMs.

## Controls

The CHIP-8 hex keypad is mapped to your keyboard:

```
CHIP-8        Keyboard
1 2 3 C       1 2 3 4
4 5 6 D       Q W E R
7 8 9 E       A S D F
A 0 B F       Z X C V
```

## Test ROMs

The `testROM/` directory includes several ROMs for testing different emulator features (logo display, CPU flags, quirks, keypad, beep, scrolling).
