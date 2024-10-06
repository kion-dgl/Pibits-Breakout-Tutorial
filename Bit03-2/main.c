#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>
#include <stdio.h>
#include "init.h"  // Include init.h for initialization
#include "utils.h"  // Include utility functions like loadTexture


void initBall(GLuint* VBO, GLuint* textureID, GLuint shaderProgram, const char* textureFilePath) {

    // Updated vertex data (with texture coordinates)
    GLfloat vertices[] = {
        // Position (X, Y, Z)   // Texture coordinates (U, V)
        0.0f, 50.0f, 0.0f,  0.0f, 1.0f,  // Top-left
        0.0f, 0.0f, 0.0f,  0.0f, 0.0f,  // Bottom-left
        50.0f, 0.0f, 0.0f,  1.0f, 0.0f,  // Bottom-right

        0.0f, 50.0f, 0.0f,  0.0f, 1.0f,  // Top-left
        50.0f, 0.0f, 0.0f,  1.0f, 0.0f,  // Bottom-right
        50.0f, 50.0f, 0.0f,  1.0f, 1.0f   // Top-right
    };

    // Generate and bind the VBO
    glGenBuffers(1, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Load the texture for the ball
    *textureID = loadTexture(textureFilePath);

    // Set up position attribute
    GLint positionAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(positionAttrib);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);

    // Set up texture coordinate attribute
    GLint texCoordAttrib = glGetAttribLocation(shaderProgram, "texCoord");
    glEnableVertexAttribArray(texCoordAttrib);
    glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
}

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_GLContext glContext;
    GLuint shaderProgram;

    // Initialize SDL and OpenGL
    if (initializeSDLAndOpenGL(&window, &glContext, &shaderProgram) != 0) {
        return 1;  // Exit if initialization failed
    }

    // Initialize ball (vertices, VBO, and texture)
    GLuint VBO;
    GLuint ballTexture;
    initBall(&VBO, &ballTexture, shaderProgram, "ball.png");

    // Main loop
    int running = 1;
    SDL_Event event;
    while (running) {
        // Event handling
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT);

        // Inside the main loop, before drawing
        glBindTexture(GL_TEXTURE_2D, ballTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Swap the buffers (double buffering)
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
