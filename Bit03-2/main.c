#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>
#include <stdio.h>
#include "init.h"  // Include init.h for initialization
#include "utils.h"  // Include utility functions like loadTexture

typedef struct {
    float x, y;          // Ball position
    float radius;        // Ball radius (or half-size for square)
    float velocityX, velocityY; // Ball velocity
    GLuint textureID;     // OpenGL texture ID
} Ball;

// Updated vertex data (with texture coordinates)
GLfloat vertices[] = {
    // Position (X, Y, Z)   // Texture coordinates (U, V)
    -15.0f, 15.0f, 0.0f,  0.0f, 1.0f,  // Top-left
    -15.0f, -15.0f, 0.0f,  0.0f, 0.0f,  // Bottom-left
    15.0f, -15.0f, 0.0f,  1.0f, 0.0f,  // Bottom-right

    -15.0f, 15.0f, 0.0f,  0.0f, 1.0f,  // Top-left
    15.0f, -15.0f, 0.0f,  1.0f, 0.0f,  // Bottom-right
    15.0f, 15.0f, 0.0f,  1.0f, 1.0f   // Top-right
};

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_GLContext glContext;
    GLuint shaderProgram;

    // Initialize SDL and OpenGL
    if (initializeSDLAndOpenGL(&window, &glContext, &shaderProgram) != 0) {
        return 1;  // Exit if initialization failed
    }

    // Create a vertex buffer object (VBO) and upload the vertex data
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Get the location of the "position" attribute in the vertex shader
    GLint positionAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(positionAttrib);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);

    // Enable the position attribute
    glEnableVertexAttribArray(positionAttrib);

    // Get the location of the "texCoord" attribute in the vertex shader
    GLint texCoordAttrib = glGetAttribLocation(shaderProgram, "texCoord");
    glEnableVertexAttribArray(texCoordAttrib);
    glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))); // Texture coordinates

    GLuint ballTexture = loadTexture("ball.png"); // Load your PNG file

    // Inside the main loop, before drawing:
    glBindTexture(GL_TEXTURE_2D, ballTexture);

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

        // Clear the screen
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f); // Set a background color
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
