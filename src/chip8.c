#include "chip8.h"

#include <stdio.h>
#include <stdlib.h>
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

    strncpy(chip8->romPath, filename, sizeof(chip8->romPath) - 1);
    chip8->romPath[sizeof(chip8->romPath) - 1] = '\0';

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    if(size > 4096 - 0x200) {
        printf("ROM is too large.\n");
        fclose(file);
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

    //printf("PC: %04X / Opcode: %04X\n", chip8->pc, opcode);

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
    //uint16_t lastDigit = opcode & 0xF;

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
                    if(chip8->sp == 0) {
                        printf("Stack underflow\n");
                        return;
                    }

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
            if(chip8->sp >= 16) {
                printf("Stack overflow\n");
                return;
            }

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

        case 0x8: {
            uint8_t x = (opcode >> 8) & 0xF;
            uint8_t y = (opcode >> 4) & 0xF;

            switch(opcode & 0xF) {
                case 0x0: {
                    chip8->V[x] = chip8->V[y];
                    break;
                }

                case 0x1: {
                    chip8->V[x] |= chip8->V[y];
                    break;
                }

                case 0x2: {
                    chip8->V[x] &= chip8->V[y];
                    break;
                }

                case 0x3: {
                    chip8->V[x] ^= chip8->V[y];
                    break;
                }

                case 0x4: {
                    uint16_t sum = chip8->V[x] + chip8->V[y];

                    chip8->V[0xF] = sum > 0xFF;
                    chip8->V[x] = sum & 0xFF;
                    break;
                }

                case 0x5: {
                    chip8->V[0xF] = chip8->V[x] >= chip8->V[y];
                    chip8->V[x] -= chip8->V[y];

                    break;
                }

                case 0x6: {
                    chip8->V[0xF] = chip8->V[x] & 0x1;
                    chip8->V[x] >>= 0x1;

                    break;
                }
            
                case 0x7: {
                    chip8->V[0xF] = chip8->V[y] >= chip8->V[x];
                    chip8->V[x] = chip8->V[y] - chip8->V[x];

                    break;
                }                
            
                case 0xE: {
                    chip8->V[0xF] = (chip8->V[x] >> 7) & 0x1;
                    chip8->V[x] <<= 0x1;

                    break;
                }
            }
            break;
        }

        case 0x9: {
            uint8_t x = (opcode >> 8) & 0xF;
            uint8_t y = (opcode >> 4) & 0xF;

            switch(opcode & 0xF) {
                case 0x0: {
                    if(chip8->V[x] != chip8->V[y]) {
                        chip8->pc += 2;
                    }

                    break;
                }
            }
            break;
        }

        case 0xA: {
            chip8->I = opcode & 0xFFF;

            break;
        }

        case 0xB: {
            chip8->pc = (opcode & 0xFFF) + chip8->V[0];

            break;
        }

        case 0xC: {
            uint8_t x = (opcode >> 8) & 0xF;
            chip8->V[x] = (uint8_t)rand() & lastTwoDigits;

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
                if(chip8->I + i >= 4096) {
                    break;
                }

                uint8_t sprite = chip8->memory[chip8->I + i];

                for(int j = 0; j < 8; j++) {
                    if(sprite & (0x80 >> j)) {
                        uint8_t pixelX = (xCoordinate + j) % 64;
                        uint8_t pixelY = (yCoordinate + i) % 32;

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
                    if(chip8->I < 4096) {
                        chip8->memory[chip8->I] = chip8->V[x] / 100;
                    }

                    if(chip8->I + 1 < 4096) {
                        chip8->memory[chip8->I + 1] = (chip8->V[x] / 10) % 10;
                    }

                    if(chip8->I + 2 < 4096) {
                        chip8->memory[chip8->I + 2] = chip8->V[x] % 10;
                    }

                    break;
                }

                case 0x55: {
                    for(int i = 0; i <= x; i++) {
                        if((chip8->I + i) > 4095) {
                            break;
                        }
                        chip8->memory[chip8->I + i] = chip8->V[i];
                    }
                    break;
                }

                case 0x65: {
                    for(int i = 0; i <= x; i++) {
                        if((chip8->I + i) > 4095) {
                            break;
                        }
                        chip8->V[i] = chip8->memory[chip8->I + i];
                    }
                    break;
                }
            }
            break;
        }
    }
}

void chip8_reset(Chip8 *chip8) {
    char romPath[256];

    strncpy(romPath, chip8->romPath, sizeof(romPath));
    romPath[sizeof(romPath) - 1] = '\0';

    chip8_init(chip8);
    chip8_load_rom(chip8, romPath);
}