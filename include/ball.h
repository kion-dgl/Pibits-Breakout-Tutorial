#ifndef BALL_H
#define BALL_H

#include <SDL.h>
#include "texture_manager.h"

typedef struct {
    float x, y;           // Position
    float vel_x, vel_y;   // Velocity
    int width, height;    // Size
    SDL_Texture* texture; // Ball texture
} Ball;

void ball_init(Ball* ball, float x, float y, SDL_Texture* texture);
void ball_update(Ball* ball, float delta_time, TextureManager* tm);
void ball_render(Ball* ball, SDL_Renderer* renderer);
void ball_bounce_x(Ball* ball, TextureManager* tm);
void ball_bounce_y(Ball* ball, TextureManager* tm);
void ball_reset(Ball* ball, float x, float y);

#endif