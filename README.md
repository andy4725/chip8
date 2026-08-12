# CHIP-8 Emulator

A CHIP-8 emulator written in C using SDL3.

## Features

- CHIP-8 CPU emulation
- 4 KB memory
- 16 general-purpose 8-bit registers
- 16-level stack
- 64×32 monochrome display
- CHIP-8 timers
- Keyboard input
- Sound timer and audio output
- ROM loading
- SDL3 rendering and audio

## Controls

The emulator uses the standard CHIP-8 keypad mapped to the keyboard:

| CHIP-8 | Keyboard |
|--------|----------|
| 1 2 3 C | 1 2 3 4 |
| 4 5 6 D | Q W E R |
| 7 8 9 E | A S D F |
| A 0 B F | Z X C V |

## Building

### Requirements

- macOS
- GCC
- Homebrew
- SDL3

Install SDL3 with:

```bash
brew install sdl3

Compile with "make"
Clean with "make clean"
Run with "make run ROM=path/to/rom.ch8
You can also run the exe directly: ./chip8 path/to/rom.ch8

The emulator has been tested with:
- CHIP-8 opcode tests
- Timer tests
- Keyboard input tests
- Sound tests
- Multiple CHIP-8 games

Games tested include:

- Breakout
- Pong
- Tetris

You can press M to reset the emulator.