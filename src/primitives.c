//
// Created by Jahid Uddin on 15.11.2026.
//

#include "primitives.h"
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Mesh prim_create_uv_sphere(float radius, unsigned int sectors, unsigned int stacks) {
    Mesh mesh = {0};

    if (sectors < 3) sectors = 3;
    if (stacks < 2) stacks = 2;

    const size_t vertex_count = (size_t) (stacks + 1) * (sectors + 1);
    const size_t index_count = (size_t) (stacks - 1) * sectors * 6;

    Vertex *vertices = malloc(vertex_count * sizeof(Vertex));
    unsigned int *indices = malloc(index_count * sizeof(unsigned int));

    if (!vertices || !indices) {
        free(vertices);
        free(indices);
        return mesh;
    }

    const float pi = M_PI;
    const float sector_step = 2.0f * pi / (float) sectors;
    const float stack_step = pi / (float) stacks;

    size_t v_idx = 0;
    for (unsigned int i = 0; i <= stacks; ++i) {
        const float stack_angle = pi / 2.0f - (float) i * stack_step;
        const float xy = cosf(stack_angle);
        const float y = sinf(stack_angle);

        for (unsigned int j = 0; j <= sectors; ++j) {
            const float sector_angle = (float) j * sector_step;

            const float x = xy * cosf(sector_angle);
            const float z = xy * sinf(sector_angle);

            vertices[v_idx].pos[0] = x * radius;
            vertices[v_idx].pos[1] = y * radius;
            vertices[v_idx].pos[2] = z * radius;

            vertices[v_idx].normal[0] = x;
            vertices[v_idx].normal[1] = y;
            vertices[v_idx].normal[2] = z;

            vertices[v_idx].uv[0] = (float) j / (float) sectors;
            vertices[v_idx].uv[1] = (float) i / (float) stacks;

            ++v_idx;
        }
    }

    size_t i_idx = 0;
    for (unsigned int i = 0; i < stacks; ++i) {
        unsigned int k1 = i * (sectors + 1);
        unsigned int k2 = k1 + sectors + 1;

        for (unsigned int j = 0; j < sectors; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices[i_idx++] = k1;
                indices[i_idx++] = k2;
                indices[i_idx++] = k1 + 1;
            }

            if (i != (stacks - 1)) {
                indices[i_idx++] = k1 + 1;
                indices[i_idx++] = k2;
                indices[i_idx++] = k2 + 1;
            }
        }
    }

    mesh.index_count = (GLsizei) i_idx;

    glCreateVertexArrays(1, &mesh.vao);
    glCreateBuffers(1, &mesh.vbo);
    glCreateBuffers(1, &mesh.ebo);

    glNamedBufferStorage(mesh.vbo, (GLsizeiptr) (vertex_count * sizeof(Vertex)), vertices, 0);
    glNamedBufferStorage(mesh.ebo, (GLsizeiptr) (i_idx * sizeof(unsigned int)), indices, 0);

    glVertexArrayVertexBuffer(mesh.vao, 0, mesh.vbo, 0, sizeof(Vertex));
    glVertexArrayElementBuffer(mesh.vao, mesh.ebo);

    // Location 0: Position
    glEnableVertexArrayAttrib(mesh.vao, 0);
    glVertexArrayAttribFormat(mesh.vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, pos));
    glVertexArrayAttribBinding(mesh.vao, 0, 0);

    // Location 1: Normale
    glEnableVertexArrayAttrib(mesh.vao, 1);
    glVertexArrayAttribFormat(mesh.vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
    glVertexArrayAttribBinding(mesh.vao, 1, 0);

    // Location 2: UV
    glEnableVertexArrayAttrib(mesh.vao, 2);
    glVertexArrayAttribFormat(mesh.vao, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
    glVertexArrayAttribBinding(mesh.vao, 2, 0);

    free(vertices);
    free(indices);

    return mesh;
}

void prim_destroy_mesh(Mesh *mesh) {
    if (!mesh) return;

    if (mesh->vao != 0) {
        glDeleteVertexArrays(1, &mesh->vao);
        mesh->vao = 0;
    }
    if (mesh->vbo != 0) {
        glDeleteBuffers(1, &mesh->vbo);
        mesh->vbo = 0;
    }
    if (mesh->ebo != 0) {
        glDeleteBuffers(1, &mesh->ebo);
        mesh->ebo = 0;
    }
    mesh->index_count = 0;
}
