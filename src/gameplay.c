#include "gameplay.h"
#include "game.h"
#include <math.h>
#include <stdio.h>

void gameplay_init(Gameplay* gp, TextureManager* tm) {
    gp->texture_manager = tm;
    gp->lives = 3;
    gp->score = 0;
    gp->stage = 1;
    
    // Initialize paddle
    float paddle_x = (WINDOW_WIDTH - 64) / 2.0f;
    float paddle_y = WINDOW_HEIGHT - 40;
    paddle_init(&gp->paddle, paddle_x, paddle_y, tm->paddle.texture);
    
    // Initialize ball
    float ball_x = (WINDOW_WIDTH - 16) / 2.0f;
    float ball_y = paddle_y - 20;
    ball_init(&gp->ball, ball_x, ball_y, tm->ball.texture);
    
    // Initialize brick grid  
    SDL_Texture* brick_textures[BRICK_TYPES_COUNT] = {
        tm->brick_red.texture,     // BRICK_RED
        tm->brick_yellow.texture,  // BRICK_ORANGE (using yellow for now)
        tm->brick_yellow.texture,  // BRICK_YELLOW
        tm->brick_green.texture,   // BRICK_GREEN
        tm->brick_blue.texture     // BRICK_BLUE
    };
    brick_grid_init(&gp->brick_grid, brick_textures);
    brick_grid_create_stage(&gp->brick_grid, gp->stage);
}

void gameplay_handle_input(Gameplay* gp, SDL_Event* e, int* next_state) {
    if (e->type == SDL_KEYDOWN) {
        switch (e->key.keysym.sym) {
            case SDLK_ESCAPE:
                *next_state = GAME_STATE_TITLE;
                break;
            case SDLK_SPACE:
                // Advance to next stage
                gp->stage++;
                if (gp->stage > 5) {
                    gp->stage = 1; // Loop back to stage 1 for now
                }
                brick_grid_create_stage(&gp->brick_grid, gp->stage);
                gameplay_reset_ball(gp);
                break;
        }
    }
}

void gameplay_update(Gameplay* gp, float delta_time) {
    const Uint8* keyboard_state = SDL_GetKeyboardState(NULL);
    
    // Update paddle
    paddle_update(&gp->paddle, keyboard_state, delta_time);
    
    // Update ball
    ball_update(&gp->ball, delta_time);
    
    // Check collisions
    gameplay_check_collisions(gp);
    
    // Check if ball went off bottom
    if (gp->ball.y > WINDOW_HEIGHT) {
        gp->lives--;
        if (gp->lives <= 0) {
            gp->lives = 0; // Prevent negative lives
            // TODO: Trigger game over state
        } else {
            gameplay_reset_ball(gp);
        }
    }
}

void gameplay_render(Gameplay* gp, SDL_Renderer* renderer) {
    // Render background
    if (gp->texture_manager->background.texture) {
        render_texture(renderer, gp->texture_manager->background.texture, 
                      0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    }
    
    // Render black header space
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_Rect header_rect = {0, 0, WINDOW_WIDTH, 60};
    SDL_RenderFillRect(renderer, &header_rect);
    
    // Render UI text (lives and score)
    if (gp->texture_manager->font_regular) {
        SDL_Color white_color = {255, 255, 255, 255};
        
        // Lives display
        char lives_text[32];
        sprintf(lives_text, "Lives: %d", gp->lives);
        int lives_width, lives_height;
        SDL_Texture* lives_texture = create_text_texture(renderer, gp->texture_manager->font_regular, 
                                                        lives_text, white_color, &lives_width, &lives_height);
        if (lives_texture) {
            render_texture(renderer, lives_texture, 10, 20, lives_width, lives_height);
            SDL_DestroyTexture(lives_texture);
        }
        
        // Score display
        char score_text[32];
        sprintf(score_text, "Score: %d", gp->score);
        int score_width, score_height;
        SDL_Texture* score_texture = create_text_texture(renderer, gp->texture_manager->font_regular, 
                                                        score_text, white_color, &score_width, &score_height);
        if (score_texture) {
            int score_x = WINDOW_WIDTH - score_width - 10;
            render_texture(renderer, score_texture, score_x, 20, score_width, score_height);
            SDL_DestroyTexture(score_texture);
        }
        
        // Stage display (center)
        char stage_text[32];
        sprintf(stage_text, "Stage: %d", gp->stage);
        int stage_width, stage_height;
        SDL_Texture* stage_texture = create_text_texture(renderer, gp->texture_manager->font_regular, 
                                                        stage_text, white_color, &stage_width, &stage_height);
        if (stage_texture) {
            int stage_x = (WINDOW_WIDTH - stage_width) / 2;
            render_texture(renderer, stage_texture, stage_x, 20, stage_width, stage_height);
            SDL_DestroyTexture(stage_texture);
        }
    }
    
    // Render game objects
    brick_grid_render(&gp->brick_grid, renderer);
    paddle_render(&gp->paddle, renderer);
    ball_render(&gp->ball, renderer);
}

void gameplay_reset_ball(Gameplay* gp) {
    float ball_x = (WINDOW_WIDTH - 16) / 2.0f;
    float ball_y = gp->paddle.y - 20;
    ball_reset(&gp->ball, ball_x, ball_y);
}

bool gameplay_check_collisions(Gameplay* gp) {
    Ball* ball = &gp->ball;
    Paddle* paddle = &gp->paddle;
    
    // Ball-paddle collision
    if (ball->x < paddle->x + paddle->width &&
        ball->x + ball->width > paddle->x &&
        ball->y < paddle->y + paddle->height &&
        ball->y + ball->height > paddle->y) {
        
        // Calculate bounce angle based on hit position
        float hit_pos = (ball->x + ball->width/2) - (paddle->x + paddle->width/2);
        float normalized_hit = hit_pos / (paddle->width/2);
        
        // Reverse Y velocity and add some X variation
        ball->vel_y = -fabs(ball->vel_y);
        ball->vel_x = normalized_hit * 150.0f;
        
        // Make sure ball is above paddle
        ball->y = paddle->y - ball->height;
        
        return true;
    }
    
    // Ball-brick collision
    if (brick_grid_check_collision(&gp->brick_grid, ball->x, ball->y, ball->width, ball->height)) {
        ball_bounce_y(ball);
        gp->score += 10;
        return true;
    }
    
    return false;
}