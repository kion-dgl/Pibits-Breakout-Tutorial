#include "brick.h"
#include <stdlib.h>

void brick_init(Brick* brick, float x, float y, BrickType type, SDL_Texture* texture) {
    brick->x = x;
    brick->y = y;
    brick->width = 50;
    brick->height = 20;
    brick->type = type;
    brick->destroyed = false;
    brick->texture = texture;
}

void brick_render(Brick* brick, SDL_Renderer* renderer) {
    if (!brick->destroyed && brick->texture) {
        SDL_Rect dest_rect = {
            (int)brick->x, (int)brick->y,
            brick->width, brick->height
        };
        SDL_RenderCopy(renderer, brick->texture, NULL, &dest_rect);
    }
}

void brick_grid_init(BrickGrid* grid, SDL_Texture* textures[BRICK_TYPES_COUNT]) {
    grid->count = 0;
    for (int i = 0; i < BRICK_TYPES_COUNT; i++) {
        grid->textures[i] = textures[i];
    }
}

void brick_grid_create_stage(BrickGrid* grid, int stage) {
    grid->count = 0;
    
    // Stage 1: Classic horizontal rows
    if (stage == 1) {
        int start_x = 50;
        int start_y = 100;
        int brick_width = 52;
        int brick_height = 22;
        int cols = 10;
        int rows = 5;
        
        BrickType row_types[] = {BRICK_RED, BRICK_ORANGE, BRICK_YELLOW, BRICK_GREEN, BRICK_BLUE};
        
        for (int row = 0; row < rows; row++) {
            for (int col = 0; col < cols; col++) {
                if (grid->count < MAX_BRICKS) {
                    float x = start_x + col * brick_width;
                    float y = start_y + row * brick_height;
                    BrickType type = row_types[row % BRICK_TYPES_COUNT];
                    
                    brick_init(&grid->bricks[grid->count], x, y, type, grid->textures[type]);
                    grid->count++;
                }
            }
        }
    }
}

void brick_grid_render(BrickGrid* grid, SDL_Renderer* renderer) {
    for (int i = 0; i < grid->count; i++) {
        brick_render(&grid->bricks[i], renderer);
    }
}

bool brick_grid_check_collision(BrickGrid* grid, float ball_x, float ball_y, float ball_w, float ball_h) {
    for (int i = 0; i < grid->count; i++) {
        Brick* brick = &grid->bricks[i];
        if (brick->destroyed) continue;
        
        // Simple AABB collision detection
        if (ball_x < brick->x + brick->width &&
            ball_x + ball_w > brick->x &&
            ball_y < brick->y + brick->height &&
            ball_y + ball_h > brick->y) {
            
            brick->destroyed = true;
            return true; // Collision detected
        }
    }
    return false;
}