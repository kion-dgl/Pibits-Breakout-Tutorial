#include "ball.h"
#include "game.h"
#include "texture_manager.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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
    
    // Accelerate ball slightly (1% speed increase per bounce)
    ball->vel_x *= 1.01f;
    ball->vel_y *= 1.01f;
    
    // Cap maximum speed to prevent it getting too crazy
    float max_speed = 500.0f;
    if (fabs(ball->vel_x) > max_speed) {
        ball->vel_x = ball->vel_x > 0 ? max_speed : -max_speed;
    }
    if (fabs(ball->vel_y) > max_speed) {
        ball->vel_y = ball->vel_y > 0 ? max_speed : -max_speed;
    }
    
    play_sfx(tm->sfx_ball_wall);
}

void ball_bounce_y(Ball* ball, TextureManager* tm) {
    ball->vel_y = -ball->vel_y;
    
    // Accelerate ball slightly (1% speed increase per bounce)
    ball->vel_x *= 1.01f;
    ball->vel_y *= 1.01f;
    
    // Cap maximum speed to prevent it getting too crazy
    float max_speed = 500.0f;
    if (fabs(ball->vel_x) > max_speed) {
        ball->vel_x = ball->vel_x > 0 ? max_speed : -max_speed;
    }
    if (fabs(ball->vel_y) > max_speed) {
        ball->vel_y = ball->vel_y > 0 ? max_speed : -max_speed;
    }
    
    play_sfx(tm->sfx_ball_wall);
}

void ball_reset(Ball* ball, float x, float y) {
    ball->x = x;
    ball->y = y;
    
    // Always go upward, but randomize the horizontal direction and angle
    ball->vel_y = -200.0f; // Always upward
    
    // Random angle between -60 to +60 degrees (converted to radians)
    float angle_degrees = -60.0f + (rand() % 121); // -60 to +60
    float angle_radians = angle_degrees * M_PI / 180.0f;
    
    // Calculate X velocity based on angle, maintaining roughly same speed
    ball->vel_x = 200.0f * sinf(angle_radians);
    
    // Ensure Y velocity magnitude is consistent
    ball->vel_y = -200.0f * cosf(angle_radians);
}