#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define PADDLE_WIDTH 100
#define PADDLE_HEIGHT 20
#define BALL_SIZE 15
#define BRICK_ROWS 6
#define BRICK_COLS 8
#define BRICK_WIDTH (SCREEN_WIDTH / BRICK_COLS)
#define BRICK_HEIGHT 30
#define SCORE_HEIGHT 40
#define INITIAL_LIVES 3
#define MIN_BALL_SPEED 4
#define MAX_BALL_SPEED 8

// Controller button mappings
#define START_BUTTON 8
#define A_BUTTON 0
#define B_BUTTON 1
#define LEFT_BUTTON 12
#define RIGHT_BUTTON 13

#define HUD_FONT_SIZE 24  // Smaller font size for HUD
#define MENU_FONT_SIZE 36  // Larger font size for menus

typedef enum {
    GAME_STATE_START_SCREEN,
    GAME_STATE_PLAYING,
    GAME_STATE_WIN_SCREEN,
    GAME_STATE_GAME_OVER
} GameState;

typedef enum {
    BRICK_RED,
    BRICK_BLUE,
    BRICK_YELLOW
} BrickType;

typedef struct {
    int x, y;
    int width, height;
    SDL_Texture* texture;
} Paddle;

typedef struct {
    int x, y;
    double dx, dy;
    int size;
    SDL_Texture* texture;
} Ball;

typedef struct {
    int x, y;
    bool destroyed;
    BrickType type;
    SDL_Texture* texture;
} Brick;

typedef struct {
    Mix_Music* background_music;
    Mix_Chunk* brick_hit;
    Mix_Chunk* paddle_hit;
} AudioAssets;

typedef struct {
    int score;
    int lives;
    int combo;
    SDL_Texture* score_text;
    SDL_Texture* lives_text;
} GameStats;

// Global Variables
Paddle paddle;
Ball ball;
Brick bricks[BRICK_ROWS][BRICK_COLS];
AudioAssets audio = {NULL, NULL, NULL};
GameStats stats = {0};
SDL_Window* win = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font_hud = NULL;      // Smaller font for HUD
TTF_Font* font_menu = NULL;     // Larger font for menus
SDL_Joystick* joystick = NULL;  // Global joystick handle
GameState game_state = GAME_STATE_START_SCREEN;
bool running = true;
bool move_left = false;
bool move_right = false;

// Textures
SDL_Texture* background_texture = NULL;
SDL_Texture* startscreen_texture = NULL;
SDL_Texture* start_text_texture = NULL;
SDL_Texture* gameover_texture = NULL;
SDL_Texture* youwin_texture = NULL;
SDL_Texture* restart_text_texture = NULL;
SDL_Texture* quit_text_texture = NULL;
SDL_Texture* brick_textures[3] = {NULL};  // Red, Blue, Yellow

// Function declarations
bool init_audio();
void cleanup_audio();
SDL_Texture* load_texture(SDL_Renderer* renderer, const char* path);
SDL_Texture* create_text_texture(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color);
bool init_game_objects();
void cleanup_game_objects();
void reset_game();
void handle_start_screen_events(SDL_Event* e);
void handle_end_screen_events(SDL_Event* e);
void render_start_screen();
void render_end_screen(bool is_win);
void main_loop();
void update_score_display();
void update_lives_display();
void randomize_ball_velocity();
void reset_ball();
void handle_paddle_collision();
void handle_brick_collisions();
void handle_ball_loss();
void render_game_stats();

int calculate_brick_score(int combo) {
    return (combo == 0) ? 50 : (combo >= 5) ? 100 : 50 + (combo * 10);
}

void update_score_display() {
    if (stats.score_text != NULL) {
        SDL_DestroyTexture(stats.score_text);
    }
    char score_str[32];
    snprintf(score_str, sizeof(score_str), "Score: %d", stats.score);
    SDL_Color white = {255, 255, 255, 255};
    stats.score_text = create_text_texture(renderer, font_hud, score_str, white);
}

