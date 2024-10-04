#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <stdio.h>

void createOrthoProjectionMatrix(float left, float right, float bottom, float top, float near, float far, float* matrix);

// Vertex Shader source code (with projection matrix support)
const GLchar* vertexShaderSource = R"(
    attribute vec4 position;
    uniform mat4 projection;
    void main()
    {
        gl_Position = projection * position;
    }
)";

// Fragment Shader source code
const GLchar* fragmentShaderSource = R"(
    precision mediump float;
    void main()
    {
        gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0); // Blue color
    }
)";

// Square vertices (100x100 square centered at 400, 240)
GLfloat vertices[] = {
    // First triangle (top-left, bottom-left, bottom-right)
    350.0f, 250.0f, 0.0f,  // Top-left vertex
    350.0f, 150.0f, 0.0f,  // Bottom-left vertex
    450.0f, 150.0f, 0.0f,  // Bottom-right vertex

    // Second triangle (top-left, bottom-right, top-right)
    350.0f, 250.0f, 0.0f,  // Top-left vertex
    450.0f, 150.0f, 0.0f,  // Bottom-right vertex
    450.0f, 250.0f, 0.0f   // Top-right vertex
};

// Function to compile a shader
GLuint compileShader(GLenum type, const GLchar* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // Check for compilation errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
        printf("Error compiling shader: %s\n", infoLog);
        return 0;
    }

    return shader;
}

// Function to create a shader program
GLuint createShaderProgram() {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for linking errors
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(shaderProgram, sizeof(infoLog), NULL, infoLog);
        printf("Error linking program: %s\n", infoLog);
        return 0;
    }

    // Clean up shaders (they're now part of the program)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

int main(int argc, char* argv[]) {
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
    SDL_Window* window = SDL_CreateWindow("Brickout - Pibit",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          800, 480,
                                          SDL_WINDOW_OPENGL);

    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create an OpenGL context
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (glContext == NULL) {
        printf("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Initialize GLEW
    glewExperimental = GL_TRUE; // Ensure GLEW uses modern techniques
    if (glewInit() != GLEW_OK) {
        printf("Error initializing GLEW!\n");
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Set the viewport
    glViewport(0, 0, 800, 480);

    // Create and use the shader program
    GLuint shaderProgram = createShaderProgram();
    glUseProgram(shaderProgram);

    // Create a vertex buffer object (VBO) and upload the vertex data
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Get the location of the "position" attribute in the vertex shader
    GLint positionAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(positionAttrib);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

    // Enable the position attribute
    glEnableVertexAttribArray(positionAttrib);

    // Specify how the data for the position attribute is retrieved from the buffer
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

    // Create an orthographic projection matrix for 800x480 window
    float orthoMatrix[16];
    createOrthoProjectionMatrix(0.0f, 800.0f, 0.0f, 480.0f, 0.1f, 100.0f, orthoMatrix);

    // Get the location of the projection matrix in the shader and set it
    GLint projectionUniform = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, orthoMatrix);

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

        // Draw the triangle
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

void createOrthoProjectionMatrix(float left, float right, float bottom, float top, float near, float far, float* matrix) {
    matrix[0] = 2.0f / (right - left);
    matrix[1] = 0.0f;
    matrix[2] = 0.0f;
    matrix[3] = 0.0f;

    matrix[4] = 0.0f;
    matrix[5] = 2.0f / (top - bottom);
    matrix[6] = 0.0f;
    matrix[7] = 0.0f;

    matrix[8] = 0.0f;
    matrix[9] = 0.0f;
    matrix[10] = -2.0f / (far - near);
    matrix[11] = 0.0f;

    matrix[12] = -(right + left) / (right - left);
    matrix[13] = -(top + bottom) / (top - bottom);
    matrix[14] = -(far + near) / (far - near);
    matrix[15] = 1.0f;
}
