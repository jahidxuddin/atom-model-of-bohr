//
// Created by Jahid Uddin on 15.11.2026.
//

#include "atom.h"

#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define ORBIT_SEGMENTS 128u

typedef struct OrbitVertex {
    float pos[3];
    uint16_t info;
    GLuint is_orbit;
} OrbitVertex;

static unsigned int max_electrons_in_shell(const unsigned int shell_n) {
    return 2u * shell_n * shell_n;
}

static uint16_t pack_particle(const ParticleType type, const BohrShell shell,
                              const unsigned int inclination, const unsigned int phase) {
    return (uint16_t) (((unsigned int) type & 0x03u)
                       | (((unsigned int) shell & 0x07u) << 2u)
                       | ((inclination & 0x07u) << 5u)
                       | ((phase & 0xffu) << 8u));
}

static unsigned int shell_inclination(const unsigned int shell_n) {
    return ((shell_n - 1u) * 2u) % 8u;
}

static int create_orbit_geometry(Atom *atom) {
    unsigned int shell_count = 0;
    for (unsigned int shell_n = SHELL_K; shell_n < SHELL_COUNT; ++shell_n) {
        if (atom->electron_count > max_electrons_in_shell(shell_n - 1u)) {
            ++shell_count;
        }
    }
    if (shell_count == 0) {
        return 1;
    }

    atom->orbit_ring_count = shell_count;
    atom->orbit_vertex_count = (GLsizei) (shell_count * (ORBIT_SEGMENTS + 1u));
    OrbitVertex *vertices = malloc((size_t) atom->orbit_vertex_count * sizeof(OrbitVertex));
    if (!vertices) {
        return 0;
    }

    unsigned int vertex_index = 0;
    for (unsigned int shell_n = SHELL_K; shell_n < SHELL_COUNT; ++shell_n) {
        if (atom->electron_count <= max_electrons_in_shell(shell_n - 1u)) {
            continue;
        }

        const uint16_t info = pack_particle(PARTICLE_ELECTRON, (BohrShell) shell_n,
                                            shell_inclination(shell_n), 0);
        const float radius = 0.25f * (float) shell_n * (float) shell_n;
        const float tilt_angle = ((float) shell_inclination(shell_n) / 8.0f) * (float) M_PI;
        const float tilt_cos = cosf(tilt_angle);
        const float tilt_sin = sinf(tilt_angle);
        for (unsigned int segment = 0; segment <= ORBIT_SEGMENTS; ++segment) {
            const float angle = (2.0f * (float) M_PI * (float) segment) / (float) ORBIT_SEGMENTS;
            const float x = cosf(angle) * radius;
            OrbitVertex *vertex = &vertices[vertex_index++];
            vertex->pos[0] = x * tilt_cos;
            vertex->pos[1] = x * tilt_sin;
            vertex->pos[2] = sinf(angle) * radius;
            vertex->info = info;
            vertex->is_orbit = 1;
        }
    }

    glCreateVertexArrays(1, &atom->orbit_vao);
    glCreateBuffers(1, &atom->orbit_vbo);
    if (!atom->orbit_vao || !atom->orbit_vbo) {
        free(vertices);
        return 0;
    }

    glNamedBufferData(atom->orbit_vbo,
                      (GLsizeiptr) (atom->orbit_vertex_count * sizeof(OrbitVertex)),
                      vertices, GL_STATIC_DRAW);
    glVertexArrayVertexBuffer(atom->orbit_vao, 0, atom->orbit_vbo, 0, sizeof(OrbitVertex));

    glEnableVertexArrayAttrib(atom->orbit_vao, 0);
    glVertexArrayAttribFormat(atom->orbit_vao, 0, 3, GL_FLOAT, GL_FALSE,
                              offsetof(OrbitVertex, pos));
    glVertexArrayAttribBinding(atom->orbit_vao, 0, 0);

    glEnableVertexArrayAttrib(atom->orbit_vao, 3);
    glVertexArrayAttribIFormat(atom->orbit_vao, 3, 1, GL_UNSIGNED_SHORT,
                               offsetof(OrbitVertex, info));
    glVertexArrayAttribBinding(atom->orbit_vao, 3, 0);

    glEnableVertexArrayAttrib(atom->orbit_vao, 4);
    glVertexArrayAttribIFormat(atom->orbit_vao, 4, 1, GL_UNSIGNED_INT,
                               offsetof(OrbitVertex, is_orbit));
    glVertexArrayAttribBinding(atom->orbit_vao, 4, 0);

    free(vertices);
    return 1;
}

