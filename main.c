#include <stdio.h>
#include "chip8.h"

int main() {
    Chip8 chip8;

    chip8_init(&chip8);

    chip8.I = 0x300;

    chip8.V[3] = 0x42;

    printf("PC: %04X\n", chip8.pc);

    chip8.memory[0x200] = 0x33;
    chip8.memory[0x201] = 0x42;

    chip8_cycle(&chip8);

    printf("PC: %04X\n", chip8.pc);
    return 0;
}