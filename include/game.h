#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include "texture_manager.h"
#include "title_screen.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define WINDOW_TITLE "Brickout"

typedef enum {
    GAME_STATE_TITLE,
    GAME_STATE_GAMEPLAY,
    GAME_STATE_GAMEOVER,
    GAME_STATE_COMPLETE,
    GAME_STATE_QUIT
} GameState;

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    GameState current_state;
    bool running;
    int last_time;
    float delta_time;
    TextureManager texture_manager;
    TitleScreen title_screen;
} Game;

int game_init(Game* game);
void game_run(Game* game);
void game_cleanup(Game* game);
void game_handle_events(Game* game);
void game_update(Game* game);
void game_render(Game* game);

#endif