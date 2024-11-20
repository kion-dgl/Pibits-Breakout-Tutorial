#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>  // For rand()
#include <time.h>    // For seeding random number generator
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

// Function to check collision between ball and brick
int checkBrickCollision(Ball ball, Brick brick) {
    // If the brick is not active, skip collision check
    if (!brick.isActive) return 0;

    // Simple AABB collision detection
    return ball.x < brick.x + brick.width &&
           ball.x + ball.radius > brick.x &&
           ball.y < brick.y + brick.height &&
           ball.y + ball.radius > brick.y;
}

// Function to handle ball and brick collision
void handleBallBrickCollision(Ball* ball, Brick* brick) {
    if (checkBrickCollision(*ball, *brick)) {
        // Reverse the ball's vertical velocity
        ball->vy = -ball->vy;
        brick->health--;

        // If the brick has full health (health == 3), there's a 25% chance of double break
        if (brick->health == 2) {
            int randomChance = rand() % 100;  // Generate a number between 0 and 99

            if (randomChance < 25) {
                // 75% chance: Set brick's health to 0, break it instantly
                brick->health--;
            }
        } else if (brick->health == 1) {
            int randomChance = rand() % 100;  // Generate a number between 0 and 99

            if (randomChance < 75) {
                // 75% chance: Set brick's health to 0, break it instantly
                brick->health--;
            }
        }

        // Update brick's state based on health
        if (brick->health <= 0) {
            brick->isActive = 0;  // Deactivate the brick if health is 0
        }
    }
}

int checkCollision (Ball ball, Paddle paddle) {
    return ball.x < paddle.x + paddle.width &&
            ball.x + ball.radius > paddle.x &&
            ball.y < paddle.y + paddle.height &&
            ball.y + ball.radius > paddle.y;
}

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_GLContext glContext;
    GLuint shaderProgram;

    srand(time(NULL));  // Seed random number generator

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


    const int rows = 6;
    const int cols = 10; // Number of bricks per row
    const float brickWidth = 70.0f;  // Assuming window width of 800
    const float brickHeight = 34.0f;
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
    Uint32 previousTime = 0;
    Uint32 currentTime = 0;
    float deltaTime = 0.0f;
    float modelMatrix[16];
    GLint modelUniform = glGetUniformLocation(shaderProgram, "model");

    // Main loop
    while (running) {
        // Calculate delta time
        currentTime = SDL_GetTicks();
        deltaTime = (currentTime - previousTime) / 10.0f;
        previousTime = currentTime;

        // Event handling
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT);

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

        // Move the paddle based on user input
        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_LEFT]) {
            paddle.x -= paddle.speed * deltaTime;  // Move left
        }
        if (state[SDL_SCANCODE_RIGHT]) {
            paddle.x += paddle.speed * deltaTime;  // Move right
        }

        // Keep the paddle within the window bounds
        if (paddle.x - paddle.width / 2 < 0) {
            paddle.x = paddle.width / 2;
        }
        if (paddle.x + paddle.width / 2 > 800) {
            paddle.x = 800 - paddle.width / 2;
        }

        if (checkCollision(ball, paddle)) {
            ball.vy = -ball.vy;
        }

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
            // Check ball-brick collision and deactivate the brick if hit
            handleBallBrickCollision(&ball, &bricks[i]);

            if (!bricks[i].isActive) continue;

            // Create and pass model matrix for each brick
            createTranslationMatrix(modelMatrix, bricks[i].x, bricks[i].y);
            glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);

            // Bind brick texture and draw
            if ( bricks[i].health == 3) {
                glBindTexture(GL_TEXTURE_2D,  bricks[i].textureID);  // Full health texture
            } else if ( bricks[i].health == 2) {
                glBindTexture(GL_TEXTURE_2D,  bricks[i].crackedTexture); // First hit texture
            } else if ( bricks[i].health == 1) {
                glBindTexture(GL_TEXTURE_2D,  bricks[i].moreCrackedTexture); // Second hit texture
            }
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