void update_lives_display() {
    if (stats.lives_text != NULL) {
        SDL_DestroyTexture(stats.lives_text);
    }
    char lives_str[32];
    snprintf(lives_str, sizeof(lives_str), "Lives: %d", stats.lives);
    SDL_Color white = {255, 255, 255, 255};
    stats.lives_text = create_text_texture(renderer, font_hud, lives_str, white);
}

void randomize_ball_velocity() {
    float angle = ((float)rand() / RAND_MAX * 90.0f - 45.0f) * M_PI / 180.0f;
    float speed = MIN_BALL_SPEED;
    ball.dx = speed * sin(angle);
    ball.dy = -speed * cos(angle);
}

void reset_ball() {
    ball.x = SCREEN_WIDTH / 2;
    ball.y = SCREEN_HEIGHT / 2;
    randomize_ball_velocity();
}

bool init_audio() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer Init Error: %s\n", Mix_GetError());
        return false;
    }

    audio.background_music = Mix_LoadMUS("sound/background_music.ogg");
    if (audio.background_music == NULL) {
        printf("Failed to load background music: %s\n", Mix_GetError());
        return false;
    }

    audio.brick_hit = Mix_LoadWAV("sound/brick_hit.ogg");
    if (audio.brick_hit == NULL) {
        printf("Failed to load brick hit sound: %s\n", Mix_GetError());
        return false;
    }

    audio.paddle_hit = Mix_LoadWAV("sound/paddle_hit.ogg");
    if (audio.paddle_hit == NULL) {
        printf("Failed to load paddle hit sound: %s\n", Mix_GetError());
        return false;
    }

    return true;
}

void cleanup_audio() {
    if (audio.background_music != NULL) {
        Mix_FreeMusic(audio.background_music);
    }
    if (audio.brick_hit != NULL) {
        Mix_FreeChunk(audio.brick_hit);
    }
    if (audio.paddle_hit != NULL) {
        Mix_FreeChunk(audio.paddle_hit);
    }
    Mix_CloseAudio();
}

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

SDL_Texture* create_text_texture(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    if (surface == NULL) {
        printf("TTF_RenderText_Blended Error: %s\n", TTF_GetError());
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (texture == NULL) {
        printf("CreateTextureFromSurface Error: %s\n", SDL_GetError());
        return NULL;
    }

    return texture;
}

bool init_game_objects() {
    // Load all screen textures
    startscreen_texture = load_texture(renderer, "sprites/startscreen.png");
    if (startscreen_texture == NULL) return false;

    background_texture = load_texture(renderer, "sprites/background.png");
    if (background_texture == NULL) return false;

    gameover_texture = load_texture(renderer, "sprites/gameover.png");
    if (gameover_texture == NULL) return false;

    youwin_texture = load_texture(renderer, "sprites/youwin.png");
    if (youwin_texture == NULL) return false;

    // Load brick textures
    brick_textures[BRICK_RED] = load_texture(renderer, "sprites/brick-red.png");
    brick_textures[BRICK_BLUE] = load_texture(renderer, "sprites/brick-blue.png");
    brick_textures[BRICK_YELLOW] = load_texture(renderer, "sprites/brick-yellow.png");

    for (int i = 0; i < 3; i++) {
        if (brick_textures[i] == NULL) return false;
    }

    // Create text textures
    SDL_Color white = {255, 255, 255, 255};
    start_text_texture = create_text_texture(renderer, font_menu, "Press START Button", white);
    if (start_text_texture == NULL) return false;

    restart_text_texture = create_text_texture(renderer, font_menu, "Press START to Restart", white);
    if (restart_text_texture == NULL) return false;

    quit_text_texture = create_text_texture(renderer, font_menu, "Press B to Quit", white);
    if (quit_text_texture == NULL) return false;

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
        .size = BALL_SIZE,
        .texture = load_texture(renderer, "sprites/ball.png")
    };
    if (ball.texture == NULL) return false;

    // Initialize bricks
    for (int i = 0; i < BRICK_ROWS; i++) {
        BrickType type;
        if (i < 2) type = BRICK_RED;
        else if (i < 4) type = BRICK_BLUE;
        else type = BRICK_YELLOW;

        for (int j = 0; j < BRICK_COLS; j++) {
            bricks[i][j].x = j * BRICK_WIDTH;
            bricks[i][j].y = i * BRICK_HEIGHT + SCORE_HEIGHT;
            bricks[i][j].destroyed = false;
            bricks[i][j].type = type;
            bricks[i][j].texture = brick_textures[type];
        }
    }

    // Initialize stats
    stats.score = 0;
    stats.lives = INITIAL_LIVES;
    stats.combo = 0;
    update_score_display();
    update_lives_display();

    randomize_ball_velocity();
    return true;
}

