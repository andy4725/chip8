#include <SDL3/SDL.h>
#include <stdio.h>
#include "chip8.h"

#define SCALE 10
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

    if(!SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "CHIP-8",
        64 * SCALE,
        32 * SCALE,
        0
    );

    if(window == NULL) {
        printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);

    if(renderer == NULL) {
        printf("SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    uint64_t lastTime = SDL_GetPerformanceCounter();
    double cpuAccumulator = 0.0;
    double timerAccumulator = 0.0;

    int running = 1;
    SDL_Event event;

    while(running) {
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_EVENT_QUIT) {
                running = 0;
            }

            if(event.type == SDL_EVENT_KEY_DOWN) {
                switch(event.key.key) {
                    case SDLK_1: chip8.keypad[0x1] = 1; break;
                    case SDLK_2: chip8.keypad[0x2] = 1; break;
                    case SDLK_3: chip8.keypad[0x3] = 1; break;
                    case SDLK_4: chip8.keypad[0xC] = 1; break;
                    case SDLK_Q: chip8.keypad[0x4] = 1; break;
                    case SDLK_W: chip8.keypad[0x5] = 1; break;
                    case SDLK_E: chip8.keypad[0x6] = 1; break;
                    case SDLK_R: chip8.keypad[0xD] = 1; break;
                    case SDLK_A: chip8.keypad[0x7] = 1; break;
                    case SDLK_S: chip8.keypad[0x8] = 1; break;
                    case SDLK_D: chip8.keypad[0x9] = 1; break;
                    case SDLK_F: chip8.keypad[0xE] = 1; break;
                    case SDLK_Z: chip8.keypad[0xA] = 1; break;
                    case SDLK_X: chip8.keypad[0x0] = 1; break;
                    case SDLK_C: chip8.keypad[0xB] = 1; break;
                    case SDLK_V: chip8.keypad[0xF] = 1; break;
                }
            }

            if(event.type == SDL_EVENT_KEY_UP) {
                switch(event.key.key) {
                    case SDLK_1: chip8.keypad[0x1] = 0; break;
                    case SDLK_2: chip8.keypad[0x2] = 0; break;
                    case SDLK_3: chip8.keypad[0x3] = 0; break;
                    case SDLK_4: chip8.keypad[0xC] = 0; break;
                    case SDLK_Q: chip8.keypad[0x4] = 0; break;
                    case SDLK_W: chip8.keypad[0x5] = 0; break;
                    case SDLK_E: chip8.keypad[0x6] = 0; break;
                    case SDLK_R: chip8.keypad[0xD] = 0; break;
                    case SDLK_A: chip8.keypad[0x7] = 0; break;
                    case SDLK_S: chip8.keypad[0x8] = 0; break;
                    case SDLK_D: chip8.keypad[0x9] = 0; break;
                    case SDLK_F: chip8.keypad[0xE] = 0; break;
                    case SDLK_Z: chip8.keypad[0xA] = 0; break;
                    case SDLK_X: chip8.keypad[0x0] = 0; break;
                    case SDLK_C: chip8.keypad[0xB] = 0; break;
                    case SDLK_V: chip8.keypad[0xF] = 0; break;
                }
            }
        }

        uint64_t currentTime = SDL_GetPerformanceCounter();
        double elapsed = (double)(currentTime - lastTime) / SDL_GetPerformanceFrequency();
        lastTime = currentTime;

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

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        for(int y = 0; y < 32; y++) {
            for(int x = 0; x < 64; x++) {
                if(chip8.display[y * 64 + x]) {
                    SDL_FRect pixel = {
                        x * SCALE,
                        y * SCALE,
                        SCALE,
                        SCALE
                    };

                    SDL_RenderFillRect(renderer, &pixel);
                }
            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}