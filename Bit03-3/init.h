#ifndef INIT_H
#define INIT_H

#include <SDL2/SDL.h>
#include <GL/glew.h>

// Function declarations for initialization

typedef struct {
    GLuint VBO;         // Vertex Buffer Object
    GLuint textureID;   // Texture ID
    float radius;       // Radius of the ball
} Ball;


int initializeSDLAndOpenGL(SDL_Window** window, SDL_GLContext* glContext, GLuint* shaderProgram);
void initBall(Ball* ball, GLuint shaderProgram);

#endif // INIT_H
