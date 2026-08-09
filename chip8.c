#include "chip8.h"

#include <stdio.h>
#include <string.h>

const uint8_t font[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void chip8_init(Chip8 *chip8) {
    memset(chip8, 0, sizeof(Chip8));

    chip8->pc = 0x200;

    for(int i = 0; i < 80; i++) {
        chip8->memory[0x50 + i] = font[i];
    }
}

int chip8_load_rom(Chip8 *chip8, const char *filename) {
    FILE *file = fopen(filename, "rb");

    if(file == NULL) {
        printf("File not found.\n");
        return 0;
    }

    fread (
        &chip8->memory[0x200],
        1,
        4096 - 0x200,
        file
    );

    

    fclose(file);
    return 1;
}

void chip8_cycle(Chip8 *chip8) {
    uint16_t opcode;

    opcode = 
        chip8->memory[chip8->pc] << 8 |
        chip8->memory[chip8->pc + 1];

    printf("PC: %04X / Opcode: %04X\n", chip8->pc, opcode);

    chip8->pc += 2;

    chip8_execute(chip8, opcode);
}

void chip8_update_timers(Chip8 *chip8) {
    if(chip8->delayTimer > 0) {
        chip8->delayTimer--;
    }

    if(chip8->soundTimer > 0) {
        chip8->soundTimer--;
    }
}

void chip8_execute(Chip8 *chip8, uint16_t opcode) {
    uint16_t firstDigit = opcode >> 12;
    uint16_t lastTwoDigits = opcode & 0xFF;

    switch(firstDigit) {
        case 0x0: {
            switch(opcode) {
                case 0x00E0: { // CLEAR SCREEN
                    for(int i = 0; i < 64 * 32; i++) {
                        chip8->display[i] = 0;
                    }
                    break;
                }

                case 0x00EE: { // RETURN
                    chip8->sp--;
                    chip8->pc = chip8->stack[chip8->sp];

                    break;
                }

                default: {
                    // printf("Unknown opcode: %04X\n", opcode);
                    // break;
                }
            }
            break;
        }

        case 0x1: {
            chip8->pc = opcode & 0xFFF;
            break;
        }

        case 0x2: { // CALL SUBROUTINE
            chip8->stack[chip8->sp] = chip8->pc;
            chip8->sp++;

            uint16_t address = opcode & 0xFFF;
            chip8->pc = address;

            break;
        }

        case 0x3: {
            uint8_t x = (opcode >> 8) & 0xF;
            if(chip8->V[x] == lastTwoDigits) {
                chip8->pc += 2;
            }

            break;
        }

        case 0x4: {
            uint8_t x = (opcode >> 8) & 0xF;
            if(chip8->V[x] != lastTwoDigits) {
                chip8->pc += 2;
            }

            break;
        }

        case 0x5: {
            uint8_t x = (opcode >> 8) & 0xF;
            uint8_t y = (opcode >> 4) & 0xF;
            if ((opcode & 0xF) == 0) {
                if(chip8->V[x] == chip8->V[y]) {
                    chip8->pc += 2;
                }
            }

            break;
        }

        case 0x6: {
            uint8_t X = (opcode >> 8) & 0xF;
            uint8_t NN = opcode & 0xFF;

            chip8->V[X] = NN;
            break;
        }

        case 0x7: {
            uint8_t x = (opcode >> 8) & 0xF;
            chip8->V[x] += lastTwoDigits;
            break;
        }

        case 0xD: {
            uint8_t xRegister = (opcode >> 8) & 0xF;
            uint8_t yRegister = (opcode >> 4) & 0xF;
            uint8_t height = opcode & 0xF; 

            uint8_t xCoordinate = chip8->V[xRegister];
            uint8_t yCoordinate = chip8->V[yRegister];

            chip8->V[15] = 0;

            for(int i = 0; i < height; i++) {
                uint8_t sprite = chip8->memory[chip8->I + i];

                for(int j = 0; j < 8; j++) {
                    if(sprite & (0x80 >> j)) {
                        uint8_t pixelX = xCoordinate + j;
                        uint8_t pixelY = yCoordinate + i;

                        uint16_t index = pixelX + pixelY * 64;
                        if(chip8->display[index] == 1) {
                            chip8->V[15] = 1;
                        }
                        chip8->display[index] ^= 1;
                    }
                }
            }
            break;
        }
    
        case 0xE: {
            uint8_t x = (opcode >> 8) & 0xF;
            switch(lastTwoDigits) {
                case 0x9E: {
                    if(chip8->keypad[chip8->V[x]] == 1) {
                        chip8->pc += 2;
                    }
                    break;
                }

                case 0xA1: {
                    if(chip8->keypad[chip8->V[x]] == 0) {
                        chip8->pc += 2;
                    }
                    break;
                }
            }
            break;
        }

        case 0xF: {
            uint8_t x = (opcode >> 8) & 0xF;
            switch(lastTwoDigits) {
                case 0x07: { // SET VX TO DELAY TIMER
                    chip8->V[x] = chip8->delayTimer;
                    break;
                }

                case 0x0A: {
                    for(int key = 0; key < 16; key++) {
                        if(chip8->keypad[key]) {
                            chip8->V[x] = key;
                            return;
                        }
                    }

                    chip8->pc -= 2;
                    break;
                }

                case 0x15: { // SET DELAY TIMER TO VX
                    chip8->delayTimer = chip8->V[x];
                    break;
                }

                case 0x18: { // SET SOUND TIMER TO VX
                    chip8->soundTimer = chip8->V[x];
                    break;
                }

                case 0x1E: {
                    chip8->I += chip8->V[x];
                    break;
                }

                case 0x29: {
                    chip8->I = 0x50 + (chip8->V[x] * 5);
                    break;
                }

                case 0x33: {
                    chip8->memory[chip8->I] = chip8->V[x] / 100;
                    chip8->memory[chip8->I + 1] = chip8->V[x] % 100 / 10;
                    chip8->memory[chip8->I + 2] = chip8->V[x] % 10;

                    break;
                }

                case 0x55: {
                    for(int i = 0; i <= x; i++) {
                        chip8->memory[chip8->I + i] = chip8->V[i];
                    }
                    break;
                }

                case 0x65: {
                    for(int i = 0; i <= x; i++) {
                        chip8->V[i] = chip8->memory[chip8->I + i];
                    }
                    break;
                }
            }
            break;
        }
    }
}