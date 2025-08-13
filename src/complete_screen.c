#include "complete_screen.h"
#include "game.h"
#include <stdio.h>

void complete_screen_init(CompleteScreen* cs, TextureManager* tm, int score) {
    printf("DEBUG: complete_screen_init called with score: %d\n", score);
    cs->current_option = COMPLETE_PLAY_AGAIN;
    cs->texture_manager = tm;
    cs->final_score = score;
    printf("DEBUG: complete_screen_init completed\n");
}

void complete_screen_handle_input(CompleteScreen* cs, SDL_Event* e, int* next_state) {
    if (e->type == SDL_KEYDOWN) {
        switch (e->key.keysym.sym) {
            case SDLK_UP:
                cs->current_option = (cs->current_option - 1 + COMPLETE_MENU_COUNT) % COMPLETE_MENU_COUNT;
                break;
            case SDLK_DOWN:
                cs->current_option = (cs->current_option + 1) % COMPLETE_MENU_COUNT;
                break;
            case SDLK_RETURN:
            case SDLK_SPACE:
                if (cs->current_option == COMPLETE_PLAY_AGAIN) {
                    *next_state = GAME_STATE_TITLE;
                } else if (cs->current_option == COMPLETE_QUIT) {
                    *next_state = GAME_STATE_TITLE;
                }
                break;
            case SDLK_ESCAPE:
                *next_state = GAME_STATE_TITLE;
                break;
        }
    }
}

void complete_screen_update(CompleteScreen* cs, float delta_time) {
    // No animation needed for now
    (void)cs;
    (void)delta_time;
}

void complete_screen_render(CompleteScreen* cs, SDL_Renderer* renderer) {
    printf("DEBUG: complete_screen_render started\n");
    
    // Render background (bright)
    if (cs->texture_manager->background.texture) {
        printf("DEBUG: Rendering background\n");
        render_texture(renderer, cs->texture_manager->background.texture, 
                      0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        printf("DEBUG: Background rendered\n");
    }
    
    // Render bright overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 50); // Golden overlay
    SDL_Rect overlay = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(renderer, &overlay);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    
    // Render Kion happy character - match Kion-ded positioning
    if (cs->texture_manager->kion_happi.texture) {
        int kion_scale = 1; // Same scale as Kion-ded
        int kion_width = cs->texture_manager->kion_happi.width * kion_scale;
        int kion_height = cs->texture_manager->kion_happi.height * kion_scale;
        int kion_x = 20; // Same position as Kion-ded
        int kion_y = WINDOW_HEIGHT - kion_height; // Flush with bottom like Kion-ded
        render_texture(renderer, cs->texture_manager->kion_happi.texture, 
                      kion_x, kion_y, kion_width, kion_height);
    }
    
    // Render "Complete!" text
    if (cs->texture_manager->font_title) {
        SDL_Color gold_color = {255, 215, 0, 255};
        
        char complete_text[] = "Complete!";
        int title_width, title_height;
        SDL_Texture* title_texture = create_text_texture(renderer, cs->texture_manager->font_title, 
                                                        complete_text, gold_color, &title_width, &title_height);
        if (title_texture) {
            int title_x = (WINDOW_WIDTH - title_width) / 2;
            int title_y = 100;
            render_texture(renderer, title_texture, title_x, title_y, title_width, title_height);
            SDL_DestroyTexture(title_texture);
        }
    }
    
    // Render final score
    if (cs->texture_manager->font_regular) {
        SDL_Color white_color = {255, 255, 255, 255};
        
        char score_text[64];
        sprintf(score_text, "Final Score: %d", cs->final_score);
        int score_width, score_height;
        SDL_Texture* score_texture = create_text_texture(renderer, cs->texture_manager->font_regular, 
                                                        score_text, white_color, &score_width, &score_height);
        if (score_texture) {
            int score_x = (WINDOW_WIDTH - score_width) / 2;
            int score_y = 180;
            render_texture(renderer, score_texture, score_x, score_y, score_width, score_height);
            SDL_DestroyTexture(score_texture);
        }
        
        char congratulations[] = "All stages complete!";
        int congrats_width, congrats_height;
        SDL_Texture* congrats_texture = create_text_texture(renderer, cs->texture_manager->font_regular, 
                                                          congratulations, white_color, &congrats_width, &congrats_height);
        if (congrats_texture) {
            int congrats_x = (WINDOW_WIDTH - congrats_width) / 2;
            int congrats_y = 220;
            render_texture(renderer, congrats_texture, congrats_x, congrats_y, congrats_width, congrats_height);
            SDL_DestroyTexture(congrats_texture);
        }
    }
    
    // Render menu options
    const char* menu_items[] = {"Play Again", "Quit Game"};
    int menu_x = WINDOW_WIDTH - 220;
    int menu_y = 300;
    SDL_Color white_color = {255, 255, 255, 255};
    SDL_Color yellow_color = {255, 255, 0, 255};
    
    if (cs->texture_manager->font_regular) {
        for (int i = 0; i < COMPLETE_MENU_COUNT; i++) {
            int text_width, text_height;
            SDL_Color text_color = (i == (int)cs->current_option) ? yellow_color : white_color;
            
            SDL_Texture* text_texture = create_text_texture(renderer, cs->texture_manager->font_regular, 
                                                           menu_items[i], text_color, &text_width, &text_height);
            if (text_texture) {
                int text_x = menu_x;
                int text_y = menu_y + i * 50;
                render_texture(renderer, text_texture, text_x, text_y, text_width, text_height);
                SDL_DestroyTexture(text_texture);
                
                if (i == (int)cs->current_option && cs->texture_manager->arrow.texture) {
                    int arrow_x = menu_x - 40;
                    int arrow_y = text_y + (text_height - cs->texture_manager->arrow.height) / 2;
                    render_texture(renderer, cs->texture_manager->arrow.texture, 
                                  arrow_x, arrow_y, cs->texture_manager->arrow.width, cs->texture_manager->arrow.height);
                }
            }
        }
    }
}