#include <SDL3/SDL.h>
#include <stdio.h>
#include "platform.h"

#define SCALE 10

static void audio_callback(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount) {
    (void)total_amount;

    Platform *platform = userdata;
    AudioData *audio = &platform->audio;
    Chip8 *chip8 = platform->chip8;

    int samples = additional_amount / sizeof(float);
    float buffer[samples];

    for(int i = 0; i < samples; i++) {
        if(chip8->soundTimer > 0) {
            buffer[i] = audio->phase < 0.5f ? 0.15f : -0.15f;
            audio->phase += audio->frequency / audio->sampleRate;

            if(audio->phase >= 1.0f) {
                audio->phase -= 1.0f;
            }
        } else {
            buffer[i] = 0.0f;
        }
    }

    SDL_PutAudioStreamData(stream, buffer, additional_amount);
}

int platform_init(Platform *platform, Chip8 *chip8) {
    if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return 0;
    }

    platform->lastTime = SDL_GetPerformanceCounter();

    platform->running = 1;
    platform->chip8 = chip8;

    platform->audio = (AudioData){
        .phase = 0.0f,
        .frequency = 440.0f,
        .sampleRate = 48000,
    };

    SDL_AudioSpec audioSpec = {
        .format = SDL_AUDIO_F32,
        .channels = 1,
        .freq = platform->audio.sampleRate
    };

    platform->audioStream = SDL_OpenAudioDeviceStream(
        SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
        &audioSpec,
        audio_callback,
        platform
    );

    if(platform->audioStream == NULL) {
        printf("SDL_OpenAudioDeviceStream failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }

    SDL_ResumeAudioStreamDevice(platform->audioStream);

    platform->window = SDL_CreateWindow(
        "CHIP-8",
        64 * SCALE,
        32 * SCALE,
        0
    );

    if(platform->window == NULL) {
        printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
        platform_shutdown(platform);
        return 0;
    }

    platform->renderer = SDL_CreateRenderer(platform->window, NULL);

    if(platform->renderer == NULL) {
        printf("SDL_CreateRenderer failed: %s\n", SDL_GetError());
        platform_shutdown(platform);
        return 0;
    }

    return 1;
}

void platform_process_input(Platform *platform) {
    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_EVENT_QUIT) {
            platform->running = 0;
        }

        if(event.type == SDL_EVENT_KEY_DOWN) {
            if(event.key.key == SDLK_M) {
                platform->resetRequested = 1;
            }

            switch(event.key.key) {
                case SDLK_1: platform->chip8->keypad[0x1] = 1; break;
                case SDLK_2: platform->chip8->keypad[0x2] = 1; break;
                case SDLK_3: platform->chip8->keypad[0x3] = 1; break;
                case SDLK_4: platform->chip8->keypad[0xC] = 1; break;
                case SDLK_Q: platform->chip8->keypad[0x4] = 1; break;
                case SDLK_W: platform->chip8->keypad[0x5] = 1; break;
                case SDLK_E: platform->chip8->keypad[0x6] = 1; break;
                case SDLK_R: platform->chip8->keypad[0xD] = 1; break;
                case SDLK_A: platform->chip8->keypad[0x7] = 1; break;
                case SDLK_S: platform->chip8->keypad[0x8] = 1; break;
                case SDLK_D: platform->chip8->keypad[0x9] = 1; break;
                case SDLK_F: platform->chip8->keypad[0xE] = 1; break;
                case SDLK_Z: platform->chip8->keypad[0xA] = 1; break;
                case SDLK_X: platform->chip8->keypad[0x0] = 1; break;
                case SDLK_C: platform->chip8->keypad[0xB] = 1; break;
                case SDLK_V: platform->chip8->keypad[0xF] = 1; break;
            }
        }

        if(event.type == SDL_EVENT_KEY_UP) {
            switch(event.key.key) {
                case SDLK_1: platform->chip8->keypad[0x1] = 0; break;
                case SDLK_2: platform->chip8->keypad[0x2] = 0; break;
                case SDLK_3: platform->chip8->keypad[0x3] = 0; break;
                case SDLK_4: platform->chip8->keypad[0xC] = 0; break;
                case SDLK_Q: platform->chip8->keypad[0x4] = 0; break;
                case SDLK_W: platform->chip8->keypad[0x5] = 0; break;
                case SDLK_E: platform->chip8->keypad[0x6] = 0; break;
                case SDLK_R: platform->chip8->keypad[0xD] = 0; break;
                case SDLK_A: platform->chip8->keypad[0x7] = 0; break;
                case SDLK_S: platform->chip8->keypad[0x8] = 0; break;
                case SDLK_D: platform->chip8->keypad[0x9] = 0; break;
                case SDLK_F: platform->chip8->keypad[0xE] = 0; break;
                case SDLK_Z: platform->chip8->keypad[0xA] = 0; break;
                case SDLK_X: platform->chip8->keypad[0x0] = 0; break;
                case SDLK_C: platform->chip8->keypad[0xB] = 0; break;
                case SDLK_V: platform->chip8->keypad[0xF] = 0; break;
            }
        }
    }
}

double platform_get_elapsed_time(Platform *platform) {
    uint64_t currentTime = SDL_GetPerformanceCounter();

    double elapsed = (double)(currentTime - platform->lastTime)
        / SDL_GetPerformanceFrequency();

    platform->lastTime = currentTime;

    return elapsed;
}

void platform_render(Platform *platform) {
    SDL_SetRenderDrawColor(platform->renderer, 0, 0, 0, 255);
    SDL_RenderClear(platform->renderer);

    SDL_SetRenderDrawColor(platform->renderer, 255, 255, 255, 255);

    for(int y = 0; y < 32; y++) {
        for(int x = 0; x < 64; x++) {
            if(platform->chip8->display[y * 64 + x]) {
                SDL_FRect pixel = {
                    x * SCALE,
                    y * SCALE,
                    SCALE,
                    SCALE
                };

                SDL_RenderFillRect(platform->renderer, &pixel);
            }
        }
    }

    SDL_RenderPresent(platform->renderer);
}

void platform_shutdown(Platform *platform) {
    if(platform->renderer != NULL) {
        SDL_DestroyRenderer(platform->renderer);
    }

    if(platform->window != NULL) {
        SDL_DestroyWindow(platform->window);
    }

    if(platform->audioStream != NULL) {
        SDL_DestroyAudioStream(platform->audioStream);
    }

    SDL_Quit();
}