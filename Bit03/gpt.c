#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

// Ball structure
typedef struct {
    float x, y;           // Ball position
    float radius;         // Ball radius
    float velocityX, velocityY; // Ball velocity
    GLuint textureID;     // OpenGL texture ID
} Ball;

// Load texture function (using SDL_image)
GLuint loadTexture(const char* filePath) {
    SDL_Surface* surface = IMG_Load(filePath);
    if (!surface) {
        printf("Error: Unable to load image %s! SDL_image Error: %s\n", filePath, IMG_GetError());
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

    SDL_FreeSurface(surface);
    return textureID;
}

// Create orthographic projection matrix function
void createOrthoProjectionMatrix(float* matrix, float width, float height);

// Vertex Shader source code (with projection matrix support)
const GLchar* vertexShaderSource = R"(
    attribute vec4 position;
    attribute vec2 texCoord;
    varying vec2 TexCoord;
    uniform mat4 projection;

    void main()
    {
        gl_Position = projection * position;
        TexCoord = texCoord; // Pass texture coordinates to fragment shader
    }
)";

// Fragment Shader source code
const GLchar* fragmentShaderSource = R"(
    precision mediump float;
    varying vec2 TexCoord;
    uniform sampler2D textureSampler;

    void main()
    {
        vec4 texColor = texture2D(textureSampler, TexCoord); // Sample the texture color
        if (texColor.a < 0.1) {
            discard; // Discard transparent pixels
        }
        gl_FragColor = texColor; // Set the fragment color
    }
)";

// Function to compile a shader
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

    // Initialize SDL_image
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        SDL_Quit();
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

    // Create a vertex buffer object (VBO)
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Create an orthographic projection matrix for 800x480 window
    float orthoMatrix[16];
    createOrthoProjectionMatrix(orthoMatrix, 800.0f, 480.0f);

    // Get the location of the projection matrix in the shader and set it
    GLint projectionUniform = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, orthoMatrix);

    // Enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Initialize ball
    Ball ball;
    ball.x = 400.0f;
    ball.y = 240.0f;
    ball.radius = 10.0f;
    ball.velocityX = 200.0f;  // Move 200 pixels per second horizontally
    ball.velocityY = 150.0f;  // Move 150 pixels per second vertically
    ball.textureID = loadTexture("ball.png");  // Load texture

    // Main loop variables
    int running = 1;
    SDL_Event event;
    Uint32 lastTime = SDL_GetTicks();

    // Main loop
    while (running) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        // Event handling
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        // Update ball position
        ball.x += ball.velocityX * deltaTime;
        ball.y += ball.velocityY * deltaTime;

        // Bounce the ball off the screen edges
        if (ball.x - ball.radius < 0 || ball.x + ball.radius > 800) {
            ball.velocityX = -ball.velocityX;  // Reverse X velocity
        }
        if (ball.y - ball.radius < 0 || ball.y + ball.radius > 480) {
            ball.velocityY = -ball.velocityY;  // Reverse Y velocity
        }

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);

        // Update ball vertices based on its new position
        GLfloat ballVertices[] = {
            ball.x - ball.radius, ball.y + ball.radius, 0.0f,  0.0f, 1.0f,  // Top-left
            ball.x - ball.radius, ball.y - ball.radius, 0.0f,  0.0f, 0.0f,  // Bottom-left
            ball.x + ball.radius, ball.y - ball.radius, 0.0f,  1.0f, 0.0f,  // Bottom-right

            ball.x - ball.radius, ball.y + ball.radius, 0.0f,  0.0f, 1.0f,  // Top-left
            ball.x + ball.radius, ball.y - ball.radius, 0.0f,  1.0f, 0.0f,  // Bottom-right
            ball.x + ball.radius, ball.y + ball.radius, 0.0f,  1.0f, 1.0f   // Top-right
        };

        // Update the vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(ballVertices), ballVertices, GL_DYNAMIC_DRAW);

        // Bind the ball texture
        glBindTexture(GL_TEXTURE_2D, ball.textureID);

        // Draw the ball
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Swap the buffers (double buffering)
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glDeleteTextures(1, &ball.textureID);
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    IMG_Quit();

    return 0;
}

// Define the createOrthoProjectionMatrix, compileShader, and createShaderProgram functions as before


void createOrthoProjectionMatrix(float* matrix, float width, float height) {

    float right, left, top, bottom;
	bottom = 0;
	top = height;
	left = 0;
	right = width;

	float zNear = -0.1f;
	float zFar = 1.0f;

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
    matrix[10] = -2.0f / (zFar - zNear);
    matrix[11] = 0.0f;

    matrix[12] = -(right + left) / (right - left);
    matrix[13] = -(top + bottom) / (top - bottom);
    matrix[14] = -(zFar + zNear) / (zFar - zNear);
    matrix[15] = 1.0f;
}
