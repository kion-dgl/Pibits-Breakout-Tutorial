#include "init.h"
#include "utils.h"  // Include utils.h for createShaderProgram
#include <stdio.h>

int initializeSDLAndOpenGL(SDL_Window** window, SDL_GLContext* glContext, GLuint* shaderProgram) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Set OpenGL ES 2.0
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

    // Create an 800x480 window
    *window = SDL_CreateWindow("Brickout - Pibit",
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               800, 480,
                               SDL_WINDOW_OPENGL);

    if (*window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create an OpenGL context
    *glContext = SDL_GL_CreateContext(*window);
    if (*glContext == NULL) {
        printf("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return 1;
    }

    // Initialize GLEW
    glewExperimental = GL_TRUE; // Ensure GLEW uses modern techniques
    if (glewInit() != GLEW_OK) {
        printf("Error initializing GLEW!\n");
        SDL_GL_DeleteContext(*glContext);
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return 1;
    }

    // Set the viewport
    glViewport(0, 0, 800, 480);

    // Create and use the shader program
    *shaderProgram = createShaderProgram();
    glUseProgram(*shaderProgram);

    // Create an orthographic projection matrix for 800x480 window
    float orthoMatrix[16];
    createOrthoProjectionMatrix(orthoMatrix, 800.0f, 480.0f);

    // Set the projection matrix uniform in the shader
    GLint projectionUniform = glGetUniformLocation(*shaderProgram, "projection");
    glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, orthoMatrix);

    // Clear the screen
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f); // Set a background color

    return 0;  // Return success
}
