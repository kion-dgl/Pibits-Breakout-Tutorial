#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define PADDLE_WIDTH 100
#define PADDLE_HEIGHT 20
#define BALL_SIZE 15
#define BRICK_ROWS 5
#define BRICK_COLS 8
#define BRICK_WIDTH (SCREEN_WIDTH / BRICK_COLS)
#define BRICK_HEIGHT 30

typedef struct {
    int x, y;
    int width, height;
} Paddle;

typedef struct {
    int x, y;
    int dx, dy;
    int size;
} Ball;

typedef struct {
    int x, y;
    bool destroyed;
} Brick;

Paddle paddle;
Ball ball;
Brick bricks[BRICK_ROWS][BRICK_COLS];
SDL_Window *win = NULL;
SDL_Renderer *renderer = NULL;
bool running = true;
bool move_left = false;
bool move_right = false;

void main_loop() {
    SDL_Event e;

    // Handle events
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
        } else if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_LEFT) {
                move_left = true;
            } else if (e.key.keysym.sym == SDLK_RIGHT) {
                move_right = true;
            }
        } else if (e.type == SDL_KEYUP) {
            if (e.key.keysym.sym == SDLK_LEFT) {
                move_left = false;
            } else if (e.key.keysym.sym == SDLK_RIGHT) {
                move_right = false;
            }
        } else if (e.type == SDL_JOYAXISMOTION) {
            if (e.jaxis.axis == 0) { // Left/Right axis on the joystick
                if (e.jaxis.value < -8000) {
                    move_left = true;
                    move_right = false;
                } else if (e.jaxis.value > 8000) {
                    move_right = true;
                    move_left = false;
                } else {
                    move_left = false;
                    move_right = false;
                }
            }
        } else if (e.type == SDL_JOYHATMOTION) {
            if (e.jhat.value & SDL_HAT_LEFT) {
                move_left = true;
            } else if (e.jhat.value & SDL_HAT_RIGHT) {
                move_right = true;
            } else {
                move_left = false;
                move_right = false;
            }
        }
    }

    // Update paddle position based on input
    if (move_left) {
        paddle.x -= 10;
    }
    if (move_right) {
        paddle.x += 10;
    }

    // Keep paddle within screen bounds
    if (paddle.x < 0) paddle.x = 0;
    if (paddle.x + paddle.width > SCREEN_WIDTH) paddle.x = SCREEN_WIDTH - paddle.width;

    // Update ball position
    ball.x += ball.dx;
    ball.y += ball.dy;

    // Ball collision with walls
    if (ball.x <= 0 || ball.x + ball.size >= SCREEN_WIDTH) {
        ball.dx = -ball.dx;
    }
    if (ball.y <= 0) {
        ball.dy = -ball.dy;
    }

    // Ball collision with paddle
    if (ball.y + ball.size >= paddle.y && ball.x + ball.size >= paddle.x && ball.x <= paddle.x + paddle.width) {
        ball.dy = -ball.dy;
    }

    // Ball collision with bricks
    for (int i = 0; i < BRICK_ROWS; i++) {
        for (int j = 0; j < BRICK_COLS; j++) {
            Brick *brick = &bricks[i][j];
            if (!brick->destroyed &&
                ball.x + ball.size > brick->x && ball.x < brick->x + BRICK_WIDTH &&
                ball.y + ball.size > brick->y && ball.y < brick->y + BRICK_HEIGHT) {
                brick->destroyed = true;
                ball.dy = -ball.dy;
            }
        }
    }

    // Ball falls below the screen
    if (ball.y > SCREEN_HEIGHT) {
        running = false; // Game over
    }

    // Clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    // Draw paddle
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_Rect paddleRect = {paddle.x, paddle.y, paddle.width, paddle.height};
    SDL_RenderFillRect(renderer, &paddleRect);

    // Draw ball
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_Rect ballRect = {ball.x, ball.y, ball.size, ball.size};
    SDL_RenderFillRect(renderer, &ballRect);

    // Draw bricks
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
    for (int i = 0; i < BRICK_ROWS; i++) {
        for (int j = 0; j < BRICK_COLS; j++) {
            if (!bricks[i][j].destroyed) {
                SDL_Rect brickRect = {bricks[i][j].x, bricks[i][j].y, BRICK_WIDTH, BRICK_HEIGHT};
                SDL_RenderFillRect(renderer, &brickRect);
            }
        }
    }

    // Update screen
    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    // Open the first available joystick
    if (SDL_NumJoysticks() > 0) {
        SDL_Joystick *joystick = SDL_JoystickOpen(0);
        if (joystick == NULL) {
            printf("SDL_JoystickOpen Error: %s\n", SDL_GetError());
        }
    }

    // Create a window
    win = SDL_CreateWindow("BrickCrush", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (win == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create a renderer
    renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        SDL_DestroyWindow(win);
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Initialize game objects
    paddle = (Paddle){SCREEN_WIDTH / 2 - PADDLE_WIDTH / 2, SCREEN_HEIGHT - 40, PADDLE_WIDTH, PADDLE_HEIGHT};
    ball = (Ball){SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 4, -4, BALL_SIZE};

    for (int i = 0; i < BRICK_ROWS; i++) {
        for (int j = 0; j < BRICK_COLS; j++) {
            bricks[i][j].x = j * BRICK_WIDTH;
            bricks[i][j].y = i * BRICK_HEIGHT;
            bricks[i][j].destroyed = false;
        }
    }

    // Main game loop
    while (running) {
        main_loop();
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}

