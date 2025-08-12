#ifndef PADDLE_H
#define PADDLE_H

#include <SDL.h>

typedef struct {
    float x, y;           // Position
    int width, height;    // Size
    float speed;          // Movement speed
    SDL_Texture* texture; // Paddle texture
} Paddle;

void paddle_init(Paddle* paddle, float x, float y, SDL_Texture* texture);
void paddle_update(Paddle* paddle, const Uint8* keyboard_state, float delta_time);
void paddle_render(Paddle* paddle, SDL_Renderer* renderer);

#endif