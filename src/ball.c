#include "ball.h"
#include "game.h"
#include "texture_manager.h"
#include <math.h>

void ball_init(Ball* ball, float x, float y, SDL_Texture* texture) {
    ball->x = x;
    ball->y = y;
    ball->vel_x = 200.0f;  // Initial velocity
    ball->vel_y = -200.0f; // Moving upward initially
    ball->width = 16;
    ball->height = 16;
    ball->texture = texture;
}

void ball_update(Ball* ball, float delta_time, TextureManager* tm) {
    ball->x += ball->vel_x * delta_time;
    ball->y += ball->vel_y * delta_time;
    
    // Wall collision detection
    if (ball->x <= 0) {
        ball->x = 0;
        ball_bounce_x(ball, tm);
    }
    if (ball->x + ball->width >= WINDOW_WIDTH) {
        ball->x = WINDOW_WIDTH - ball->width;
        ball_bounce_x(ball, tm);
    }
    // Top boundary is below the UI header (60px)
    int header_height = 60;
    if (ball->y <= header_height) {
        ball->y = header_height;
        ball_bounce_y(ball, tm);
    }
    
    // Ball goes off bottom - will be handled by game logic
}

void ball_render(Ball* ball, SDL_Renderer* renderer) {
    if (ball->texture) {
        SDL_Rect dest_rect = {
            (int)ball->x, (int)ball->y, 
            ball->width, ball->height
        };
        SDL_RenderCopy(renderer, ball->texture, NULL, &dest_rect);
    }
}

void ball_bounce_x(Ball* ball, TextureManager* tm) {
    ball->vel_x = -ball->vel_x;
    play_sfx(tm->sfx_ball_wall);
}

void ball_bounce_y(Ball* ball, TextureManager* tm) {
    ball->vel_y = -ball->vel_y;
    play_sfx(tm->sfx_ball_wall);
}

void ball_reset(Ball* ball, float x, float y) {
    ball->x = x;
    ball->y = y;
    ball->vel_x = 200.0f;
    ball->vel_y = -200.0f;
}