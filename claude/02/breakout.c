#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>

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
    GLuint texture;  // Added texture field
} GameObject;

typedef struct {
    float x, y;
    float dx, dy;
    float size;
    GLuint texture;  // Added texture field
} Ball;

SDL_Window* window = NULL;
SDL_GLContext glContext;
GLuint backgroundTexture;
GameObject paddle;
GameObject bricks[NUM_BRICK_ROWS][NUM_BRICK_COLUMNS];
Ball ball;
bool gameRunning = true;

// [Previous includes and definitions remain the same...]

// Add these function declarations at the top of the file, after the global variables
void handleInput(void);
void updateGame(void);
bool checkCollision(GameObject* obj, float x, float y, float size);

// Add these function implementations before the main() function

void handleInput(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            gameRunning = false;
        }
        else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    gameRunning = false;
                    break;
            }
        }
    }

    // Handle continuous keyboard state
    const Uint8* keyState = SDL_GetKeyboardState(NULL);
    float paddleSpeed = 7.0f;

    if (keyState[SDL_SCANCODE_LEFT] || keyState[SDL_SCANCODE_A]) {
        paddle.x -= paddleSpeed;
        if (paddle.x < 0) paddle.x = 0;
    }
    if (keyState[SDL_SCANCODE_RIGHT] || keyState[SDL_SCANCODE_D]) {
        paddle.x += paddleSpeed;
        if (paddle.x > WINDOW_WIDTH - paddle.width) {
            paddle.x = WINDOW_WIDTH - paddle.width;
        }
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
    if (ball.x - ball.size < 0) {
        ball.x = ball.size;
        ball.dx = fabs(ball.dx);  // Ensure ball moves right
    }
    if (ball.x + ball.size > WINDOW_WIDTH) {
        ball.x = WINDOW_WIDTH - ball.size;
        ball.dx = -fabs(ball.dx);  // Ensure ball moves left
    }
    if (ball.y - ball.size < 0) {
        ball.y = ball.size;
        ball.dy = fabs(ball.dy);  // Ensure ball moves down
    }

    // Ball out of bounds (bottom)
    if (ball.y + ball.size > WINDOW_HEIGHT) {
        // Reset ball position
        ball.x = WINDOW_WIDTH / 2;
        ball.y = WINDOW_HEIGHT / 2;
        ball.dx = BALL_SPEED;
        ball.dy = -BALL_SPEED;
    }

    // Ball collision with paddle
    if (checkCollision(&paddle, ball.x, ball.y, ball.size)) {
        // Calculate where on the paddle the ball hit
        float relativeIntersectX = (paddle.x + (paddle.width / 2)) - ball.x;
        float normalizedIntersect = relativeIntersectX / (paddle.width / 2);
        
        // Calculate new angle (between -60 and 60 degrees)
        float maxAngle = 60.0f * M_PI / 180.0f; // Convert to radians
        float angle = normalizedIntersect * maxAngle;
        
        // Update ball velocity
        float speed = sqrt(ball.dx * ball.dx + ball.dy * ball.dy);
        ball.dx = speed * -sin(angle);
        ball.dy = -fabs(speed * cos(angle)); // Ensure ball goes up
        
        // Move ball out of paddle to prevent multiple collisions
        ball.y = paddle.y - ball.size;
    }

    // Ball collision with bricks
    for (int row = 0; row < NUM_BRICK_ROWS; row++) {
        for (int col = 0; col < NUM_BRICK_COLUMNS; col++) {
            if (bricks[row][col].active && 
                checkCollision(&bricks[row][col], ball.x, ball.y, ball.size)) {
                bricks[row][col].active = false;
                
                // Determine which side of the brick was hit
                float brickCenterX = bricks[row][col].x + bricks[row][col].width / 2;
                float brickCenterY = bricks[row][col].y + bricks[row][col].height / 2;
                
                // Calculate difference from center of brick to center of ball
                float dx = ball.x - brickCenterX;
                float dy = ball.y - brickCenterY;
                
                // Compare the absolute values to determine which side was hit
                if (fabs(dx) * bricks[row][col].height > fabs(dy) * bricks[row][col].width) {
                    // Hit left or right side
                    ball.dx = -ball.dx;
                } else {
                    // Hit top or bottom
                    ball.dy = -ball.dy;
                }
                
                break; // Exit after first collision
            }
        }
    }
}

// [Rest of the code remains the same...]

// Texture loading function
GLuint loadTexture(const char* filename) {
    printf("Loading texture: %s\n", filename);
    
    SDL_Surface* surface = IMG_Load(filename);
    if (!surface) {
        printf("Failed to load texture %s: %s\n", filename, IMG_GetError());
        return 0;
    }
    printf("Texture loaded successfully: %s (%dx%d)\n", filename, surface->w, surface->h);

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int mode = GL_RGB;
    if (surface->format->BytesPerPixel == 4) {
        mode = GL_RGBA;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, mode, surface->w, surface->h, 0, mode, GL_UNSIGNED_BYTE, surface->pixels);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        printf("OpenGL error while creating texture: %d\n", error);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    SDL_FreeSurface(surface);
    return textureID;
}

