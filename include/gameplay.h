#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <SDL.h>
#include "ball.h"
#include "paddle.h"
#include "brick.h"
#include "texture_manager.h"

typedef struct {
    Ball ball;
    Paddle paddle;
    BrickGrid brick_grid;
    TextureManager* texture_manager;
    int lives;
    int score;
    int stage;
    bool paused;
} Gameplay;

void gameplay_init(Gameplay* gp, TextureManager* tm);
void gameplay_handle_input(Gameplay* gp, SDL_Event* e, int* next_state);
void gameplay_update(Gameplay* gp, float delta_time, int* next_state);
void gameplay_render(Gameplay* gp, SDL_Renderer* renderer);
void gameplay_reset_ball(Gameplay* gp);
void gameplay_reset_game(Gameplay* gp);
bool gameplay_check_collisions(Gameplay* gp);

#endif