#include "texture_manager.h"
#include <stdio.h>

SDL_Texture* load_texture(SDL_Renderer* renderer, const char* path, int* width, int* height) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        printf("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
        return NULL;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        printf("Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
    } else {
        if (width) *width = surface->w;
        if (height) *height = surface->h;
    }
    
    SDL_FreeSurface(surface);
    return texture;
}

int texture_manager_init(TextureManager* tm, SDL_Renderer* renderer) {
    printf("DEBUG: Starting texture manager init...\n");
    tm->renderer = renderer;
    
    // Initialize all textures to NULL first
    tm->background.texture = NULL;
    tm->logo.texture = NULL;
    tm->dashie.texture = NULL;
    tm->kion_ded.texture = NULL;
    tm->kion_happi.texture = NULL;
    tm->arrow.texture = NULL;
    tm->ball.texture = NULL;
    tm->paddle.texture = NULL;
    tm->brick_red.texture = NULL;
    tm->brick_yellow.texture = NULL;
    tm->brick_green.texture = NULL;
    tm->brick_blue.texture = NULL;
    tm->brick_purple.texture = NULL;
    tm->font_regular = NULL;
    tm->font_title = NULL;
    
    // Initialize audio to NULL
    tm->bgm_title = NULL;
    tm->bgm_stage1 = NULL;
    tm->bgm_stage2 = NULL;
    tm->bgm_stage3 = NULL;
    tm->bgm_stage4 = NULL;
    tm->bgm_stage5 = NULL;
    tm->bgm_gameover = NULL;
    tm->bgm_complete = NULL;
    tm->sfx_ball_paddle = NULL;
    tm->sfx_ball_wall = NULL;
    tm->sfx_ball_brick = NULL;
    tm->sfx_brick_break = NULL;
    tm->sfx_lose_life = NULL;
    tm->sfx_menu_select = NULL;
    
    printf("DEBUG: Loading background texture...\n");
    tm->background.texture = load_texture(renderer, "docs/img/background-bits.png", 
                                        &tm->background.width, &tm->background.height);
    if (!tm->background.texture) {
        printf("Warning: Failed to load background texture\n");
        // Don't return error, continue with other textures
    } else {
        printf("DEBUG: Background texture loaded successfully\n");
    }
    
    tm->logo.texture = load_texture(renderer, "docs/img/brickout-logo.webp", 
                                   &tm->logo.width, &tm->logo.height);
    if (!tm->logo.texture) {
        printf("Warning: Failed to load logo texture\n");
    }
    
    tm->dashie.texture = load_texture(renderer, "docs/img/dashie.webp", 
                                     &tm->dashie.width, &tm->dashie.height);
    if (!tm->dashie.texture) {
        printf("Warning: Failed to load dashie texture\n");
    }
    
    tm->kion_ded.texture = load_texture(renderer, "docs/img/kion-ded.webp", 
                                       &tm->kion_ded.width, &tm->kion_ded.height);
    if (!tm->kion_ded.texture) {
        printf("Warning: Failed to load kion-ded texture\n");
    }
    
    tm->kion_happi.texture = load_texture(renderer, "docs/img/kion-happi.webp", 
                                         &tm->kion_happi.width, &tm->kion_happi.height);
    if (!tm->kion_happi.texture) {
        printf("Warning: Failed to load kion-happi texture\n");
    }
    
    tm->arrow.texture = load_texture(renderer, "docs/assets/UI/arrow_decorative_green.png", 
                                    &tm->arrow.width, &tm->arrow.height);
    if (!tm->arrow.texture) {
        printf("Warning: Failed to load arrow texture\n");
    }
    
    printf("DEBUG: Loading gameplay textures...\n");
    tm->ball.texture = load_texture(renderer, "docs/assets/UI/ballBlue.png", 
                                   &tm->ball.width, &tm->ball.height);
    if (!tm->ball.texture) {
        printf("Warning: Failed to load ball texture\n");
    }
    
    tm->paddle.texture = load_texture(renderer, "docs/assets/UI/paddleBlu.png", 
                                     &tm->paddle.width, &tm->paddle.height);
    if (!tm->paddle.texture) {
        printf("Warning: Failed to load paddle texture\n");
    }
    
    tm->brick_red.texture = load_texture(renderer, "docs/assets/UI/element_red_rectangle.png", 
                                        &tm->brick_red.width, &tm->brick_red.height);
    tm->brick_yellow.texture = load_texture(renderer, "docs/assets/UI/element_yellow_rectangle.png", 
                                           &tm->brick_yellow.width, &tm->brick_yellow.height);
    tm->brick_green.texture = load_texture(renderer, "docs/assets/UI/element_green_rectangle.png", 
                                          &tm->brick_green.width, &tm->brick_green.height);
    tm->brick_blue.texture = load_texture(renderer, "docs/assets/UI/element_blue_rectangle.png", 
                                         &tm->brick_blue.width, &tm->brick_blue.height);
    tm->brick_purple.texture = load_texture(renderer, "docs/assets/UI/element_purple_rectangle.png", 
                                           &tm->brick_purple.width, &tm->brick_purple.height);
    
    printf("Loading fonts...\n");
    tm->font_regular = TTF_OpenFont("docs/assets/Font/Kenney Future.ttf", 24);
    if (!tm->font_regular) {
        printf("Warning: Failed to load regular font: %s\n", TTF_GetError());
        // Try alternative path
        tm->font_regular = TTF_OpenFont("docs/assets/Font/Kenney Future Narrow.ttf", 24);
        if (!tm->font_regular) {
            printf("Warning: Failed to load narrow font: %s\n", TTF_GetError());
        } else {
            printf("Loaded narrow font successfully\n");
        }
    } else {
        printf("Loaded regular font successfully\n");
    }
    
    tm->font_title = TTF_OpenFont("docs/assets/Font/Kenney Future.ttf", 32);
    if (!tm->font_title) {
        printf("Warning: Failed to load title font: %s\n", TTF_GetError());
    }
    
    // Load BGM tracks
    printf("DEBUG: Loading BGM tracks...\n");
    tm->bgm_title = Mix_LoadMUS("docs/assets/BGM/Title Screen.wav");
    if (!tm->bgm_title) {
        printf("Warning: Failed to load title BGM: %s\n", Mix_GetError());
    }
    
    tm->bgm_stage1 = Mix_LoadMUS("docs/assets/BGM/Stage_1.wav");
    if (!tm->bgm_stage1) {
        printf("Warning: Failed to load stage 1 BGM: %s\n", Mix_GetError());
    }
    
    tm->bgm_stage2 = Mix_LoadMUS("docs/assets/BGM/Stage_2.wav");
    if (!tm->bgm_stage2) {
        printf("Warning: Failed to load stage 2 BGM: %s\n", Mix_GetError());
    }
    
    tm->bgm_stage3 = Mix_LoadMUS("docs/assets/BGM/Stage_3.wav");
    if (!tm->bgm_stage3) {
        printf("Warning: Failed to load stage 3 BGM: %s\n", Mix_GetError());
    }
    
    tm->bgm_stage4 = Mix_LoadMUS("docs/assets/BGM/Stage_4.wav");
    if (!tm->bgm_stage4) {
        printf("Warning: Failed to load stage 4 BGM: %s\n", Mix_GetError());
    }
    
    tm->bgm_stage5 = Mix_LoadMUS("docs/assets/BGM/Stage_5.wav");
    if (!tm->bgm_stage5) {
        printf("Warning: Failed to load stage 5 BGM: %s\n", Mix_GetError());
    }
    
    tm->bgm_gameover = Mix_LoadMUS("docs/assets/BGM/GameOver.wav");
    if (!tm->bgm_gameover) {
        printf("Warning: Failed to load game over BGM: %s\n", Mix_GetError());
    }
    
    tm->bgm_complete = Mix_LoadMUS("docs/assets/BGM/GameComplete.wav");
    if (!tm->bgm_complete) {
        printf("Warning: Failed to load game complete BGM: %s\n", Mix_GetError());
    }
    
    // Load SFX
    printf("DEBUG: Loading SFX...\n");
    tm->sfx_ball_paddle = Mix_LoadWAV("docs/assets/SFX/ball_hit_paddle.mp3");
    if (!tm->sfx_ball_paddle) {
        printf("Warning: Failed to load ball paddle SFX: %s\n", Mix_GetError());
    }
    
    tm->sfx_ball_wall = Mix_LoadWAV("docs/assets/SFX/ball_hit_wall.mp3");
    if (!tm->sfx_ball_wall) {
        printf("Warning: Failed to load ball wall SFX: %s\n", Mix_GetError());
    }
    
    tm->sfx_ball_brick = Mix_LoadWAV("docs/assets/SFX/ball_hit_brick.mp3");
    if (!tm->sfx_ball_brick) {
        printf("Warning: Failed to load ball brick SFX: %s\n", Mix_GetError());
    }
    
    tm->sfx_brick_break = Mix_LoadWAV("docs/assets/SFX/ball_break_brick.mp3");
    if (!tm->sfx_brick_break) {
        printf("Warning: Failed to load brick break SFX: %s\n", Mix_GetError());
    }
    
    tm->sfx_lose_life = Mix_LoadWAV("docs/assets/SFX/lose_life.mp3");
    if (!tm->sfx_lose_life) {
        printf("Warning: Failed to load lose life SFX: %s\n", Mix_GetError());
    }
    
    tm->sfx_menu_select = Mix_LoadWAV("docs/assets/SFX/menu_select.mp3");
    if (!tm->sfx_menu_select) {
        printf("Warning: Failed to load menu select SFX: %s\n", Mix_GetError());
    }
    
    return 0;
}

