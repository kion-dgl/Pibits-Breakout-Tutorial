#ifndef TITLE_SCREEN_H
#define TITLE_SCREEN_H

#include <SDL.h>
#include "texture_manager.h"

typedef enum {
    MENU_START_GAME,
    MENU_QUIT_GAME,
    MENU_COUNT
} MenuOption;

typedef struct {
    MenuOption current_option;
    float arrow_rotation;
    TextureManager* texture_manager;
} TitleScreen;

void title_screen_init(TitleScreen* ts, TextureManager* tm);
void title_screen_handle_input(TitleScreen* ts, SDL_Event* e, int* next_state);
void title_screen_update(TitleScreen* ts, float delta_time);
void title_screen_render(TitleScreen* ts, SDL_Renderer* renderer);

#endif