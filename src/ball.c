#include "ball.h"
#include "game.h"
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

void ball_update(Ball* ball, float delta_time) {
    ball->x += ball->vel_x * delta_time;
    ball->y += ball->vel_y * delta_time;
    
    // Wall collision detection
    if (ball->x <= 0) {
        ball->x = 0;
        ball_bounce_x(ball);
    }
    if (ball->x + ball->width >= WINDOW_WIDTH) {
        ball->x = WINDOW_WIDTH - ball->width;
        ball_bounce_x(ball);
    }
    if (ball->y <= 0) {
        ball->y = 0;
        ball_bounce_y(ball);
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

void ball_bounce_x(Ball* ball) {
    ball->vel_x = -ball->vel_x;
}

void ball_bounce_y(Ball* ball) {
    ball->vel_y = -ball->vel_y;
}

void ball_reset(Ball* ball, float x, float y) {
    ball->x = x;
    ball->y = y;
    ball->vel_x = 200.0f;
    ball->vel_y = -200.0f;
}