void cleanup_game_objects() {
    SDL_DestroyTexture(paddle.texture);
    SDL_DestroyTexture(ball.texture);
    SDL_DestroyTexture(background_texture);
    SDL_DestroyTexture(startscreen_texture);
    SDL_DestroyTexture(start_text_texture);
    SDL_DestroyTexture(gameover_texture);
    SDL_DestroyTexture(youwin_texture);
    SDL_DestroyTexture(restart_text_texture);
    SDL_DestroyTexture(quit_text_texture);
    SDL_DestroyTexture(stats.score_text);
    SDL_DestroyTexture(stats.lives_text);

    for (int i = 0; i < 3; i++) {
        if (brick_textures[i] != NULL) {
            SDL_DestroyTexture(brick_textures[i]);
        }
    }
}

bool check_all_bricks_destroyed() {
    for (int i = 0; i < BRICK_ROWS; i++) {
        for (int j = 0; j < BRICK_COLS; j++) {
            if (!bricks[i][j].destroyed) {
                return false;
            }
        }
    }
    return true;
}

void reset_game() {
    // Reset ball and paddle
    paddle.x = SCREEN_WIDTH / 2 - PADDLE_WIDTH / 2;
    reset_ball();

    // Reset bricks
    for (int i = 0; i < BRICK_ROWS; i++) {
        BrickType type;
        if (i < 2) type = BRICK_RED;
        else if (i < 4) type = BRICK_BLUE;
        else type = BRICK_YELLOW;

        for (int j = 0; j < BRICK_COLS; j++) {
            bricks[i][j].destroyed = false;
            bricks[i][j].type = type;
            bricks[i][j].texture = brick_textures[type];
        }
    }

    // Reset stats
    stats.score = 0;
    stats.lives = INITIAL_LIVES;
    stats.combo = 0;
    update_score_display();
    update_lives_display();

    // Ensure music is playing
    if (!Mix_PlayingMusic()) {
        Mix_PlayMusic(audio.background_music, -1);
    }

    game_state = GAME_STATE_PLAYING;
}

void handle_paddle_collision() {
    if (ball.y + ball.size >= paddle.y &&
        ball.x + ball.size >= paddle.x &&
        ball.x <= paddle.x + paddle.width) {

        // Calculate where on the paddle the ball hit (0 to 1)
        float hit_position = (ball.x + ball.size/2 - paddle.x) / paddle.width;

        // Adjust angle based on hit position (-60 to 60 degrees)
        float angle = (hit_position - 0.5f) * M_PI * 2/3;  // Convert to radians

        // Maintain speed but change direction
        float current_speed = sqrt(ball.dx * ball.dx + ball.dy * ball.dy);
        ball.dx = current_speed * sin(angle);
        ball.dy = -current_speed * cos(angle);

        Mix_PlayChannel(-1, audio.paddle_hit, 0);
        stats.combo = 0;
    }
}

void handle_brick_collisions() {
    bool hit = false;
    for (int i = 0; i < BRICK_ROWS; i++) {
        for (int j = 0; j < BRICK_COLS; j++) {
            Brick* brick = &bricks[i][j];
            if (!brick->destroyed &&
                ball.x + ball.size > brick->x &&
                ball.x < brick->x + BRICK_WIDTH &&
                ball.y + ball.size > brick->y &&
                ball.y < brick->y + BRICK_HEIGHT) {

                brick->destroyed = true;
                ball.dy = -ball.dy;

                stats.score += calculate_brick_score(stats.combo);
                stats.combo++;

                update_score_display();
                Mix_PlayChannel(-1, audio.brick_hit, 0);
                hit = true;
                break;
            }
        }
        if (hit) break;
    }
}

