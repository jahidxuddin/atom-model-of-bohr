//
// Created by Jahid Uddin on 15.11.2026.
//

#ifndef SHADER_TOYS_ENGINE_H
#define SHADER_TOYS_ENGINE_H

#include "glad/glad.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

typedef struct Engine {
    GLFWwindow* window;
    GLuint VAO, VBO, EBO;
    GLuint shaderProgram;
} Engine;

Engine* engine_new();

void engine_update(Engine *engine, double dt);

void engine_render(const Engine *engine);

void engine_loop(Engine *engine);

void engine_run(Engine *engine);

void engine_free(const Engine *engine);

#endif //SHADER_TOYS_ENGINE_H
