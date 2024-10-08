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

    Paddle paddle;
    initPaddle(&paddle, shaderProgram);

    // Main loop
    int running = 1;
    SDL_Event event;
    Uint32 previousTime = 0;
    Uint32 currentTime = 0;
    float deltaTime = 0.0f;

    while (running) {
        // Calculate delta time
        currentTime = SDL_GetTicks();  // Get current time in milliseconds
        deltaTime = (currentTime - previousTime) / 10.0f;
        previousTime = currentTime;

        // Event handling
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        // Update ball position based on velocity
        ball.x += ball.vx * deltaTime;
        ball.y += ball.vy * deltaTime;

        // Collision with window edges (bounce back)
        if (ball.x + ball.radius > 800 || ball.x - ball.radius < 0) {
            ball.vx = -ball.vx;  // Reverse horizontal direction
        }
        if (ball.y + ball.radius > 480 || ball.y - ball.radius < 0) {
            ball.vy = -ball.vy;  // Reverse vertical direction
        }

        glClear(GL_COLOR_BUFFER_BIT);

        // Inside the main loop
        float modelMatrix[16];
        createTranslationMatrix(modelMatrix, ball.x, ball.y);
        GLint modelUniform = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);

        glBindBuffer(GL_ARRAY_BUFFER, ball.VBO);  // Bind the ball's VBO
        glBindTexture(GL_TEXTURE_2D, ball.textureID);  // Bind the ball's texture
        glDrawArrays(GL_TRIANGLES, 0, 6);  // Draw the ball

        // Update and draw the paddle
        createTranslationMatrix(modelMatrix, paddle.x, paddle.y);
        glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);

        glBindBuffer(GL_ARRAY_BUFFER, paddle.VBO);  // Bind the paddle's VBO
        glBindTexture(GL_TEXTURE_2D, paddle.textureID);  // Bind the paddle's texture (optional)
        glDrawArrays(GL_TRIANGLES, 0, 6);  // Draw the paddle

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
