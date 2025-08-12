#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>

int main() {
    printf("Starting debug test...\n");
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }
    printf("SDL initialized\n");
    
    SDL_Window* window = SDL_CreateWindow("Debug", 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
        640, 480, SDL_WINDOW_SHOWN);
    
    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }
    printf("Window created\n");
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        return -1;
    }
    printf("Renderer created\n");
    
    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        return -1;
    }
    printf("TTF initialized\n");
    
    TTF_Font* font = TTF_OpenFont("docs/assets/Font/Kenney Future.ttf", 24);
    if (!font) {
        printf("Failed to load font: %s\n", TTF_GetError());
    } else {
        printf("Font loaded successfully\n");
    }
    
    printf("Rendering for 2 seconds...\n");
    for (int i = 0; i < 120; i++) {
        SDL_SetRenderDrawColor(renderer, 15, 174, 188, 255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    
    printf("Cleaning up...\n");
    if (font) TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    printf("Debug test complete\n");
    return 0;
}