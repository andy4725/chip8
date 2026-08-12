#include <stdio.h>
#include "chip8.h"
#include "platform.h"

#define CPU_FREQUENCY 700
#define TIMER_FREQUENCY 60

int main(int argc, char *argv[]) {
    Chip8 chip8;
    chip8_init(&chip8);

    if(argc < 2) {
        printf("Usage: %s <rom>\n", argv[0]);
        return 1;
    }

    if(!chip8_load_rom(&chip8, argv[1])) {
        return 1;
    }

    Platform platform = {0};

    if(!platform_init(&platform, &chip8)) {
        return 1;
    }

    double cpuAccumulator = 0.0;
    double timerAccumulator = 0.0;

    while(platform.running) {
        platform_process_input(&platform);

        if(platform.resetRequested) {
            chip8_reset(&chip8);
            platform.resetRequested = 0;
        }

        double elapsed = platform_get_elapsed_time(&platform);

        cpuAccumulator += elapsed;
        timerAccumulator += elapsed;

        while(cpuAccumulator >= 1.0 / CPU_FREQUENCY) {
            chip8_cycle(&chip8);
            cpuAccumulator -= 1.0 / CPU_FREQUENCY;
        }

        while(timerAccumulator >= 1.0 / TIMER_FREQUENCY) {
            chip8_update_timers(&chip8);
            timerAccumulator -= 1.0 / TIMER_FREQUENCY;
        }

        platform_render(&platform);
    }

    platform_shutdown(&platform);

    return 0;
}