#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

typedef struct {
    uint8_t memory[4096];

    uint8_t display[64 * 32];

    uint16_t pc;

    uint16_t I;

    uint16_t stack[16];
    uint8_t sp;

    uint8_t delayTimer;
    uint8_t soundTimer;

    uint8_t V[16];

    uint8_t keypad[16];
} Chip8;

void chip8_init(Chip8 *chip8);
void chip8_cycle(Chip8 *chip8);
int chip8_load_rom(Chip8 *chip8, const char *filename);
void chip8_execute(Chip8 *chip8, uint16_t opcode);
void chip8_update_timers(Chip8 *chip8);

#endif