#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>
#include <stdio.h>

void createOrthoProjectionMatrix(float* matrix, float width, float height);

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


// Updated Vertex Shader source code
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


// Updated Fragment Shader source code
const GLchar* fragmentShaderSource = R"(
    precision mediump float;
    varying vec2 TexCoord; // Pass the texture coordinates from the vertex shader
    uniform sampler2D textureSampler; // The texture sampler

    void main()
    {
        gl_FragColor = texture2D(textureSampler, TexCoord); // Sample the texture
    }
)";


// Updated vertex data (with texture coordinates)
GLfloat vertices[] = {
    // Position (X, Y, Z)   // Texture coordinates (U, V)
    350.0f, 250.0f, 0.0f,  0.0f, 1.0f,  // Top-left
    350.0f, 150.0f, 0.0f,  0.0f, 0.0f,  // Bottom-left
    450.0f, 150.0f, 0.0f,  1.0f, 0.0f,  // Bottom-right

    350.0f, 250.0f, 0.0f,  0.0f, 1.0f,  // Top-left
    450.0f, 150.0f, 0.0f,  1.0f, 0.0f,  // Bottom-right
    450.0f, 250.0f, 0.0f,  1.0f, 1.0f   // Top-right
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
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);

    // Enable the position attribute
    glEnableVertexAttribArray(positionAttrib);


    // Create an orthographic projection matrix for 800x480 window
    float orthoMatrix[16];
    createOrthoProjectionMatrix(orthoMatrix, 800.0f, 480.0f);

    // Get the location of the projection matrix in the shader and set it
    GLint projectionUniform = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, orthoMatrix);

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
