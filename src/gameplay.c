#include "gameplay.h"
#include "game.h"
#include <math.h>

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
            // Game over - will be handled by state management
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
    
    // Render game objects
    brick_grid_render(&gp->brick_grid, renderer);
    paddle_render(&gp->paddle, renderer);
    ball_render(&gp->ball, renderer);
    
    // TODO: Render UI (lives, score)
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