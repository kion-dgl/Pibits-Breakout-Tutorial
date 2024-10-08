#include "init.h"
#include "utils.h"  // Include utils.h for createShaderProgram
#include <stdio.h>

void initBall(Ball* ball, GLuint shaderProgram) {
    ball->radius = 15.0f;
    ball->x = 400.0f;   // Start in the middle of the window (800 / 2)
    ball->y = 240.0f;   // Start in the middle of the window (480 / 2)
    ball->vx = 2.0f;    // Set a horizontal velocity (arbitrary value)
    ball->vy = 1.5f;    // Set a vertical velocity (arbitrary value)

    // Hard-coded vertex data for a ball (you can adjust these values as needed)
    GLfloat vertices[] = {
        // Position (X, Y, Z)          // Texture coordinates (U, V)
        -ball->radius,  ball->radius, 0.0f,   0.0f, 1.0f,  // Top-left
        -ball->radius, -ball->radius, 0.0f,   0.0f, 0.0f,  // Bottom-left
         ball->radius, -ball->radius, 0.0f,   1.0f, 0.0f,  // Bottom-right

        -ball->radius,  ball->radius, 0.0f,   0.0f, 1.0f,  // Top-left
         ball->radius, -ball->radius, 0.0f,   1.0f, 0.0f,  // Bottom-right
         ball->radius,  ball->radius, 0.0f,   1.0f, 1.0f   // Top-right
    };

    // Generate and bind the VBO
    glGenBuffers(1, &ball->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, ball->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Hard-coded texture file path for the ball
    const char* textureFilePath = "ball.png";
    ball->textureID = loadTexture(textureFilePath);

    // Set up position attribute
    GLint positionAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(positionAttrib);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);

    // Set up texture coordinate attribute
    GLint texCoordAttrib = glGetAttribLocation(shaderProgram, "texCoord");
    glEnableVertexAttribArray(texCoordAttrib);
    glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
}


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
    *window = SDL_CreateWindow("Ball Bounce",
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
