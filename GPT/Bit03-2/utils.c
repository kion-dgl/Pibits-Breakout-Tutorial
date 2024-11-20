#include "utils.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>

void createOrthoProjectionMatrix(float* matrix, float width, float height) {
    float right = width;
    float left = 0;
    float top = height;
    float bottom = 0;
    float zNear = -1.0f;
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

GLuint loadTexture(const char* filePath) {
    SDL_Surface* surface = IMG_Load(filePath);
    if (!surface) {
        printf("Error: Unable to load image %s! SDL_image Error: %s\n", filePath, IMG_GetError());
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

    SDL_FreeSurface(surface);
    return textureID;
}

GLuint compileShader(GLenum type, const GLchar* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

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

GLuint createShaderProgram() {
    const GLchar* vertexShaderSource = R"(
        attribute vec4 position;
        attribute vec2 texCoord;
        varying vec2 TexCoord;
        uniform mat4 projection;

        void main()
        {
            gl_Position = projection * position;
            TexCoord = texCoord;
        }
    )";

    const GLchar* fragmentShaderSource = R"(
        precision mediump float;
        varying vec2 TexCoord;
        uniform sampler2D textureSampler;

        void main()
        {
            vec4 texColor = texture2D(textureSampler, TexCoord);
            if (texColor.a < 0.1) discard;
            gl_FragColor = texColor;
        }
    )";

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(shaderProgram, sizeof(infoLog), NULL, infoLog);
        printf("Error linking shader program: %s\n", infoLog);
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}