void handle_ball_loss() {
    if (ball.y > SCREEN_HEIGHT) {
        stats.lives--;
        update_lives_display();

        if (stats.lives <= 0) {
            game_state = GAME_STATE_GAME_OVER;
        } else {
            reset_ball();
            paddle.x = SCREEN_WIDTH / 2 - PADDLE_WIDTH / 2;
        }
    }
}

void handle_start_screen_events(SDL_Event* e) {
    if (e->type == SDL_QUIT) {
        running = false;
    } else if (e->type == SDL_KEYDOWN) {
        if (e->key.keysym.sym == SDLK_9) {
            reset_game();
        }
    } else if (e->type == SDL_JOYBUTTONDOWN) {
        if (e->jbutton.button == START_BUTTON) {
            reset_game();
        }
    }
}

void handle_end_screen_events(SDL_Event* e) {
    if (e->type == SDL_QUIT) {
        running = false;
    } else if (e->type == SDL_KEYDOWN) {
        switch (e->key.keysym.sym) {
            case SDLK_r:
                reset_game();
                break;
            case SDLK_q:
                running = false;
                break;
        }
    } else if (e->type == SDL_JOYBUTTONDOWN) {
        if (e->jbutton.button == START_BUTTON) {
            reset_game();
        } else if (e->jbutton.button == B_BUTTON) {
            running = false;
        }
    }
}

void render_start_screen() {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, startscreen_texture, NULL, NULL);

    int text_width, text_height;
    SDL_QueryTexture(start_text_texture, NULL, NULL, &text_width, &text_height);
    SDL_Rect text_rect = {
        (SCREEN_WIDTH - text_width) / 2,
        SCREEN_HEIGHT * 3/4,
        text_width,
        text_height
    };
    SDL_RenderCopy(renderer, start_text_texture, NULL, &text_rect);

    SDL_RenderPresent(renderer);
}

void render_end_screen(bool is_win) {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, is_win ? youwin_texture : gameover_texture, NULL, NULL);

    int text_width, text_height;

    SDL_QueryTexture(restart_text_texture, NULL, NULL, &text_width, &text_height);
    SDL_Rect restart_rect = {
        (SCREEN_WIDTH - text_width) / 2,
        SCREEN_HEIGHT * 3/4,
        text_width,
        text_height
    };
    SDL_RenderCopy(renderer, restart_text_texture, NULL, &restart_rect);

    SDL_QueryTexture(quit_text_texture, NULL, NULL, &text_width, &text_height);
    SDL_Rect quit_rect = {
        (SCREEN_WIDTH - text_width) / 2,
        SCREEN_HEIGHT * 3/4 + text_height + 10,
        text_width,
        text_height
    };
    SDL_RenderCopy(renderer, quit_text_texture, NULL, &quit_rect);

    SDL_RenderPresent(renderer);
}

void render_game_stats() {
    if (stats.score_text != NULL) {
        SDL_Rect score_rect = {10, 10, 0, 0};
        SDL_QueryTexture(stats.score_text, NULL, NULL, &score_rect.w, &score_rect.h);
        SDL_RenderCopy(renderer, stats.score_text, NULL, &score_rect);
    }

    if (stats.lives_text != NULL) {
        SDL_Rect lives_rect = {SCREEN_WIDTH - 150, 10, 0, 0};
        SDL_QueryTexture(stats.lives_text, NULL, NULL, &lives_rect.w, &lives_rect.h);
        SDL_RenderCopy(renderer, stats.lives_text, NULL, &lives_rect);
    }
}

