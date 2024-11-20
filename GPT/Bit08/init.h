#ifndef INIT_H
#define INIT_H

#include <SDL2/SDL.h>
#include <GL/glew.h>

// Function declarations for initialization

typedef struct {
    GLuint VBO;         // Vertex Buffer Object
    GLuint textureID;   // Texture ID
    float radius;       // Radius of the ball
    float x, y;         // Position of the ball
    float vx, vy;       // Velocity of the ball
} Ball;

typedef struct {
    GLuint VBO;         // Vertex Buffer Object
    GLuint textureID;   // Texture ID (if you want to use a texture, otherwise leave it empty)
    float width;        // Width of the paddle
    float height;       // Height of the paddle
    float x, y;         // Paddle position
    float speed;        // Paddle movement speed
} Paddle;

typedef struct {
    float x, y;        // Position of the brick
    float width, height;  // Size of the brick
    int isActive;      // Whether the brick is still active (not hit)
    GLuint textureID;  // Texture ID for the brick
} Brick;

void initPaddle(Paddle* paddle, GLuint shaderProgram);
int initializeSDLAndOpenGL(SDL_Window** window, SDL_GLContext* glContext, GLuint* shaderProgram);
void initBall(Ball* ball, GLuint shaderProgram);
void initBricks(Brick* bricks, int cols, int rows, float brickWidth, float brickHeight);

#endif // INIT_H
