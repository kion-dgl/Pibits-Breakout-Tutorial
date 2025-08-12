#include "game.h"
#include <stdio.h>

int game_init(Game* game) {
    printf("DEBUG: Starting SDL initialization...\n");
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }
    printf("DEBUG: SDL initialized successfully\n");
    
    printf("DEBUG: Creating window...\n");
    game->window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    
    if (game->window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }
    printf("DEBUG: Window created successfully\n");
    
    printf("DEBUG: Creating renderer...\n");
    game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED);
    if (game->renderer == NULL) {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        return -1;
    }
    printf("DEBUG: Renderer created successfully\n");
    
    printf("DEBUG: Initializing SDL_image...\n");
    if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) & (IMG_INIT_PNG | IMG_INIT_JPG))) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        return -1;
    }
    printf("DEBUG: SDL_image initialized successfully\n");
    
    printf("DEBUG: Initializing SDL_mixer...\n");
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return -1;
    }
    printf("DEBUG: SDL_mixer initialized successfully\n");
    
    printf("DEBUG: Initializing SDL_ttf...\n");
    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        return -1;
    }
    printf("DEBUG: SDL_ttf initialized successfully\n");
    
    printf("DEBUG: Initializing texture manager...\n");
    if (texture_manager_init(&game->texture_manager, game->renderer) != 0) {
        printf("Failed to initialize texture manager\n");
        return -1;
    }
    printf("DEBUG: Texture manager initialized successfully\n");
    
    printf("DEBUG: Initializing title screen...\n");
    title_screen_init(&game->title_screen, &game->texture_manager);
    printf("DEBUG: Title screen initialized successfully\n");
    
    game->current_state = GAME_STATE_TITLE;
    game->running = true;
    game->last_time = SDL_GetTicks();
    game->delta_time = 0.0f;
    
    return 0;
}

void game_run(Game* game) {
    while (game->running) {
        int current_time = SDL_GetTicks();
        game->delta_time = (current_time - game->last_time) / 1000.0f;
        game->last_time = current_time;
        
        game_handle_events(game);
        game_update(game);
        game_render(game);
        
        SDL_Delay(16);
    }
}

void game_cleanup(Game* game) {
    texture_manager_cleanup(&game->texture_manager);
    
    TTF_Quit();
    Mix_Quit();
    IMG_Quit();
    
    if (game->renderer) {
        SDL_DestroyRenderer(game->renderer);
    }
    
    if (game->window) {
        SDL_DestroyWindow(game->window);
    }
    
    SDL_Quit();
}

void game_handle_events(Game* game) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            game->running = false;
        }
        
        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    game->running = false;
                    break;
            }
        }
        
        switch (game->current_state) {
            case GAME_STATE_TITLE: {
                int next_state = game->current_state;
                title_screen_handle_input(&game->title_screen, &e, &next_state);
                game->current_state = next_state;
                break;
            }
            case GAME_STATE_GAMEPLAY:
                break;
            case GAME_STATE_GAMEOVER:
                break;
            case GAME_STATE_COMPLETE:
                break;
            case GAME_STATE_QUIT:
                game->running = false;
                break;
        }
    }
}

void game_update(Game* game) {
    switch (game->current_state) {
        case GAME_STATE_TITLE:
            title_screen_update(&game->title_screen, game->delta_time);
            break;
        case GAME_STATE_GAMEPLAY:
            break;
        case GAME_STATE_GAMEOVER:
            break;
        case GAME_STATE_COMPLETE:
            break;
        case GAME_STATE_QUIT:
            game->running = false;
            break;
    }
}

void game_render(Game* game) {
    SDL_SetRenderDrawColor(game->renderer, 15, 174, 188, 255);
    SDL_RenderClear(game->renderer);
    
    switch (game->current_state) {
        case GAME_STATE_TITLE:
            title_screen_render(&game->title_screen, game->renderer);
            break;
        case GAME_STATE_GAMEPLAY:
            break;
        case GAME_STATE_GAMEOVER:
            break;
        case GAME_STATE_COMPLETE:
            break;
        case GAME_STATE_QUIT:
            break;
    }
    
    SDL_RenderPresent(game->renderer);
}