void texture_manager_cleanup(TextureManager* tm) {
    printf("DEBUG: Destroying textures...\n");
    if (tm->background.texture) {
        SDL_DestroyTexture(tm->background.texture);
        tm->background.texture = NULL;
    }
    if (tm->logo.texture) {
        SDL_DestroyTexture(tm->logo.texture);
        tm->logo.texture = NULL;
    }
    if (tm->dashie.texture) {
        SDL_DestroyTexture(tm->dashie.texture);
        tm->dashie.texture = NULL;
    }
    if (tm->kion_ded.texture) {
        SDL_DestroyTexture(tm->kion_ded.texture);
        tm->kion_ded.texture = NULL;
    }
    if (tm->kion_happi.texture) {
        SDL_DestroyTexture(tm->kion_happi.texture);
        tm->kion_happi.texture = NULL;
    }
    if (tm->arrow.texture) {
        SDL_DestroyTexture(tm->arrow.texture);
        tm->arrow.texture = NULL;
    }
    if (tm->ball.texture) {
        SDL_DestroyTexture(tm->ball.texture);
        tm->ball.texture = NULL;
    }
    if (tm->paddle.texture) {
        SDL_DestroyTexture(tm->paddle.texture);
        tm->paddle.texture = NULL;
    }
    if (tm->brick_red.texture) {
        SDL_DestroyTexture(tm->brick_red.texture);
        tm->brick_red.texture = NULL;
    }
    if (tm->brick_yellow.texture) {
        SDL_DestroyTexture(tm->brick_yellow.texture);
        tm->brick_yellow.texture = NULL;
    }
    if (tm->brick_green.texture) {
        SDL_DestroyTexture(tm->brick_green.texture);
        tm->brick_green.texture = NULL;
    }
    if (tm->brick_blue.texture) {
        SDL_DestroyTexture(tm->brick_blue.texture);
        tm->brick_blue.texture = NULL;
    }
    if (tm->brick_purple.texture) {
        SDL_DestroyTexture(tm->brick_purple.texture);
        tm->brick_purple.texture = NULL;
    }
    
    printf("DEBUG: Closing fonts...\n");
    if (tm->font_regular) {
        TTF_CloseFont(tm->font_regular);
        tm->font_regular = NULL;
    }
    if (tm->font_title) {
        TTF_CloseFont(tm->font_title);
        tm->font_title = NULL;
    }
    
    printf("DEBUG: Freeing audio assets...\n");
    if (tm->bgm_title) {
        Mix_FreeMusic(tm->bgm_title);
        tm->bgm_title = NULL;
    }
    if (tm->bgm_stage1) {
        Mix_FreeMusic(tm->bgm_stage1);
        tm->bgm_stage1 = NULL;
    }
    if (tm->bgm_stage2) {
        Mix_FreeMusic(tm->bgm_stage2);
        tm->bgm_stage2 = NULL;
    }
    if (tm->bgm_stage3) {
        Mix_FreeMusic(tm->bgm_stage3);
        tm->bgm_stage3 = NULL;
    }
    if (tm->bgm_stage4) {
        Mix_FreeMusic(tm->bgm_stage4);
        tm->bgm_stage4 = NULL;
    }
    if (tm->bgm_stage5) {
        Mix_FreeMusic(tm->bgm_stage5);
        tm->bgm_stage5 = NULL;
    }
    if (tm->bgm_gameover) {
        Mix_FreeMusic(tm->bgm_gameover);
        tm->bgm_gameover = NULL;
    }
    if (tm->bgm_complete) {
        Mix_FreeMusic(tm->bgm_complete);
        tm->bgm_complete = NULL;
    }
    
    if (tm->sfx_ball_paddle) {
        Mix_FreeChunk(tm->sfx_ball_paddle);
        tm->sfx_ball_paddle = NULL;
    }
    if (tm->sfx_ball_wall) {
        Mix_FreeChunk(tm->sfx_ball_wall);
        tm->sfx_ball_wall = NULL;
    }
    if (tm->sfx_ball_brick) {
        Mix_FreeChunk(tm->sfx_ball_brick);
        tm->sfx_ball_brick = NULL;
    }
    if (tm->sfx_brick_break) {
        Mix_FreeChunk(tm->sfx_brick_break);
        tm->sfx_brick_break = NULL;
    }
    if (tm->sfx_lose_life) {
        Mix_FreeChunk(tm->sfx_lose_life);
        tm->sfx_lose_life = NULL;
    }
    if (tm->sfx_menu_select) {
        Mix_FreeChunk(tm->sfx_menu_select);
        tm->sfx_menu_select = NULL;
    }
    
    printf("DEBUG: Texture manager cleanup complete.\n");
}

void render_texture(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, int width, int height) {
    if (!texture) return;
    
    SDL_Rect dest_rect = {x, y, width, height};
    SDL_RenderCopy(renderer, texture, NULL, &dest_rect);
}

SDL_Texture* create_text_texture(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color, int* width, int* height) {
    if (!font || !text) return NULL;
    
    SDL_Surface* text_surface = TTF_RenderText_Solid(font, text, color);
    if (!text_surface) {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
        return NULL;
    }
    
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    if (!text_texture) {
        printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
    } else {
        if (width) *width = text_surface->w;
        if (height) *height = text_surface->h;
    }
    
    SDL_FreeSurface(text_surface);
    return text_texture;
}

void play_bgm(Mix_Music* music) {
    if (!music) return;
    
    // Stop current music if playing
    if (Mix_PlayingMusic()) {
        Mix_HaltMusic();
    }
    
    // Play new music on loop
    Mix_PlayMusic(music, -1);
    Mix_VolumeMusic(64); // 50% volume
}

void stop_bgm(void) {
    if (Mix_PlayingMusic()) {
        Mix_HaltMusic();
    }
}

void play_sfx(Mix_Chunk* chunk) {
    if (!chunk) return;
    
    Mix_PlayChannel(-1, chunk, 0);
}