#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

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
} TextureManager;

int texture_manager_init(TextureManager* tm, SDL_Renderer* renderer);
void texture_manager_cleanup(TextureManager* tm);
SDL_Texture* load_texture(SDL_Renderer* renderer, const char* path, int* width, int* height);
void render_texture(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, int width, int height);
SDL_Texture* create_text_texture(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color, int* width, int* height);

#endif