void main_loop() {
    SDL_Event e;

    while (SDL_PollEvent(&e)) {
        switch (game_state) {
            case GAME_STATE_START_SCREEN:
                handle_start_screen_events(&e);
                break;

            case GAME_STATE_PLAYING:
                if (e.type == SDL_QUIT) {
                    running = false;
                }
                // Keyboard controls
                else if (e.type == SDL_KEYDOWN) {
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
                // Controller D-pad
                else if (e.type == SDL_JOYBUTTONDOWN) {
                    if (e.jbutton.button == LEFT_BUTTON) {
                        move_left = true;
                    } else if (e.jbutton.button == RIGHT_BUTTON) {
                        move_right = true;
                    }
                } else if (e.type == SDL_JOYBUTTONUP) {
                    if (e.jbutton.button == LEFT_BUTTON) {
                        move_left = false;
                    } else if (e.jbutton.button == RIGHT_BUTTON) {
                        move_right = false;
                    }
                }
                // Controller analog stick
                else if (e.type == SDL_JOYAXISMOTION) {
                    if (e.jaxis.axis == 0) {  // X axis
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
                }
                break;

            case GAME_STATE_WIN_SCREEN:
            case GAME_STATE_GAME_OVER:
                handle_end_screen_events(&e);
                break;
        }
    }

    switch (game_state) {
        case GAME_STATE_START_SCREEN:
            render_start_screen();
            return;
        case GAME_STATE_WIN_SCREEN:
            render_end_screen(true);
            return;
        case GAME_STATE_GAME_OVER:
            render_end_screen(false);
            return;
        default:
            break;
    }

    // Game logic
    if (game_state == GAME_STATE_PLAYING) {
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
        if (ball.y <= SCORE_HEIGHT) {
            ball.dy = -ball.dy;
        }

        // Handle collisions
        handle_paddle_collision();
        handle_brick_collisions();
        handle_ball_loss();

        // Check win condition
        if (check_all_bricks_destroyed()) {
            game_state = GAME_STATE_WIN_SCREEN;
            return;
        }

        // Render game
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

        // Draw stats
        render_game_stats();

        SDL_RenderPresent(renderer);
    }
}

int main(void) {
    // Seed random number generator
    srand(time(NULL));

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) != 0) {
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

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        printf("TTF_Init Error: %s\n", TTF_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Initialize audio
    if (!init_audio()) {
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Open joystick
    if (SDL_NumJoysticks() > 0) {
        joystick = SDL_JoystickOpen(0);
        if (joystick == NULL) {
            printf("Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError());
        } else {
            printf("Game controller connected: %s\n", SDL_JoystickName(joystick));
        }
    }

    // Open fonts
    font_menu = TTF_OpenFont("fonts/arial.ttf", MENU_FONT_SIZE);
    font_hud = TTF_OpenFont("fonts/arial.ttf", HUD_FONT_SIZE);
    if (font_menu == NULL || font_hud == NULL) {
        printf("TTF_OpenFont Error: %s\n", TTF_GetError());
        if (joystick != NULL) SDL_JoystickClose(joystick);
        cleanup_audio();
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Create window
    win = SDL_CreateWindow("BrickCrush",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (win == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        TTF_CloseFont(font_menu);
        TTF_CloseFont(font_hud);
        if (joystick != NULL) SDL_JoystickClose(joystick);
        cleanup_audio();
        TTF_Quit();
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
        TTF_CloseFont(font_menu);
        TTF_CloseFont(font_hud);
        if (joystick != NULL) SDL_JoystickClose(joystick);
        cleanup_audio();
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Initialize game objects and load textures
    if (!init_game_objects()) {
        cleanup_game_objects();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(win);
        TTF_CloseFont(font_menu);
        TTF_CloseFont(font_hud);
        if (joystick != NULL) SDL_JoystickClose(joystick);
        cleanup_audio();
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Start background music
    Mix_PlayMusic(audio.background_music, -1);

    // Main game loop
    while (running) {
        main_loop();
        SDL_Delay(16);  // Cap to roughly 60 FPS
    }

    // Cleanup everything
    cleanup_game_objects();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    TTF_CloseFont(font_menu);
    TTF_CloseFont(font_hud);
    if (joystick != NULL) SDL_JoystickClose(joystick);
    cleanup_audio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
