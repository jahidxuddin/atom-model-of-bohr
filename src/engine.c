//
// Created by Jahid Uddin on 15.11.2026.
//

#include "engine.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cglm/cglm.h>

#include "shader.h"
#include "glad/glad.h"

static void set_cursor_capture(Engine *engine, const int captured) {
    engine->cursor_captured = captured;
    engine->first_mouse = 1;
    glfwSetInputMode(engine->window, GLFW_CURSOR,
                     captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

static void cursor_position_callback(GLFWwindow *window, const double x, const double y) {
    Engine *engine = glfwGetWindowUserPointer(window);
    if (!engine || !engine->cursor_captured) return;

    if (engine->first_mouse) {
        engine->last_mouse_x = x;
        engine->last_mouse_y = y;
        engine->first_mouse = 0;
        return;
    }

    const float sensitivity = 0.1f;
    const float x_offset = (float) (x - engine->last_mouse_x) * sensitivity;
    const float y_offset = (float) (engine->last_mouse_y - y) * sensitivity;
    engine->last_mouse_x = x;
    engine->last_mouse_y = y;

    engine->camera_yaw += x_offset;
    engine->camera_pitch += y_offset;
    if (engine->camera_pitch > 89.0f) engine->camera_pitch = 89.0f;
    if (engine->camera_pitch < -89.0f) engine->camera_pitch = -89.0f;
}

static void scroll_callback(GLFWwindow *window, const double x_offset, const double y_offset) {
    (void) x_offset;
    Engine *engine = glfwGetWindowUserPointer(window);
    if (!engine) return;
    engine->camera_fov -= (float) y_offset;
    if (engine->camera_fov < 20.0f) engine->camera_fov = 20.0f;
    if (engine->camera_fov > 75.0f) engine->camera_fov = 75.0f;
}

static void key_callback(GLFWwindow *window, const int key, const int scancode,
                         const int action, const int mods) {
    (void) scancode;
    (void) mods;
    Engine *engine = glfwGetWindowUserPointer(window);
    if (!engine || key != GLFW_KEY_ESCAPE || action != GLFW_PRESS) return;
    set_cursor_capture(engine, !engine->cursor_captured);
}

static void mouse_button_callback(GLFWwindow *window, const int button,
                                 const int action, const int mods) {
    (void) mods;
    Engine *engine = glfwGetWindowUserPointer(window);
    if (engine && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS &&
        !engine->cursor_captured) {
        set_cursor_capture(engine, 1);
    }
}

static void camera_front(const Engine *engine, vec3 front) {
    const float yaw = glm_rad(engine->camera_yaw);
    const float pitch = glm_rad(engine->camera_pitch);
    front[0] = cosf(yaw) * cosf(pitch);
    front[1] = sinf(pitch);
    front[2] = sinf(yaw) * cosf(pitch);
    glm_vec3_normalize(front);
}

static void set_scene_matrices(const Engine *engine, const int width, const int height) {
    if (width <= 0 || height <= 0) return;

    mat4 view, projection;
    vec3 eye = {engine->camera_position[0], engine->camera_position[1], engine->camera_position[2]};
    vec3 front;
    vec3 center;
    vec3 up = {0.0f, 1.0f, 0.0f};
    const float near_plane = 0.1f;
    const float far_plane = 100.0f;
    const float aspect = (float)width / (float)height;

    camera_front(engine, front);
    glm_vec3_add(eye, front, center);
    glm_lookat(eye, center, up, view);
    glm_perspective(glm_rad(engine->camera_fov), aspect, near_plane, far_plane, projection);

    glUniformMatrix4fv(0, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(1, 1, GL_FALSE, &projection[0][0]);
}

Engine *engine_new() {
    if (!glfwInit()) {
        fprintf(stderr, "Error while initialising GLFW\n");
        return NULL;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(1080, 720, "Atom model of Bohr", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Window cannot be created\n");
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        fprintf(stderr, "Error while initialising GLAD\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return NULL;
    }

    Engine *engine = calloc(1, sizeof(Engine));
    if (!engine) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return NULL;
    }

    engine->window = window;
    engine->camera_position[2] = 7.5f;
    engine->camera_yaw = -90.0f;
    engine->camera_fov = 45.0f;
    engine->first_mouse = 1;
    engine->cursor_captured = 1;
    glfwSetWindowUserPointer(window, engine);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    set_cursor_capture(engine, 1);

    return engine;
}

void engine_update(Engine *engine, const double dt) {
    if (!engine || !engine->cursor_captured || dt <= 0.0) return;

    vec3 front, world_up = {0.0f, 1.0f, 0.0f}, right;
    camera_front(engine, front);
    glm_vec3_cross(front, world_up, right);
    glm_vec3_normalize(right);

    float speed = 3.0f * (float) dt;
    if (glfwGetKey(engine->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        speed *= 3.0f;
    }

    for (int axis = 0; axis < 3; ++axis) {
        if (glfwGetKey(engine->window, GLFW_KEY_W) == GLFW_PRESS)
            engine->camera_position[axis] += front[axis] * speed;
        if (glfwGetKey(engine->window, GLFW_KEY_S) == GLFW_PRESS)
            engine->camera_position[axis] -= front[axis] * speed;
        if (glfwGetKey(engine->window, GLFW_KEY_D) == GLFW_PRESS)
            engine->camera_position[axis] += right[axis] * speed;
        if (glfwGetKey(engine->window, GLFW_KEY_A) == GLFW_PRESS)
            engine->camera_position[axis] -= right[axis] * speed;
    }

    if (glfwGetKey(engine->window, GLFW_KEY_SPACE) == GLFW_PRESS)
        engine->camera_position[1] += speed;
    if (glfwGetKey(engine->window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        engine->camera_position[1] -= speed;
}

void engine_render(const Engine *engine, const float time) {
    atom_render(engine->atom, &engine->sphere, time);
}

void engine_loop(Engine *engine) {
    const GLuint shaderProgram = createShaderProgram("src/shaders/particle.vert", "src/shaders/particle.frag");
    engine->shaderProgram = shaderProgram;

    const Mesh sphere = prim_create_uv_sphere(1.0f, 36, 18);

    engine->sphere = sphere;
    engine->atom = atom_new(18, 18, 22);
    if (!engine->atom || !engine->sphere.vao || !shaderProgram) {
        fprintf(stderr, "Atom, sphere, or shader initialization failed\n");
        return;
    }

    double previous_frame = glfwGetTime();
    while (!glfwWindowShouldClose(engine->window)) {
        const double current_frame = glfwGetTime();
        const double dt = current_frame - previous_frame;
        previous_frame = current_frame;

        int width, height;
        glfwGetFramebufferSize(engine->window, &width, &height);
        glViewport(0, 0, width, height);

        glClearColor(0.008f, 0.012f, 0.028f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        engine_update(engine, dt);

        glUseProgram(shaderProgram);
        set_scene_matrices(engine, width, height);

        engine_render(engine, (float) current_frame);

        glfwSwapBuffers(engine->window);
        glfwPollEvents();
    }
}

void engine_run(Engine *engine) {
    if (!engine) return;
    engine_loop(engine);
    engine_free(engine);
}

void engine_free(Engine *engine) {
    if (!engine) return;
    atom_free(engine->atom);
    prim_destroy_mesh(&engine->sphere);
    glDeleteProgram(engine->shaderProgram);
    glfwDestroyWindow(engine->window);
    glfwTerminate();
    free(engine);
}
