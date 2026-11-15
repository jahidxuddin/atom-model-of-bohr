//
// Created by Jahid Uddin on 15.11.2026.
//

#ifndef SHADER_TOYS_PRIMITIVES_H
#define SHADER_TOYS_PRIMITIVES_H

#include <stddef.h>
#include <glad/glad.h>

typedef struct {
    float pos[3];
    float normal[3];
    float uv[2];
} Vertex;

typedef struct {
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLsizei index_count;
} Mesh;

Mesh prim_create_uv_sphere(float radius, unsigned int sectors, unsigned int stacks);

void prim_destroy_mesh(Mesh *mesh);

#endif //SHADER_TOYS_PRIMITIVES_H
