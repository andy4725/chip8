#include <stdio.h>
#include "chip8.h"

int main() {
    Chip8 chip8;

    chip8_init(&chip8);

    chip8.V[1] = 0xAB;
    //chip8.V[2] = 0x10;

    chip8.memory[0x200] = 0x81;
    chip8.memory[0x201] = 0x2E;

    chip8_cycle(&chip8);

    printf("V1: %02X\n", chip8.V[1]);
    printf("VF: %01X\n", chip8.V[0xF]);
    return 0;
}