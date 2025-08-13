#ifndef GAMEOVER_SCREEN_H
#define GAMEOVER_SCREEN_H

#include <SDL.h>
#include "texture_manager.h"

typedef enum {
    GAMEOVER_RETRY,
    GAMEOVER_QUIT,
    GAMEOVER_MENU_COUNT
} GameOverOption;

typedef struct {
    GameOverOption current_option;
    TextureManager* texture_manager;
    int final_score;
    int final_stage;
} GameOverScreen;

void gameover_screen_init(GameOverScreen* gos, TextureManager* tm, int score, int stage);
void gameover_screen_handle_input(GameOverScreen* gos, SDL_Event* e, int* next_state);
void gameover_screen_update(GameOverScreen* gos, float delta_time);
void gameover_screen_render(GameOverScreen* gos, SDL_Renderer* renderer);

#endif