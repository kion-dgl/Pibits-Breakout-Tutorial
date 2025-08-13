#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

typedef struct {
    SDL_Texture* texture;
    int width;
    int height;
} Texture;

typedef struct {
    SDL_Renderer* renderer;
    Texture background;
    Texture logo;
    Texture dashie;
    Texture kion_ded;
    Texture kion_happi;
    Texture arrow;
    Texture ball;
    Texture paddle;
    Texture brick_red;
    Texture brick_yellow;
    Texture brick_green;
    Texture brick_blue;
    Texture brick_purple;
    TTF_Font* font_regular;
    TTF_Font* font_title;
    
    // BGM tracks
    Mix_Music* bgm_title;
    Mix_Music* bgm_stage1;
    Mix_Music* bgm_stage2;
    Mix_Music* bgm_stage3;
    Mix_Music* bgm_stage4;
    Mix_Music* bgm_stage5;
    Mix_Music* bgm_gameover;
    Mix_Music* bgm_complete;
    
    // SFX
    Mix_Chunk* sfx_ball_paddle;
    Mix_Chunk* sfx_ball_wall;
    Mix_Chunk* sfx_ball_brick;
    Mix_Chunk* sfx_brick_break;
    Mix_Chunk* sfx_lose_life;
    Mix_Chunk* sfx_menu_select;
} TextureManager;

int texture_manager_init(TextureManager* tm, SDL_Renderer* renderer);
void texture_manager_cleanup(TextureManager* tm);
SDL_Texture* load_texture(SDL_Renderer* renderer, const char* path, int* width, int* height);
void render_texture(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, int width, int height);
SDL_Texture* create_text_texture(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color, int* width, int* height);

// Audio functions
void play_bgm(Mix_Music* music);
void stop_bgm(void);
void play_sfx(Mix_Chunk* chunk);

#endif