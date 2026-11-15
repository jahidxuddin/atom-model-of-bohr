//
// Created by Jahid Uddin on 14.11.2026.
//
#include "../include/shader.h"

static char *readFile(const char *filepath) {
    FILE *file = fopen(filepath, "rb");
    if (!file) {
        fprintf(stderr, "Error: Could not open shader file '%s'.\n", filepath);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    const long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(length + 1);
    if (!buffer) {
        fprintf(stderr, "Error: Not enough memory for '%s'.\n", filepath);
        fclose(file);
        return NULL;
    }

    size_t readBytes = fread(buffer, 1, length, file);
    buffer[readBytes] = '\0';

    fclose(file);
    return buffer;
}

static GLuint compileShader(GLenum type, const char *source, const char *debugName) {
    const GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
        fprintf(stderr, "Shader compilation error (%s):\n%s\n", debugName, infoLog);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint createShaderProgram(const char *vertexPath, const char *fragmentPath) {
    char *vertexCode = readFile(vertexPath);
    char *fragmentCode = readFile(fragmentPath);

    if (!vertexCode || !fragmentCode) {
        free(vertexCode);
        free(fragmentCode);
        return 0;
    }

    const GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexCode, vertexPath);
    const GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentCode, fragmentPath);

    free(vertexCode);
    free(fragmentCode);

    if (!vertexShader || !fragmentShader) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetProgramInfoLog(program, sizeof(infoLog), NULL, infoLog);
        fprintf(stderr, "Shader linking error:\n%s\n", infoLog);
        glDeleteProgram(program);
        program = 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}
