//
// Created by Jahid Uddin on 15.11.2026.
//

#ifndef SHADER_TOYS_ENGINE_H
#define SHADER_TOYS_ENGINE_H

#include "glad/glad.h"

#define GLFW_INCLUDE_NONE
#include "atom.h"
#include "primitives.h"
#include "GLFW/glfw3.h"

typedef struct Engine {
    GLFWwindow *window;
    Mesh sphere;
    Atom *atom;
    GLuint shaderProgram;
    float camera_position[3];
    float camera_yaw;
    float camera_pitch;
    float camera_fov;
    double last_mouse_x;
    double last_mouse_y;
    int first_mouse;
    int cursor_captured;
} Engine;

Engine *engine_new();

void engine_update(Engine *engine, double dt);

void engine_render(const Engine *engine, float time);

void engine_loop(Engine *engine);

void engine_run(Engine *engine);

void engine_free(Engine *engine);

#endif //SHADER_TOYS_ENGINE_H
