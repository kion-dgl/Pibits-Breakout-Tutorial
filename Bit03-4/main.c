#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>
#include <stdio.h>
#include "init.h"  // Include init.h for initialization

void createTranslationMatrix(float* matrix, float x, float y) {
    matrix[0] = 1.0f;  matrix[1] = 0.0f;  matrix[2] = 0.0f;  matrix[3] = 0.0f;
    matrix[4] = 0.0f;  matrix[5] = 1.0f;  matrix[6] = 0.0f;  matrix[7] = 0.0f;
    matrix[8] = 0.0f;  matrix[9] = 0.0f;  matrix[10] = 1.0f; matrix[11] = 0.0f;
    matrix[12] = x;    matrix[13] = y;    matrix[14] = 0.0f; matrix[15] = 1.0f;
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
    Ball ball;
    initBall(&ball, shaderProgram);

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

        // Inside the main loop
        float modelMatrix[16];
        createTranslationMatrix(modelMatrix, ball.x, ball.y);

        // Get the location of the model matrix uniform in the shader
        GLint modelUniform = glGetUniformLocation(shaderProgram, "model");

        // Pass the model matrix to the shader
        glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);

        // Inside the main loop, before drawing
        glBindTexture(GL_TEXTURE_2D, ball.textureID);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Swap the buffers (double buffering)
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    glDeleteBuffers(1, &ball.VBO);
    glDeleteTextures(1, &ball.textureID);
    glDeleteProgram(shaderProgram);
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
