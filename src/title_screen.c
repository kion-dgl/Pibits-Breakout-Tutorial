#include "title_screen.h"
#include "game.h"
#include <math.h>

void title_screen_init(TitleScreen* ts, TextureManager* tm) {
    ts->current_option = MENU_START_GAME;
    ts->arrow_rotation = 0.0f;
    ts->texture_manager = tm;
    
    // Start title screen BGM
    play_bgm(tm->bgm_title);
}

void title_screen_handle_input(TitleScreen* ts, SDL_Event* e, int* next_state) {
    if (e->type == SDL_KEYDOWN) {
        switch (e->key.keysym.sym) {
            case SDLK_UP:
                ts->current_option = (ts->current_option - 1 + MENU_COUNT) % MENU_COUNT;
                play_sfx(ts->texture_manager->sfx_menu_select);
                break;
            case SDLK_DOWN:
                ts->current_option = (ts->current_option + 1) % MENU_COUNT;
                play_sfx(ts->texture_manager->sfx_menu_select);
                break;
            case SDLK_RETURN:
            case SDLK_SPACE:
                play_sfx(ts->texture_manager->sfx_menu_select);
                if (ts->current_option == MENU_START_GAME) {
                    *next_state = GAME_STATE_GAMEPLAY;
                } else if (ts->current_option == MENU_QUIT_GAME) {
                    *next_state = GAME_STATE_QUIT;
                }
                break;
        }
    }
}

void title_screen_update(TitleScreen* ts, float delta_time) {
    ts->arrow_rotation += delta_time * 180.0f;
    if (ts->arrow_rotation >= 360.0f) {
        ts->arrow_rotation -= 360.0f;
    }
}

void title_screen_render(TitleScreen* ts, SDL_Renderer* renderer) {
    if (ts->texture_manager->background.texture) {
        render_texture(renderer, ts->texture_manager->background.texture, 
                      0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    }
    
    if (ts->texture_manager->logo.texture) {
        int logo_width = 300;
        int logo_height = (ts->texture_manager->logo.height * logo_width) / ts->texture_manager->logo.width;
        int logo_x = 50;
        int logo_y = 50;
        render_texture(renderer, ts->texture_manager->logo.texture, 
                      logo_x, logo_y, logo_width, logo_height);
    }
    
    if (ts->texture_manager->dashie.texture) {
        int dashie_width = ts->texture_manager->dashie.width;
        int dashie_height = ts->texture_manager->dashie.height;
        int dashie_x = WINDOW_WIDTH - dashie_width - 10;
        int dashie_y = WINDOW_HEIGHT - dashie_height;
        render_texture(renderer, ts->texture_manager->dashie.texture, 
                      dashie_x, dashie_y, dashie_width, dashie_height);
    }
    
    const char* menu_items[] = {"Start Game", "Quit Game"};
    int menu_x = 50;
    int menu_y = 300;
    SDL_Color white_color = {255, 255, 255, 255};
    SDL_Color yellow_color = {255, 255, 0, 255};
    
    for (int i = 0; i < MENU_COUNT; i++) {
        int text_width, text_height;
        SDL_Color text_color = (i == (int)ts->current_option) ? yellow_color : white_color;
        
        if (ts->texture_manager->font_regular) {
            SDL_Texture* text_texture = create_text_texture(renderer, ts->texture_manager->font_regular, 
                                                           menu_items[i], text_color, &text_width, &text_height);
            if (text_texture) {
                int text_x = menu_x;
                int text_y = menu_y + i * 50;
                render_texture(renderer, text_texture, text_x, text_y, text_width, text_height);
                SDL_DestroyTexture(text_texture);
                
                if (i == (int)ts->current_option && ts->texture_manager->arrow.texture) {
                    int arrow_x = menu_x - 40;
                    int arrow_y = text_y + (text_height - ts->texture_manager->arrow.height) / 2;
                    render_texture(renderer, ts->texture_manager->arrow.texture, 
                                  arrow_x, arrow_y, ts->texture_manager->arrow.width, ts->texture_manager->arrow.height);
                }
            }
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_Rect text_rect = {menu_x, menu_y + i * 50, 120, 20};
            SDL_RenderFillRect(renderer, &text_rect);
            
            if (i == (int)ts->current_option) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                SDL_Rect arrow_rect = {menu_x - 30, menu_y + i * 50 + 5, 20, 10};
                SDL_RenderFillRect(renderer, &arrow_rect);
            }
        }
    }
}