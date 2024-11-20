#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
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
    SDL_Texture* texture;
} Paddle;

typedef struct {
    int x, y;
    int dx, dy;
    int size;
    SDL_Texture* texture;
} Ball;

typedef struct {
    int x, y;
    bool destroyed;
    SDL_Texture* texture;
} Brick;

Paddle paddle;
Ball ball;
Brick bricks[BRICK_ROWS][BRICK_COLS];
SDL_Texture* background_texture = NULL;
SDL_Window* win = NULL;
SDL_Renderer* renderer = NULL;
bool running = true;
bool move_left = false;
bool move_right = false;

// Helper function to load texture
SDL_Texture* load_texture(SDL_Renderer* renderer, const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    if (surface == NULL) {
        printf("IMG_Load Error for %s: %s\n", path, IMG_GetError());
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (texture == NULL) {
        printf("CreateTextureFromSurface Error for %s: %s\n", path, SDL_GetError());
        return NULL;
    }

    return texture;
}

// Initialize game objects and load textures
bool init_game_objects() {
    // Load background first
    background_texture = load_texture(renderer, "sprites/background.png");
    if (background_texture == NULL) return false;

    // Initialize paddle
    paddle = (Paddle){
        .x = SCREEN_WIDTH / 2 - PADDLE_WIDTH / 2,
        .y = SCREEN_HEIGHT - 40,
        .width = PADDLE_WIDTH,
        .height = PADDLE_HEIGHT,
        .texture = load_texture(renderer, "sprites/paddle.png")
    };

    if (paddle.texture == NULL) return false;

    // Initialize ball
    ball = (Ball){
        .x = SCREEN_WIDTH / 2,
        .y = SCREEN_HEIGHT / 2,
        .dx = 4,
        .dy = -4,
        .size = BALL_SIZE,
        .texture = load_texture(renderer, "sprites/ball.png")
    };

    if (ball.texture == NULL) return false;

    // Initialize bricks
    SDL_Texture* brick_texture = load_texture(renderer, "sprites/brick.png");
    if (brick_texture == NULL) return false;

    for (int i = 0; i < BRICK_ROWS; i++) {
        for (int j = 0; j < BRICK_COLS; j++) {
            bricks[i][j].x = j * BRICK_WIDTH;
            bricks[i][j].y = i * BRICK_HEIGHT;
            bricks[i][j].destroyed = false;
            bricks[i][j].texture = brick_texture;
        }
    }

    return true;
}

void cleanup_game_objects() {
    SDL_DestroyTexture(paddle.texture);
    SDL_DestroyTexture(ball.texture);
    SDL_DestroyTexture(background_texture);

    // Only destroy brick texture once since all bricks share the same texture
    if (BRICK_ROWS > 0 && BRICK_COLS > 0) {
        SDL_DestroyTexture(bricks[0][0].texture);
    }
}

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
        }
    }

    // Update paddle position
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
            Brick* brick = &bricks[i][j];
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
        running = false;
    }

    // Clear screen and draw background
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, background_texture, NULL, NULL);

    // Draw bricks
    for (int i = 0; i < BRICK_ROWS; i++) {
        for (int j = 0; j < BRICK_COLS; j++) {
            if (!bricks[i][j].destroyed) {
                SDL_Rect brickRect = {bricks[i][j].x, bricks[i][j].y, BRICK_WIDTH, BRICK_HEIGHT};
                SDL_RenderCopy(renderer, bricks[i][j].texture, NULL, &brickRect);
            }
        }
    }

    // Draw paddle
    SDL_Rect paddleRect = {paddle.x, paddle.y, paddle.width, paddle.height};
    SDL_RenderCopy(renderer, paddle.texture, NULL, &paddleRect);

    // Draw ball
    SDL_Rect ballRect = {ball.x, ball.y, ball.size, ball.size};
    SDL_RenderCopy(renderer, ball.texture, NULL, &ballRect);

    // Update screen
    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize SDL_image
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("IMG_Init Error: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    // Create window
    win = SDL_CreateWindow("BrickCrush",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (win == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Create renderer
    renderer = SDL_CreateRenderer(win, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        SDL_DestroyWindow(win);
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Initialize game objects and load textures
    if (!init_game_objects()) {
        cleanup_game_objects();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(win);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Main game loop
    while (running) {
        main_loop();
        SDL_Delay(16);  // Cap to roughly 60 FPS
    }

    // Cleanup
    cleanup_game_objects();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
