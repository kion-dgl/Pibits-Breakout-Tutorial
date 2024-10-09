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

void drawBrick(Brick brick, GLuint shaderProgram, GLint modelUniform) {
    if (!brick.isActive) return; // Only draw active bricks

    // Create the translation matrix for the brick
    float modelMatrix[16];
    createTranslationMatrix(modelMatrix, brick.x, brick.y);

    // Pass the model matrix to the shader
    glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);

    // Bind the brick texture
    glBindTexture(GL_TEXTURE_2D, brick.textureID);

    // Draw the brick
    glDrawArrays(GL_TRIANGLES, 0, 6);
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
    // Initialize ball (vertices, VBO, and texture)
    Ball ball;
    initBall(&ball, shaderProgram);

    // Bind ball VBO and set up vertex attributes
    glBindBuffer(GL_ARRAY_BUFFER, ball.VBO);

    // Set up position attribute for ball
    GLint positionAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(positionAttrib);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);

    // Set up texture coordinate attribute for ball
    GLint texCoordAttrib = glGetAttribLocation(shaderProgram, "texCoord");
    glEnableVertexAttribArray(texCoordAttrib);
    glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    // Now do the same for the paddle
    Paddle paddle;
    initPaddle(&paddle, shaderProgram);

    // Bind paddle VBO and set up vertex attributes
    glBindBuffer(GL_ARRAY_BUFFER, paddle.VBO);

    // Set up position attribute for paddle
    glEnableVertexAttribArray(positionAttrib);  // You can reuse the positionAttrib location
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);

    // Set up texture coordinate attribute for paddle
    glEnableVertexAttribArray(texCoordAttrib);  // You can reuse the texCoordAttrib location
    glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));


    const int rows = 1;
    const int cols = 4; // Number of bricks per row
    const float brickWidth = 45.0f;  // Assuming window width of 800
    const float brickHeight = 20.0f;
    Brick bricks[rows * cols];



    const float halfBrickWidth = brickWidth / 2.0f;
    const float halfBrickHeight = brickHeight / 2.0f;

    float brickVertices[] = {
        // First triangle
        -halfBrickWidth, -halfBrickHeight, 0.0f,  0.0f, 0.0f,  // Bottom-left
         halfBrickWidth, -halfBrickHeight, 0.0f,  1.0f, 0.0f,  // Bottom-right
         halfBrickWidth,  halfBrickHeight, 0.0f,  1.0f, 1.0f,  // Top-right

        // Second triangle
        -halfBrickWidth, -halfBrickHeight, 0.0f,  0.0f, 0.0f,  // Bottom-left
         halfBrickWidth,  halfBrickHeight, 0.0f,  1.0f, 1.0f,  // Top-right
        -halfBrickWidth,  halfBrickHeight, 0.0f,  0.0f, 1.0f   // Top-left
    };


    GLuint brickVBO;
    glGenBuffers(1, &brickVBO);
    glBindBuffer(GL_ARRAY_BUFFER, brickVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(brickVertices), brickVertices, GL_STATIC_DRAW);


    // Bind paddle VBO and set up vertex attributes
    glBindBuffer(GL_ARRAY_BUFFER, brickVBO);

    // Set up position attribute for paddle
    glEnableVertexAttribArray(positionAttrib);  // You can reuse the positionAttrib location
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);

    // Set up texture coordinate attribute for paddle
    glEnableVertexAttribArray(texCoordAttrib);  // You can reuse the texCoordAttrib location
    glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
   initBricks(bricks, cols, rows, brickWidth, brickHeight );

    // Main loop
    int running = 1;
    SDL_Event event;
    // Uint32 previousTime = 0;
    // Uint32 currentTime = 0;
    // float deltaTime = 0.0f;
    float modelMatrix[16];
    GLint modelUniform = glGetUniformLocation(shaderProgram, "model");

    // Main loop
    while (running) {
        // Calculate delta time
        // currentTime = SDL_GetTicks();
        // deltaTime = (currentTime - previousTime) / 10.0f;
        // previousTime = currentTime;

        // Event handling
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT);

        // 1. Render Ball
        createTranslationMatrix(modelMatrix, ball.x, ball.y);
        glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);

        // Bind ball VBO and set up vertex attributes for ball
        glBindBuffer(GL_ARRAY_BUFFER, ball.VBO);
        glEnableVertexAttribArray(positionAttrib);
        glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(texCoordAttrib);
        glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

        // Draw the ball
        glBindTexture(GL_TEXTURE_2D, ball.textureID);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Disable vertex attributes for the ball
        glDisableVertexAttribArray(positionAttrib);
        glDisableVertexAttribArray(texCoordAttrib);

        // 2. Render Paddle
        createTranslationMatrix(modelMatrix, paddle.x, paddle.y);
        glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);

        // Bind paddle VBO and set up vertex attributes for paddle
        glBindBuffer(GL_ARRAY_BUFFER, paddle.VBO);
        glEnableVertexAttribArray(positionAttrib);
        glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(texCoordAttrib);
        glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

        // Draw the paddle
        glBindTexture(GL_TEXTURE_2D, paddle.textureID);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Disable vertex attributes for the paddle
        glDisableVertexAttribArray(positionAttrib);
        glDisableVertexAttribArray(texCoordAttrib);

        // 3. Render Bricks
        glBindBuffer(GL_ARRAY_BUFFER, brickVBO);
        glEnableVertexAttribArray(positionAttrib);
        glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(texCoordAttrib);
        glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

        for (int i = 0; i < rows * cols; i++) {
            if (!bricks[i].isActive) continue;

            // Create and pass model matrix for each brick
            createTranslationMatrix(modelMatrix, bricks[i].x, bricks[i].y);
            glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);

            // Bind brick texture and draw
            glBindTexture(GL_TEXTURE_2D, bricks[i].textureID);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        // Disable vertex attributes for bricks
        glDisableVertexAttribArray(positionAttrib);
        glDisableVertexAttribArray(texCoordAttrib);

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
