#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include <stdbool.h>
#include <math.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 480
#define BRICK_WIDTH 80
#define BRICK_HEIGHT 20
#define NUM_BRICK_COLUMNS 10
#define NUM_BRICK_ROWS 5
#define PADDLE_WIDTH 100
#define PADDLE_HEIGHT 20
#define BALL_SIZE 15
#define BALL_SPEED 5.0f

typedef struct {
    float x, y;
    float width, height;
    bool active;
} GameObject;

typedef struct {
    float x, y;
    float dx, dy;
    float size;
} Ball;

// Global variables
SDL_Window* window = NULL;
SDL_GLContext glContext;
GLuint backgroundTexture;
GLuint brickTexture;
GameObject paddle;
GameObject bricks[NUM_BRICK_ROWS][NUM_BRICK_COLUMNS];
Ball ball;
bool gameRunning = true;

// Function prototypes
bool initSDL(void);
void cleanup(void);
GLuint loadTexture(const char* filename);
void handleInput(void);
void updateGame(void);
void renderGame(void);
void initializeGame(void);
bool checkCollision(GameObject* a, float x, float y, float size);

bool initSDL(void) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return false;
    }

    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    window = SDL_CreateWindow("Breakout",
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            WINDOW_WIDTH, WINDOW_HEIGHT,
                            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    
    if (!window) {
        return false;
    }

    glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        return false;
    }

    // Initialize OpenGL
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}

GLuint loadTexture(const char* filename) {
    SDL_Surface* surface = IMG_Load(filename);
    if (!surface) {
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int mode = GL_RGB;
    if (surface->format->BytesPerPixel == 4) {
        mode = GL_RGBA;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, mode, surface->w, surface->h, 0, mode, GL_UNSIGNED_BYTE, surface->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    SDL_FreeSurface(surface);
    return textureID;
}

void initializeGame(void) {
    // Initialize paddle
    paddle.x = WINDOW_WIDTH / 2 - PADDLE_WIDTH / 2;
    paddle.y = WINDOW_HEIGHT - 40;
    paddle.width = PADDLE_WIDTH;
    paddle.height = PADDLE_HEIGHT;

    // Initialize ball
    ball.x = WINDOW_WIDTH / 2;
    ball.y = WINDOW_HEIGHT / 2;
    ball.dx = BALL_SPEED;
    ball.dy = -BALL_SPEED;
    ball.size = BALL_SIZE;

    // Initialize bricks
    for (int row = 0; row < NUM_BRICK_ROWS; row++) {
        for (int col = 0; col < NUM_BRICK_COLUMNS; col++) {
            bricks[row][col].x = col * BRICK_WIDTH;
            bricks[row][col].y = row * BRICK_HEIGHT + 50;
            bricks[row][col].width = BRICK_WIDTH;
            bricks[row][col].height = BRICK_HEIGHT;
            bricks[row][col].active = true;
        }
    }
}

void handleInput(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            gameRunning = false;
        }
    }

    const Uint8* state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_LEFT] && paddle.x > 0) {
        paddle.x -= 5;
    }
    if (state[SDL_SCANCODE_RIGHT] && paddle.x < WINDOW_WIDTH - paddle.width) {
        paddle.x += 5;
    }
}

bool checkCollision(GameObject* obj, float x, float y, float size) {
    return (x + size > obj->x &&
            x - size < obj->x + obj->width &&
            y + size > obj->y &&
            y - size < obj->y + obj->height);
}

void updateGame(void) {
    // Update ball position
    ball.x += ball.dx;
    ball.y += ball.dy;

    // Ball collision with walls
    if (ball.x - ball.size < 0 || ball.x + ball.size > WINDOW_WIDTH) {
        ball.dx = -ball.dx;
    }
    if (ball.y - ball.size < 0) {
        ball.dy = -ball.dy;
    }
    if (ball.y + ball.size > WINDOW_HEIGHT) {
        // Reset ball position if it goes below paddle
        ball.x = WINDOW_WIDTH / 2;
        ball.y = WINDOW_HEIGHT / 2;
        ball.dx = BALL_SPEED;
        ball.dy = -BALL_SPEED;
    }

    // Ball collision with paddle
    if (checkCollision(&paddle, ball.x, ball.y, ball.size)) {
        ball.dy = -ball.dy;
        // Add some horizontal variation based on where the ball hits the paddle
        float relativeIntersectX = (paddle.x + (paddle.width / 2)) - ball.x;
        float normalizedIntersect = relativeIntersectX / (paddle.width / 2);
        ball.dx = -normalizedIntersect * BALL_SPEED;
    }

    // Ball collision with bricks
    for (int row = 0; row < NUM_BRICK_ROWS; row++) {
        for (int col = 0; col < NUM_BRICK_COLUMNS; col++) {
            if (bricks[row][col].active && 
                checkCollision(&bricks[row][col], ball.x, ball.y, ball.size)) {
                bricks[row][col].active = false;
                ball.dy = -ball.dy;
            }
        }
    }
}

void renderGame(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // Render background
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(0, 0);
    glTexCoord2f(1, 0); glVertex2f(WINDOW_WIDTH, 0);
    glTexCoord2f(1, 1); glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT);
    glTexCoord2f(0, 1); glVertex2f(0, WINDOW_HEIGHT);
    glEnd();

    // Render bricks
    glBindTexture(GL_TEXTURE_2D, brickTexture);
    for (int row = 0; row < NUM_BRICK_ROWS; row++) {
        for (int col = 0; col < NUM_BRICK_COLUMNS; col++) {
            if (bricks[row][col].active) {
                glBegin(GL_QUADS);
                glTexCoord2f(0, 0); glVertex2f(bricks[row][col].x, bricks[row][col].y);
                glTexCoord2f(1, 0); glVertex2f(bricks[row][col].x + BRICK_WIDTH, bricks[row][col].y);
                glTexCoord2f(1, 1); glVertex2f(bricks[row][col].x + BRICK_WIDTH, bricks[row][col].y + BRICK_HEIGHT);
                glTexCoord2f(0, 1); glVertex2f(bricks[row][col].x, bricks[row][col].y + BRICK_HEIGHT);
                glEnd();
            }
        }
    }

    // Render paddle
    glColor3f(0.2f, 0.5f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(paddle.x, paddle.y);
    glVertex2f(paddle.x + paddle.width, paddle.y);
    glVertex2f(paddle.x + paddle.width, paddle.y + paddle.height);
    glVertex2f(paddle.x, paddle.y + paddle.height);
    glEnd();

    // Render ball
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(ball.x - ball.size, ball.y - ball.size);
    glVertex2f(ball.x + ball.size, ball.y - ball.size);
    glVertex2f(ball.x + ball.size, ball.y + ball.size);
    glVertex2f(ball.x - ball.size, ball.y + ball.size);
    glEnd();

    SDL_GL_SwapWindow(window);
}

void cleanup(void) {
    glDeleteTextures(1, &backgroundTexture);
    glDeleteTextures(1, &brickTexture);
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    if (!initSDL()) {
        return 1;
    }

    // Load textures
    backgroundTexture = loadTexture("background.png");
    brickTexture = loadTexture("brick.png");
    
    if (!backgroundTexture || !brickTexture) {
        cleanup();
        return 1;
    }

    initializeGame();

    while (gameRunning) {
        handleInput();
        updateGame();
        renderGame();
        SDL_Delay(16); // Cap at roughly 60 FPS
    }

    cleanup();
    return 0;
}
