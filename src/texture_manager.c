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
    tm->font_regular = NULL;
    tm->font_title = NULL;
    
    printf("DEBUG: Loading background texture...\n");
    tm->background.texture = load_texture(renderer, "docs/img/background.png", 
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
    
    printf("DEBUG: Closing fonts...\n");
    if (tm->font_regular) {
        TTF_CloseFont(tm->font_regular);
        tm->font_regular = NULL;
    }
    if (tm->font_title) {
        TTF_CloseFont(tm->font_title);
        tm->font_title = NULL;
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