//
// Created by Jahid Uddin on 15.11.2026.
//

#ifndef ATOM_MODEL_OF_BOHR_ATOM_H
#define ATOM_MODEL_OF_BOHR_ATOM_H
#include <stdint.h>
#include "primitives.h"

typedef enum {
    PARTICLE_NONE     = 0,
    PARTICLE_PROTON   = 1,
    PARTICLE_NEUTRON  = 2,
    PARTICLE_ELECTRON = 3
} ParticleType;

typedef enum {
    SHELL_NUCLEUS = 0, // 0
    SHELL_K,           // 1
    SHELL_L,           // 2
    SHELL_M,           // 3
    SHELL_N,           // 4
    SHELL_O,           // 5
    SHELL_P,           // 6
    SHELL_Q,           // 7
    SHELL_COUNT        // 8
} BohrShell;

typedef struct Atom {
    unsigned int particle_count;
    unsigned int electron_count;
    unsigned int proton_count;
    unsigned int neutron_count;
    GLuint particle_buffer;
    GLuint orbit_vao;
    GLuint orbit_vbo;
    GLsizei orbit_vertex_count;
    unsigned int orbit_ring_count;
    uint16_t particles[];
} Atom;

Atom *atom_new(unsigned int amount_electron, unsigned int amount_proton, unsigned int amount_neutron);

void atom_update(Atom *atom, double dt);

void atom_render(Atom *atom, const Mesh *sphere, float time);

void atom_free(Atom *atom);

#endif //ATOM_MODEL_OF_BOHR_ATOM_H