bool initSDL(void) {
    printf("Initializing SDL...\n");
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return false;
    }
    printf("SDL initialized successfully\n");

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("SDL_image initialization failed: %s\n", IMG_GetError());
        return false;
    }
    printf("SDL_image initialized successfully\n");

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    window = SDL_CreateWindow("Breakout",
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            WINDOW_WIDTH, WINDOW_HEIGHT,
                            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    
    if (!window) {
        printf("Window creation failed: %s\n", SDL_GetError());
        return false;
    }

    glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        printf("OpenGL context creation failed: %s\n", SDL_GetError());
        return false;
    }

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

void initializeGame(void) {
    printf("Initializing game objects...\n");
    
    // Initialize paddle
    paddle.x = WINDOW_WIDTH / 2 - PADDLE_WIDTH / 2;
    paddle.y = WINDOW_HEIGHT - 40;
    paddle.width = PADDLE_WIDTH;
    paddle.height = PADDLE_HEIGHT;
    paddle.texture = loadTexture("paddle.png");

    // Initialize ball
    ball.x = WINDOW_WIDTH / 2;
    ball.y = WINDOW_HEIGHT / 2;
    ball.dx = BALL_SPEED;
    ball.dy = -BALL_SPEED;
    ball.size = BALL_SIZE;
    ball.texture = loadTexture("ball.png");

    // Initialize bricks
    GLuint brickTexture = loadTexture("brick.png");
    for (int row = 0; row < NUM_BRICK_ROWS; row++) {
        for (int col = 0; col < NUM_BRICK_COLUMNS; col++) {
            bricks[row][col].x = col * BRICK_WIDTH;
            bricks[row][col].y = row * BRICK_HEIGHT + 50;
            bricks[row][col].width = BRICK_WIDTH;
            bricks[row][col].height = BRICK_HEIGHT;
            bricks[row][col].active = true;
            bricks[row][col].texture = brickTexture;
        }
    }
}

void renderTexturedQuad(float x, float y, float width, float height, GLuint texture) {
    glBindTexture(GL_TEXTURE_2D, texture);
    glBegin(GL_QUADS);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);  // Reset color to white
    glTexCoord2f(0, 0); glVertex2f(x, y);
    glTexCoord2f(1, 0); glVertex2f(x + width, y);
    glTexCoord2f(1, 1); glVertex2f(x + width, y + height);
    glTexCoord2f(0, 1); glVertex2f(x, y + height);
    glEnd();
}

void renderGame(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // Render background
    renderTexturedQuad(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, backgroundTexture);

    // Render bricks
    for (int row = 0; row < NUM_BRICK_ROWS; row++) {
        for (int col = 0; col < NUM_BRICK_COLUMNS; col++) {
            if (bricks[row][col].active) {
                renderTexturedQuad(bricks[row][col].x, bricks[row][col].y,
                                 bricks[row][col].width, bricks[row][col].height,
                                 bricks[row][col].texture);
            }
        }
    }

    // Render paddle
    renderTexturedQuad(paddle.x, paddle.y, paddle.width, paddle.height, paddle.texture);

    // Render ball
    renderTexturedQuad(ball.x - ball.size, ball.y - ball.size,
                      ball.size * 2, ball.size * 2, ball.texture);

    SDL_GL_SwapWindow(window);
}

// [Previous handleInput, updateGame, and checkCollision functions remain the same]

void cleanup(void) {
    printf("Cleaning up resources...\n");
    
    // Delete all textures
    glDeleteTextures(1, &backgroundTexture);
    glDeleteTextures(1, &paddle.texture);
    glDeleteTextures(1, &ball.texture);
    if (NUM_BRICK_ROWS > 0 && NUM_BRICK_COLUMNS > 0) {
        glDeleteTextures(1, &bricks[0][0].texture); // Only need to delete brick texture once
    }
    
    if (glContext) {
        SDL_GL_DeleteContext(glContext);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    IMG_Quit();
    SDL_Quit();
    printf("Cleanup complete\n");
}

int main(int argc, char* argv[]) {
    printf("Starting Breakout game...\n");

    if (!initSDL()) {
        printf("Failed to initialize SDL. Exiting...\n");
        cleanup();
        return 1;
    }

    printf("Loading background texture...\n");
    backgroundTexture = loadTexture("background.png");
    if (!backgroundTexture) {
        printf("Failed to load background texture. Exiting...\n");
        cleanup();
        return 1;
    }

    initializeGame();
    printf("Game initialized, entering main loop...\n");

    while (gameRunning) {
        handleInput();
        updateGame();
        renderGame();
        SDL_Delay(16);
    }

    printf("Game loop ended, cleaning up...\n");
    cleanup();
    return 0;
}
