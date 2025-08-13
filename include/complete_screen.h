#ifndef COMPLETE_SCREEN_H
#define COMPLETE_SCREEN_H

#include <SDL.h>
#include "texture_manager.h"

typedef enum {
    COMPLETE_PLAY_AGAIN,
    COMPLETE_QUIT,
    COMPLETE_MENU_COUNT
} CompleteOption;

typedef struct {
    CompleteOption current_option;
    TextureManager* texture_manager;
    int final_score;
} CompleteScreen;

void complete_screen_init(CompleteScreen* cs, TextureManager* tm, int score);
void complete_screen_handle_input(CompleteScreen* cs, SDL_Event* e, int* next_state);
void complete_screen_update(CompleteScreen* cs, float delta_time);
void complete_screen_render(CompleteScreen* cs, SDL_Renderer* renderer);

#endif