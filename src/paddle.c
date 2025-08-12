#include "paddle.h"
#include "game.h"

void paddle_init(Paddle* paddle, float x, float y, SDL_Texture* texture) {
    paddle->x = x;
    paddle->y = y;
    paddle->width = 64;
    paddle->height = 16;
    paddle->speed = 300.0f;
    paddle->texture = texture;
}

void paddle_update(Paddle* paddle, const Uint8* keyboard_state, float delta_time) {
    if (keyboard_state[SDL_SCANCODE_LEFT]) {
        paddle->x -= paddle->speed * delta_time;
    }
    if (keyboard_state[SDL_SCANCODE_RIGHT]) {
        paddle->x += paddle->speed * delta_time;
    }
    
    // Keep paddle within screen bounds
    if (paddle->x < 0) {
        paddle->x = 0;
    }
    if (paddle->x + paddle->width > WINDOW_WIDTH) {
        paddle->x = WINDOW_WIDTH - paddle->width;
    }
}

void paddle_render(Paddle* paddle, SDL_Renderer* renderer) {
    if (paddle->texture) {
        SDL_Rect dest_rect = {
            (int)paddle->x, (int)paddle->y,
            paddle->width, paddle->height
        };
        SDL_RenderCopy(renderer, paddle->texture, NULL, &dest_rect);
    }
}