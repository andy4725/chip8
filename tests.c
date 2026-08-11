#include <stdio.h>
#include <stdint.h>
#include "chip8.h"

int testsPassed = 0;
int testsFailed = 0;

void test(const char *name, int condition) {
    if(condition) {
        printf("PASS: %s\n", name);
        testsPassed++;
    } else {
        printf("FAIL: %s\n", name);
        testsFailed++;
    }
}

int main() {
    Chip8 chip8;

    // 00E0 - Clear screen
    chip8_init(&chip8);
    chip8.display[0] = 1;

    chip8.memory[0x200] = 0x00;
    chip8.memory[0x201] = 0xE0;

    chip8_cycle(&chip8);

    test("00E0 - Clear screen", chip8.display[0] == 0);

    // 00EE - Return from subroutine
    chip8_init(&chip8);
    chip8.stack[0] = 0x300;
    chip8.sp = 1;

    chip8.memory[0x200] = 0x00;
    chip8.memory[0x201] = 0xEE;

    chip8_cycle(&chip8);

    test("00EE - Return", chip8.pc == 0x300 && chip8.sp == 0);

    // 1NNN - Jump
    chip8_init(&chip8);

    chip8.memory[0x200] = 0x12;
    chip8.memory[0x201] = 0x34;

    chip8_cycle(&chip8);

    test("1NNN - Jump", chip8.pc == 0x234);

    // 2NNN - Call
    chip8_init(&chip8);

    chip8.memory[0x200] = 0x22;
    chip8.memory[0x201] = 0x34;

    chip8_cycle(&chip8);

    test("2NNN - Call", chip8.pc == 0x234 &&
        chip8.sp == 1 &&
        chip8.stack[0] == 0x202);

    // 3XNN - Skip if equal
    chip8_init(&chip8);
    chip8.V[1] = 0xAB;

    chip8.memory[0x200] = 0x31;
    chip8.memory[0x201] = 0xAB;

    chip8_cycle(&chip8);

    test("3XNN - Skip when equal", chip8.pc == 0x204);

    // 3XNN - Don't skip if different
    chip8_init(&chip8);
    chip8.V[1] = 0xAA;

    chip8.memory[0x200] = 0x31;
    chip8.memory[0x201] = 0xAB;

    chip8_cycle(&chip8);

    test("3XNN - Don't skip when different", chip8.pc == 0x202);

    // 4XNN - Skip if not equal
    chip8_init(&chip8);
    chip8.V[1] = 0xAA;

    chip8.memory[0x200] = 0x41;
    chip8.memory[0x201] = 0xAB;

    chip8_cycle(&chip8);

    test("4XNN - Skip when different", chip8.pc == 0x204);

    // 4XNN - Don't skip if equal
    chip8_init(&chip8);
    chip8.V[1] = 0xAB;

    chip8.memory[0x200] = 0x41;
    chip8.memory[0x201] = 0xAB;

    chip8_cycle(&chip8);

    test("4XNN - Don't skip when equal", chip8.pc == 0x202);

    // 5XY0 - Skip if VX == VY
    chip8_init(&chip8);
    chip8.V[1] = 0xAB;
    chip8.V[2] = 0xAB;

    chip8.memory[0x200] = 0x51;
    chip8.memory[0x201] = 0x20;

    chip8_cycle(&chip8);

    test("5XY0 - Skip when equal", chip8.pc == 0x204);

    // 5XY0 - Don't skip if VX != VY
    chip8_init(&chip8);
    chip8.V[1] = 0xAB;
    chip8.V[2] = 0xAA;

    chip8.memory[0x200] = 0x51;
    chip8.memory[0x201] = 0x20;

    chip8_cycle(&chip8);

    test("5XY0 - Don't skip when different", chip8.pc == 0x202);

        // 7XNN - Add immediate
    chip8_init(&chip8);
    chip8.V[1] = 0x10;

    chip8.memory[0x200] = 0x71;
    chip8.memory[0x201] = 0x20;

    chip8_cycle(&chip8);

    test("7XNN - Add immediate", chip8.V[1] == 0x30);

    // 7XNN - Overflow wraps around
    chip8_init(&chip8);
    chip8.V[1] = 0xFF;

    chip8.memory[0x200] = 0x71;
    chip8.memory[0x201] = 0x02;

    chip8_cycle(&chip8);

    test("7XNN - Overflow", chip8.V[1] == 0x01);

    // 8XY0 - VX = VY
    chip8_init(&chip8);
    chip8.V[1] = 0x12;
    chip8.V[2] = 0xAB;

    chip8.memory[0x200] = 0x81;
    chip8.memory[0x201] = 0x20;

    chip8_cycle(&chip8);

    test("8XY0 - VX = VY", chip8.V[1] == 0xAB);

    // 8XY1 - OR
    chip8_init(&chip8);
    chip8.V[1] = 0xAA;
    chip8.V[2] = 0x55;

    chip8.memory[0x200] = 0x81;
    chip8.memory[0x201] = 0x21;

    chip8_cycle(&chip8);

    test("8XY1 - OR", chip8.V[1] == 0xFF);

    // 8XY2 - AND
    chip8_init(&chip8);
    chip8.V[1] = 0xAA;
    chip8.V[2] = 0x55;

    chip8.memory[0x200] = 0x81;
    chip8.memory[0x201] = 0x22;

    chip8_cycle(&chip8);

    test("8XY2 - AND", chip8.V[1] == 0x00);

    // 8XY3 - XOR
    chip8_init(&chip8);
    chip8.V[1] = 0xAA;
    chip8.V[2] = 0x55;

    chip8.memory[0x200] = 0x81;
    chip8.memory[0x201] = 0x23;

    chip8_cycle(&chip8);

    test("8XY3 - XOR", chip8.V[1] == 0xFF);

    // 8XY4 - Add with carry, no overflow
    chip8_init(&chip8);
    chip8.V[1] = 0x20;
    chip8.V[2] = 0x10;

    chip8.memory[0x200] = 0x81;
    chip8.memory[0x201] = 0x24;

    chip8_cycle(&chip8);

    test("8XY4 - Add without carry",
        chip8.V[1] == 0x30 && chip8.V[0xF] == 0);

    // 8XY4 - Add with carry
    chip8_init(&chip8);
    chip8.V[1] = 0xFF;
    chip8.V[2] = 0x02;

    chip8.memory[0x200] = 0x81;
    chip8.memory[0x201] = 0x24;

    chip8_cycle(&chip8);

    test("8XY4 - Add with carry",
        chip8.V[1] == 0x01 && chip8.V[0xF] == 1);

    // 8XY5 - Subtract without borrow
    chip8_init(&chip8);
    chip8.V[1] = 0x10;
    chip8.V[2] = 0x03;

    chip8.memory[0x200] = 0x81;
    chip8.memory[0x201] = 0x25;

    chip8_cycle(&chip8);

    test("8XY5 - Subtract without borrow",
        chip8.V[1] == 0x0D && chip8.V[0xF] == 1);

    // 8XY5 - Subtract with borrow
    chip8_init(&chip8);
    chip8.V[1] = 0x03;
    chip8.V[2] = 0x10;

    chip8.memory[0x200] = 0x81;
    chip8.memory[0x201] = 0x25;

    chip8_cycle(&chip8);

    test("8XY5 - Subtract with borrow",
        chip8.V[1] == 0xF3 && chip8.V[0xF] == 0);

    // 8XY6 - Shift right
    chip8_init(&chip8);
    chip8.V[1] = 0xAB;

    chip8.memory[0x200] = 0x81;
    chip8.memory[0x201] = 0x26;

    chip8_cycle(&chip8);

    test("8XY6 - Shift right",
        chip8.V[1] == 0x55 && chip8.V[0xF] == 1);

    // 8XY7 - Reverse subtract without borrow
    chip8_init(&chip8);
    chip8.V[1] = 0x03;
    chip8.V[2] = 0x10;

    chip8.memory[0x200] = 0x81;
    chip8.memory[0x201] = 0x27;

    chip8_cycle(&chip8);

    test("8XY7 - Reverse subtract without borrow",
        chip8.V[1] == 0x0D && chip8.V[0xF] == 1);

    // 8XY7 - Reverse subtract with borrow
    chip8_init(&chip8);
    chip8.V[1] = 0x10;
    chip8.V[2] = 0x03;

    chip8.memory[0x200] = 0x81;
    chip8.memory[0x201] = 0x27;

    chip8_cycle(&chip8);

    test("8XY7 - Reverse subtract with borrow",
        chip8.V[1] == 0xF3 && chip8.V[0xF] == 0);

    // 8XYE - Shift left
    chip8_init(&chip8);
    chip8.V[1] = 0xAB;

    chip8.memory[0x200] = 0x81;
    chip8.memory[0x201] = 0x2E;

    chip8_cycle(&chip8);

    test("8XYE - Shift left",
        chip8.V[1] == 0x56 && chip8.V[0xF] == 1);

        // 9XY0 - Skip if VX != VY
    
    chip8_init(&chip8);
    chip8.V[1] = 0x10;
    chip8.V[2] = 0x20;

    chip8.memory[0x200] = 0x91;
    chip8.memory[0x201] = 0x20;

    chip8_cycle(&chip8);

    test("9XY0 - Skip when different", chip8.pc == 0x204);

    // 9XY0 - Don't skip if VX == VY
    chip8_init(&chip8);
    chip8.V[1] = 0x20;
    chip8.V[2] = 0x20;

    chip8.memory[0x200] = 0x91;
    chip8.memory[0x201] = 0x20;

    chip8_cycle(&chip8);

    test("9XY0 - Don't skip when equal", chip8.pc == 0x202);

    // ANNN - Set I
    chip8_init(&chip8);

    chip8.memory[0x200] = 0xA3;
    chip8.memory[0x201] = 0xAB;

    chip8_cycle(&chip8);

    test("ANNN - Set I", chip8.I == 0x3AB);

    // BNNN - Jump to NNN + V0
    chip8_init(&chip8);
    chip8.V[0] = 0x10;

    chip8.memory[0x200] = 0xB3;
    chip8.memory[0x201] = 0x00;

    chip8_cycle(&chip8);

    test("BNNN - Jump with V0", chip8.pc == 0x310);

    // CXNN - Random AND
    chip8_init(&chip8);

    chip8.memory[0x200] = 0xC1;
    chip8.memory[0x201] = 0xF0;

    chip8_cycle(&chip8);

    test("CXNN - Random AND",
        (chip8.V[1] & 0x0F) == 0);

        // DXYN - Draw sprite
    chip8_init(&chip8);
    chip8.I = 0x300;
    chip8.V[0] = 0;
    chip8.V[1] = 0;

    chip8.memory[0x300] = 0xF0;

    chip8.memory[0x200] = 0xD0;
    chip8.memory[0x201] = 0x11;

    chip8_cycle(&chip8);

    test("DXYN - Draw sprite",
        chip8.display[0] == 1 &&
        chip8.display[1] == 1 &&
        chip8.display[2] == 1 &&
        chip8.display[3] == 1 &&
        chip8.display[4] == 0);

    // DXYN - XOR
    chip8_init(&chip8);
    chip8.I = 0x300;
    chip8.V[0] = 0;
    chip8.V[1] = 0;

    chip8.memory[0x300] = 0xF0;

    chip8.display[0] = 1;
    chip8.display[1] = 1;
    chip8.display[2] = 1;
    chip8.display[3] = 1;

    chip8.memory[0x200] = 0xD0;
    chip8.memory[0x201] = 0x11;

    chip8_cycle(&chip8);

    test("DXYN - XOR",
        chip8.display[0] == 0 &&
        chip8.display[1] == 0 &&
        chip8.display[2] == 0 &&
        chip8.display[3] == 0);

    // DXYN - Collision
    chip8_init(&chip8);
    chip8.I = 0x300;
    chip8.V[0] = 0;
    chip8.V[1] = 0;

    chip8.memory[0x300] = 0xF0;

    chip8.display[0] = 1;

    chip8.memory[0x200] = 0xD0;
    chip8.memory[0x201] = 0x11;

    chip8_cycle(&chip8);

    test("DXYN - Collision",
        chip8.V[0xF] == 1);

    // DXYN - Horizontal wrapping
    chip8_init(&chip8);
    chip8.I = 0x300;
    chip8.V[0] = 63;
    chip8.V[1] = 0;

    chip8.memory[0x300] = 0xFF;

    chip8.memory[0x200] = 0xD0;
    chip8.memory[0x201] = 0x11;

    chip8_cycle(&chip8);

    test("DXYN - Horizontal wrapping",
        chip8.display[63] == 1 &&
        chip8.display[0] == 1 &&
        chip8.display[1] == 1 &&
        chip8.display[2] == 1 &&
        chip8.display[3] == 1 &&
        chip8.display[4] == 1 &&
        chip8.display[5] == 1 &&
        chip8.display[6] == 1);

        // EX9E - Skip if key is pressed
    chip8_init(&chip8);
    chip8.V[1] = 0xA;
    chip8.keypad[0xA] = 1;

    chip8.memory[0x200] = 0xE1;
    chip8.memory[0x201] = 0x9E;

    chip8_cycle(&chip8);

    test("EX9E - Skip when key is pressed", chip8.pc == 0x204);

    // EX9E - Don't skip if key is not pressed
    chip8_init(&chip8);
    chip8.V[1] = 0xA;
    chip8.keypad[0xA] = 0;

    chip8.memory[0x200] = 0xE1;
    chip8.memory[0x201] = 0x9E;

    chip8_cycle(&chip8);

    test("EX9E - Don't skip when key is not pressed", chip8.pc == 0x202);

    // EXA1 - Skip if key is not pressed
    chip8_init(&chip8);
    chip8.V[1] = 0xA;
    chip8.keypad[0xA] = 0;

    chip8.memory[0x200] = 0xE1;
    chip8.memory[0x201] = 0xA1;

    chip8_cycle(&chip8);

    test("EXA1 - Skip when key is not pressed", chip8.pc == 0x204);

    // EXA1 - Don't skip if key is pressed
    chip8_init(&chip8);
    chip8.V[1] = 0xA;
    chip8.keypad[0xA] = 1;

    chip8.memory[0x200] = 0xE1;
    chip8.memory[0x201] = 0xA1;

    chip8_cycle(&chip8);

    test("EXA1 - Don't skip when key is pressed", chip8.pc == 0x202);

        // FX07 - VX = delay timer
    chip8_init(&chip8);
    chip8.V[1] = 0;
    chip8.delayTimer = 42;

    chip8.memory[0x200] = 0xF1;
    chip8.memory[0x201] = 0x07;

    chip8_cycle(&chip8);

    test("FX07 - Read delay timer", chip8.V[1] == 42);

    // FX15 - Delay timer = VX
    chip8_init(&chip8);
    chip8.V[1] = 42;

    chip8.memory[0x200] = 0xF1;
    chip8.memory[0x201] = 0x15;

    chip8_cycle(&chip8);

    test("FX15 - Set delay timer", chip8.delayTimer == 42);

    // FX18 - Sound timer = VX
    chip8_init(&chip8);
    chip8.V[1] = 42;

    chip8.memory[0x200] = 0xF1;
    chip8.memory[0x201] = 0x18;

    chip8_cycle(&chip8);

    test("FX18 - Set sound timer", chip8.soundTimer == 42);

    // FX1E - I += VX
    chip8_init(&chip8);
    chip8.I = 0x300;
    chip8.V[1] = 0x25;

    chip8.memory[0x200] = 0xF1;
    chip8.memory[0x201] = 0x1E;

    chip8_cycle(&chip8);

    test("FX1E - Add VX to I", chip8.I == 0x325);

    // FX29 - I = sprite address
    chip8_init(&chip8);
    chip8.V[1] = 0xA;

    chip8.memory[0x200] = 0xF1;
    chip8.memory[0x201] = 0x29;

    chip8_cycle(&chip8);

    test("FX29 - Sprite address", chip8.I == 0x82);

    // FX33 - Store BCD
    chip8_init(&chip8);
    chip8.I = 0x300;
    chip8.V[1] = 123;

    chip8.memory[0x200] = 0xF1;
    chip8.memory[0x201] = 0x33;

    chip8_cycle(&chip8);

    test("FX33 - BCD",
        chip8.memory[0x300] == 1 &&
        chip8.memory[0x301] == 2 &&
        chip8.memory[0x302] == 3);

    // FX55 - Store V0 through VX
    chip8_init(&chip8);
    chip8.I = 0x300;
    chip8.V[0] = 0x10;
    chip8.V[1] = 0x20;
    chip8.V[2] = 0x30;

    chip8.memory[0x200] = 0xF2;
    chip8.memory[0x201] = 0x55;

    chip8_cycle(&chip8);

    test("FX55 - Store registers",
        chip8.memory[0x300] == 0x10 &&
        chip8.memory[0x301] == 0x20 &&
        chip8.memory[0x302] == 0x30);

    // FX65 - Load V0 through VX
    chip8_init(&chip8);
    chip8.I = 0x300;
    chip8.memory[0x300] = 0x10;
    chip8.memory[0x301] = 0x20;
    chip8.memory[0x302] = 0x30;

    chip8.memory[0x200] = 0xF2;
    chip8.memory[0x201] = 0x65;

    chip8_cycle(&chip8);

    test("FX65 - Load registers",
        chip8.V[0] == 0x10 &&
        chip8.V[1] == 0x20 &&
        chip8.V[2] == 0x30);

        // FX0A - Wait when no key is pressed
    chip8_init(&chip8);
    chip8.V[1] = 0xFF;

    chip8.memory[0x200] = 0xF1;
    chip8.memory[0x201] = 0x0A;

    chip8_cycle(&chip8);

    test("FX0A - Wait for key",
        chip8.pc == 0x200 &&
        chip8.V[1] == 0xFF);

    // FX0A - Store pressed key
    chip8_init(&chip8);
    chip8.V[1] = 0xFF;
    chip8.keypad[0xA] = 1;

    chip8.memory[0x200] = 0xF1;
    chip8.memory[0x201] = 0x0A;

    chip8_cycle(&chip8);

    test("FX0A - Store pressed key",
        chip8.pc == 0x202 &&
        chip8.V[1] == 0xA);

        // 2NNN - Stack overflow
    chip8_init(&chip8);
    chip8.sp = 16;

    chip8.memory[0x200] = 0x22;
    chip8.memory[0x201] = 0x34;

    chip8_cycle(&chip8);

    test("2NNN - Stack overflow",
        chip8.sp == 16 &&
        chip8.pc == 0x202);

    // 00EE - Stack underflow
    chip8_init(&chip8);
    chip8.sp = 0;

    chip8.memory[0x200] = 0x00;
    chip8.memory[0x201] = 0xEE;

    chip8_cycle(&chip8);

    test("00EE - Stack underflow",
        chip8.sp == 0 &&
        chip8.pc == 0x202);

        // ROM loading - Missing file
    chip8_init(&chip8);

    test("ROM loading - Missing file",
        chip8_load_rom(&chip8, "does_not_exist.ch8") == 0);

    // ROM loading - Maximum valid ROM size
    FILE *testFile = fopen("test_rom.ch8", "wb");
    uint8_t testByte = 0xAB;

    for(int i = 0; i < 3584; i++) {
        fwrite(&testByte, 1, 1, testFile);
    }

    fclose(testFile);

    chip8_init(&chip8);

    test("ROM loading - Maximum size",
        chip8_load_rom(&chip8, "test_rom.ch8") == 1 &&
        chip8.memory[0x200] == 0xAB &&
        chip8.memory[0xFFF] == 0xAB);

    remove("test_rom.ch8");

    // ROM loading - Oversized ROM
    testFile = fopen("test_rom.ch8", "wb");
    testByte = 0xCD;

    for(int i = 0; i < 3585; i++) {
        fwrite(&testByte, 1, 1, testFile);
    }

    fclose(testFile);

    chip8_init(&chip8);

    test("ROM loading - Oversized ROM",
        chip8_load_rom(&chip8, "test_rom.ch8") == 0);

    remove("test_rom.ch8");

        // DXYN - Vertical wrapping
    chip8_init(&chip8);
    chip8.I = 0x300;
    chip8.V[0] = 0;
    chip8.V[1] = 31;

    chip8.memory[0x300] = 0x80;
    chip8.memory[0x301] = 0x40;

    chip8.memory[0x200] = 0xD0;
    chip8.memory[0x201] = 0x12;

    chip8_cycle(&chip8);

    test("DXYN - Vertical wrapping",
        chip8.display[31 * 64] == 1 &&
        chip8.display[0 * 64 + 1] == 1);

        // FX1E - I overflow
    chip8_init(&chip8);
    chip8.I = 0xFFF;
    chip8.V[1] = 0x01;

    chip8.memory[0x200] = 0xF1;
    chip8.memory[0x201] = 0x1E;

    chip8_cycle(&chip8);

    test("FX1E - I overflow", chip8.I == 0x1000);

        // DXYN - Sprite memory boundary
    chip8_init(&chip8);
    chip8.I = 0xFFF;
    chip8.V[0] = 0;
    chip8.V[1] = 0;

    chip8.memory[0xFFF] = 0x80;

    chip8.memory[0x200] = 0xD0;
    chip8.memory[0x201] = 0x12;

    chip8_cycle(&chip8);

    test("DXYN - Sprite memory boundary",
        chip8.display[0] == 1);

        // FX33 - BCD memory boundary
    chip8_init(&chip8);
    chip8.I = 0xFFE;
    chip8.V[1] = 123;

    chip8.memory[0x200] = 0xF1;
    chip8.memory[0x201] = 0x33;

    chip8_cycle(&chip8);

    test("FX33 - BCD memory boundary",
        chip8.memory[0xFFE] == 1 &&
        chip8.memory[0xFFF] == 2);

        // FX55 - Memory boundary
    chip8_init(&chip8);
    chip8.I = 0xFFF;
    chip8.V[0] = 0x12;
    chip8.V[1] = 0x34;

    chip8.memory[0x200] = 0xF1;
    chip8.memory[0x201] = 0x55;

    chip8_cycle(&chip8);

    test("FX55 - Memory boundary",
        chip8.memory[0xFFF] == 0x12);

        // FX65 - Memory boundary
    chip8_init(&chip8);
    chip8.I = 0xFFF;
    chip8.V[0] = 0;
    chip8.V[1] = 0;

    chip8.memory[0xFFF] = 0xAB;

    chip8.memory[0x200] = 0xF1;
    chip8.memory[0x201] = 0x65;

    chip8_cycle(&chip8);

    test("FX65 - Memory boundary",
        chip8.V[0] == 0xAB &&
        chip8.V[1] == 0);

        // Timers - Both decrement
    chip8_init(&chip8);
    chip8.delayTimer = 10;
    chip8.soundTimer = 5;

    chip8_update_timers(&chip8);

    test("Timers - Decrement",
        chip8.delayTimer == 9 &&
        chip8.soundTimer == 4);

    // Timers - Don't underflow
    chip8_init(&chip8);
    chip8.delayTimer = 0;
    chip8.soundTimer = 0;

    chip8_update_timers(&chip8);

    test("Timers - No underflow",
        chip8.delayTimer == 0 &&
        chip8.soundTimer == 0);

    // Timers - Independent
    chip8_init(&chip8);
    chip8.delayTimer = 1;
    chip8.soundTimer = 10;

    chip8_update_timers(&chip8);

    test("Timers - Independent",
        chip8.delayTimer == 0 &&
        chip8.soundTimer == 9);

        // Invalid opcode - 0000
    chip8_init(&chip8);

    chip8.memory[0x200] = 0x00;
    chip8.memory[0x201] = 0x00;

    chip8_cycle(&chip8);

    test("Invalid opcode - 0000",
        chip8.pc == 0x202);

    // Invalid opcode - 8XY8
    chip8_init(&chip8);
    chip8.V[1] = 0xAA;
    chip8.V[2] = 0x55;

    chip8.memory[0x200] = 0x81;
    chip8.memory[0x201] = 0x28;

    chip8_cycle(&chip8);

    test("Invalid opcode - 8XY8",
        chip8.V[1] == 0xAA &&
        chip8.V[2] == 0x55 &&
        chip8.pc == 0x202);

        // CPU - PC advances by 2
    chip8_init(&chip8);
    chip8.memory[0x200] = 0x61;
    chip8.memory[0x201] = 0x42;

    chip8_cycle(&chip8);

    test("CPU - PC advances by 2",
        chip8.pc == 0x202 &&
        chip8.V[1] == 0x42);

    printf("\n%d passed, %d failed\n", testsPassed, testsFailed);

    return testsFailed != 0;
}