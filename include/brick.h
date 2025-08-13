#ifndef BRICK_H
#define BRICK_H

#include <SDL.h>
#include <stdbool.h>

typedef enum {
    BRICK_RED,
    BRICK_ORANGE,
    BRICK_YELLOW,
    BRICK_GREEN,
    BRICK_BLUE,
    BRICK_TYPES_COUNT
} BrickType;

typedef struct {
    float x, y;           // Position
    int width, height;    // Size
    BrickType type;       // Brick color/type
    bool destroyed;       // Is brick destroyed?
    SDL_Texture* texture; // Brick texture
} Brick;

#define MAX_BRICKS 100

typedef struct {
    Brick bricks[MAX_BRICKS];
    int count;
    SDL_Texture* textures[BRICK_TYPES_COUNT];
} BrickGrid;

void brick_init(Brick* brick, float x, float y, BrickType type, SDL_Texture* texture);
void brick_render(Brick* brick, SDL_Renderer* renderer);
void brick_grid_init(BrickGrid* grid, SDL_Texture* textures[BRICK_TYPES_COUNT]);
void brick_grid_create_stage(BrickGrid* grid, int stage);
void brick_grid_render(BrickGrid* grid, SDL_Renderer* renderer);
bool brick_grid_check_collision(BrickGrid* grid, float ball_x, float ball_y, float ball_w, float ball_h);
bool brick_grid_all_destroyed(BrickGrid* grid);

#endif