//
// Created by Jahid Uddin on 15.11.2026.
//

#include "engine.h"

#include <stdio.h>
#include <stdlib.h>

#include "shader.h"
#include "glad/glad.h"

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

    GLFWwindow *window = glfwCreateWindow(1080, 720, "Atommodel from Bohr", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Window cannot be created\n");
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        fprintf(stderr, "Error while initialising GLAD\n");
        return NULL;
    }

    Engine *engine = malloc(sizeof(Engine));

    engine->window = window;

    return engine;
}

void engine_update(Engine *engine, double dt) {
}

void engine_render(const Engine *engine) {
    glBindVertexArray(engine->VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void engine_loop(Engine *engine) {
    const GLuint shaderProgram = createShaderProgram("src/shaders/pattern.vert", "src/shaders/pattern.frag");
    engine->shaderProgram = shaderProgram;
    const float vertices[] = {
        -1.0f, -1.0f, 0.0f, // 0: unten links
        1.0f, -1.0f, 0.0f, // 1: unten rechts
        1.0f, 1.0f, 0.0f, // 2: oben rechts
        -1.0f, 1.0f, 0.0f // 3: oben links
    };

    const unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    GLuint VAO, VBO, EBO;

    glCreateVertexArrays(1, &VAO);
    glCreateBuffers(1, &VBO);
    glCreateBuffers(1, &EBO);

    engine->VAO = VAO;
    engine->VBO = EBO;
    engine->EBO = EBO;

    glNamedBufferStorage(VBO, sizeof(vertices), vertices, 0);
    glNamedBufferStorage(EBO, sizeof(indices), indices, 0);

    glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 3 * sizeof(float));
    glVertexArrayElementBuffer(VAO, EBO);

    glEnableVertexArrayAttrib(VAO, 0);
    glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(VAO, 0, 0);

    const GLint timeLoc = glGetUniformLocation(shaderProgram, "uTime");
    const GLint resLoc = glGetUniformLocation(shaderProgram, "uResolution");

    while (!glfwWindowShouldClose(engine->window)) {
        int width, height;
        glfwGetFramebufferSize(engine->window, &width, &height);
        glViewport(0, 0, width, height);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        if (timeLoc != -1)
            glUniform1f(timeLoc, (float) glfwGetTime());
        if (resLoc != -1)
            glUniform2f(resLoc, (float) width, (float) height);

        engine_update(engine, glfwGetTime());

        engine_render(engine);

        glfwSwapBuffers(engine->window);
        glfwPollEvents();
    }
}

void engine_run(Engine *engine) {
    engine_loop(engine);
    engine_free(engine);
}

void engine_free(const Engine *engine) {
    glDeleteVertexArrays(1, &engine->VAO);
    glDeleteBuffers(1, &engine->VBO);
    glDeleteBuffers(1, &engine->EBO);
    glDeleteProgram(engine->shaderProgram);
    glfwDestroyWindow(engine->window);
    glfwTerminate();
}
