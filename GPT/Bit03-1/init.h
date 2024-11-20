#ifndef INIT_H
#define INIT_H

#include <SDL2/SDL.h>
#include <GL/glew.h>

// Function declarations for initialization
int initializeSDLAndOpenGL(SDL_Window** window, SDL_GLContext* glContext, GLuint* shaderProgram);

#endif // INIT_H
