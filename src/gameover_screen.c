#include "gameover_screen.h"
#include "game.h"
#include <stdio.h>

void gameover_screen_init(GameOverScreen* gos, TextureManager* tm, int score, int stage) {
    gos->current_option = GAMEOVER_RETRY;
    gos->texture_manager = tm;
    gos->final_score = score;
    gos->final_stage = stage;
}

void gameover_screen_handle_input(GameOverScreen* gos, SDL_Event* e, int* next_state) {
    if (e->type == SDL_KEYDOWN) {
        switch (e->key.keysym.sym) {
            case SDLK_UP:
                gos->current_option = (gos->current_option - 1 + GAMEOVER_MENU_COUNT) % GAMEOVER_MENU_COUNT;
                break;
            case SDLK_DOWN:
                gos->current_option = (gos->current_option + 1) % GAMEOVER_MENU_COUNT;
                break;
            case SDLK_RETURN:
            case SDLK_SPACE:
                if (gos->current_option == GAMEOVER_RETRY) {
                    *next_state = GAME_STATE_GAMEPLAY;
                } else if (gos->current_option == GAMEOVER_QUIT) {
                    *next_state = GAME_STATE_TITLE;
                }
                break;
            case SDLK_ESCAPE:
                *next_state = GAME_STATE_TITLE;
                break;
        }
    }
}

void gameover_screen_update(GameOverScreen* gos, float delta_time) {
    // No animation needed for now
    (void)gos;
    (void)delta_time;
}

void gameover_screen_render(GameOverScreen* gos, SDL_Renderer* renderer) {
    // Render background (muted)
    if (gos->texture_manager->background.texture) {
        render_texture(renderer, gos->texture_manager->background.texture, 
                      0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    }
    
    // Render muted overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100); // Dark overlay
    SDL_Rect overlay = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(renderer, &overlay);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    
    // Render Kion defeated character
    if (gos->texture_manager->kion_ded.texture) {
        int kion_scale = 1; // Reduced from 3 to 1 (about half size)
        int kion_width = gos->texture_manager->kion_ded.width * kion_scale;
        int kion_height = gos->texture_manager->kion_ded.height * kion_scale;
        int kion_x = 20; // Left side
        int kion_y = WINDOW_HEIGHT - kion_height; // Flush with bottom
        render_texture(renderer, gos->texture_manager->kion_ded.texture, 
                      kion_x, kion_y, kion_width, kion_height);
    }
    
    // Render "Game Over" text
    if (gos->texture_manager->font_title) {
        SDL_Color red_color = {255, 100, 100, 255};
        
        char gameover_text[] = "Game Over";
        int title_width, title_height;
        SDL_Texture* title_texture = create_text_texture(renderer, gos->texture_manager->font_title, 
                                                        gameover_text, red_color, &title_width, &title_height);
        if (title_texture) {
            int title_x = (WINDOW_WIDTH - title_width) / 2;
            int title_y = 100;
            render_texture(renderer, title_texture, title_x, title_y, title_width, title_height);
            SDL_DestroyTexture(title_texture);
        }
    }
    
    // Render final score and stage
    if (gos->texture_manager->font_regular) {
        SDL_Color white_color = {255, 255, 255, 255};
        
        char score_text[64];
        sprintf(score_text, "Final Score: %d", gos->final_score);
        int score_width, score_height;
        SDL_Texture* score_texture = create_text_texture(renderer, gos->texture_manager->font_regular, 
                                                        score_text, white_color, &score_width, &score_height);
        if (score_texture) {
            int score_x = (WINDOW_WIDTH - score_width) / 2;
            int score_y = 180;
            render_texture(renderer, score_texture, score_x, score_y, score_width, score_height);
            SDL_DestroyTexture(score_texture);
        }
        
        char stage_text[64];
        sprintf(stage_text, "Reached Stage: %d", gos->final_stage);
        int stage_width, stage_height;
        SDL_Texture* stage_texture = create_text_texture(renderer, gos->texture_manager->font_regular, 
                                                        stage_text, white_color, &stage_width, &stage_height);
        if (stage_texture) {
            int stage_x = (WINDOW_WIDTH - stage_width) / 2;
            int stage_y = 220;
            render_texture(renderer, stage_texture, stage_x, stage_y, stage_width, stage_height);
            SDL_DestroyTexture(stage_texture);
        }
    }
    
    // Render menu options
    const char* menu_items[] = {"Retry", "Quit Game"};
    int menu_x = WINDOW_WIDTH - 200;
    int menu_y = 300;
    SDL_Color white_color = {255, 255, 255, 255};
    SDL_Color green_color = {100, 255, 100, 255};
    
    if (gos->texture_manager->font_regular) {
        for (int i = 0; i < GAMEOVER_MENU_COUNT; i++) {
            int text_width, text_height;
            SDL_Color text_color = (i == (int)gos->current_option) ? green_color : white_color;
            
            SDL_Texture* text_texture = create_text_texture(renderer, gos->texture_manager->font_regular, 
                                                           menu_items[i], text_color, &text_width, &text_height);
            if (text_texture) {
                int text_x = menu_x;
                int text_y = menu_y + i * 50;
                render_texture(renderer, text_texture, text_x, text_y, text_width, text_height);
                SDL_DestroyTexture(text_texture);
                
                if (i == (int)gos->current_option && gos->texture_manager->arrow.texture) {
                    int arrow_x = menu_x - 40;
                    int arrow_y = text_y + (text_height - gos->texture_manager->arrow.height) / 2;
                    render_texture(renderer, gos->texture_manager->arrow.texture, 
                                  arrow_x, arrow_y, gos->texture_manager->arrow.width, gos->texture_manager->arrow.height);
                }
            }
        }
    }
}