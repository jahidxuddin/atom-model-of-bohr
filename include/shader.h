//
// Created by Jahid Uddin on 14.11.2026.
//

#ifndef SHADER_TOYS_SHADER_H
#define SHADER_TOYS_SHADER_H

#include <stdio.h>
#include <stdlib.h>

#include "glad/glad.h"

static char* readFile(const char* filepath);

static GLuint compileShader(GLenum type, const char* source, const char* debugName);

GLuint createShaderProgram(const char* vertexPath, const char* fragmentPath);

#endif //SHADER_TOYS_SHADER_H
