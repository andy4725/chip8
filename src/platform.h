#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL3/SDL.h>
#include <chip8.h>

typedef struct {
    float phase;
    float frequency;
    int sampleRate;
} AudioData;

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_AudioStream *audioStream;
    AudioData audio;
    Chip8 *chip8;
    uint64_t lastTime;
    int running;
    int resetRequested;
} Platform;

int platform_init(Platform *platform, Chip8 *chip8);
void platform_process_input(Platform *platform);
void platform_render(Platform *platform);
void platform_shutdown(Platform *platform);

double platform_get_elapsed_time(Platform *platform);

#endif