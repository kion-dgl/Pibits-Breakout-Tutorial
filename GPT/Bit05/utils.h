#ifndef UTILS_H
#define UTILS_H

#include <SDL2/SDL.h>
#include <GL/glew.h>

// Function declarations
void createOrthoProjectionMatrix(float* matrix, float width, float height);
GLuint loadTexture(const char* filePath);
GLuint compileShader(GLenum type, const GLchar* source);
GLuint createShaderProgram();

#endif // UTILS_H