Atom *atom_new(const unsigned int amount_electron, const unsigned int amount_proton,
               const unsigned int amount_neutron) {
    const unsigned int max_electrons = max_electrons_in_shell(SHELL_COUNT - 1u);
    if (amount_electron > max_electrons || amount_proton > UINT_MAX - amount_neutron) {
        return NULL;
    }

    const unsigned int nucleon_count = amount_proton + amount_neutron;
    if (amount_electron > UINT_MAX - nucleon_count) {
        return NULL;
    }

    const unsigned int particle_count = nucleon_count + amount_electron;
    if (particle_count > INT_MAX ||
        (size_t) particle_count > (SIZE_MAX - sizeof(Atom)) / sizeof(uint16_t)) {
        return NULL;
    }

    Atom *atom = calloc(1, sizeof(Atom) + (size_t) particle_count * sizeof(uint16_t));
    if (!atom) {
        return NULL;
    }

    atom->particle_count = particle_count;
    atom->electron_count = amount_electron;
    atom->proton_count = amount_proton;
    atom->neutron_count = amount_neutron;

    unsigned int index = 0;
    for (unsigned int i = 0; i < amount_proton; ++i) {
        atom->particles[index++] = pack_particle(PARTICLE_PROTON, SHELL_NUCLEUS, 0, i);
    }
    for (unsigned int i = 0; i < amount_neutron; ++i) {
        atom->particles[index++] = pack_particle(PARTICLE_NEUTRON, SHELL_NUCLEUS, 0,
                                                 amount_proton + i);
    }

    unsigned int electron_index = 0;
    for (unsigned int shell_n = SHELL_K;
         shell_n < SHELL_COUNT && electron_index < amount_electron; ++shell_n) {
        const unsigned int previous_capacity = max_electrons_in_shell(shell_n - 1u);
        const unsigned int shell_capacity = max_electrons_in_shell(shell_n) - previous_capacity;
        const unsigned int electrons_in_shell = amount_electron - electron_index < shell_capacity
                                                    ? amount_electron - electron_index
                                                    : shell_capacity;

        for (unsigned int i = 0; i < electrons_in_shell; ++i, ++electron_index) {
            const unsigned int phase = (i * 256u) / electrons_in_shell;
            const unsigned int inclination = shell_inclination(shell_n);
            atom->particles[index++] = pack_particle(PARTICLE_ELECTRON,
                                                     (BohrShell) shell_n,
                                                     inclination, phase);
        }
    }

    if (particle_count > 0) {
        glCreateBuffers(1, &atom->particle_buffer);
        if (atom->particle_buffer == 0) {
            free(atom);
            return NULL;
        }
        glNamedBufferData(atom->particle_buffer,
                          (GLsizeiptr) (particle_count * sizeof(uint16_t)),
                          atom->particles, GL_STATIC_DRAW);
    }

    if (!create_orbit_geometry(atom)) {
        atom_free(atom);
        return NULL;
    }

    return atom;
}

void atom_update(Atom *atom, double dt) {
    (void) atom;
    (void) dt;
}

void atom_render(Atom *atom, const Mesh *sphere, const float time) {
    if (!atom || !sphere || atom->particle_count == 0 || sphere->vao == 0 ||
        sphere->index_count <= 0 || atom->particle_buffer == 0) {
        return;
    }

    glUniform1f(2, time);
    if (atom->orbit_vao != 0 && atom->orbit_vertex_count > 0) {
        glBindVertexArray(atom->orbit_vao);
        const GLsizei vertices_per_ring = (GLsizei) ORBIT_SEGMENTS + 1;
        for (unsigned int ring = 0; ring < atom->orbit_ring_count; ++ring) {
            glDrawArrays(GL_LINE_STRIP, (GLint) (ring * (unsigned int) vertices_per_ring),
                         vertices_per_ring);
        }
    }

    glBindVertexArray(sphere->vao);
    glBindBuffer(GL_ARRAY_BUFFER, atom->particle_buffer);
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 1, GL_UNSIGNED_SHORT, sizeof(uint16_t), (const void *) 0);
    glVertexAttribDivisor(3, 1);
    glDrawElementsInstanced(GL_TRIANGLES, sphere->index_count, GL_UNSIGNED_INT, NULL,
                            (GLsizei) atom->particle_count);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void atom_free(Atom *atom) {
    if (!atom) {
        return;
    }

    if (atom->particle_buffer != 0) {
        glDeleteBuffers(1, &atom->particle_buffer);
        atom->particle_buffer = 0;
    }
    if (atom->orbit_vao != 0) {
        glDeleteVertexArrays(1, &atom->orbit_vao);
        atom->orbit_vao = 0;
    }
    if (atom->orbit_vbo != 0) {
        glDeleteBuffers(1, &atom->orbit_vbo);
        atom->orbit_vbo = 0;
    }
    free(atom);
}
