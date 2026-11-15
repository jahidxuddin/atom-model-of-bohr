#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 3) in uint aInfo;
layout(location = 4) in uint aOrbitLine;

layout(location = 0) uniform mat4 uView;
layout(location = 1) uniform mat4 uProjection;
layout(location = 2) uniform float uTime;

out vec3 vNormal;
flat out uint vType;
flat out uint vOrbitLine;

const float PI = 3.14159265359;

// Hilfsfunktion: 3D-Rotation um die Z-Achse
vec3 rotateZ(vec3 p, float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return vec3(p.x * c - p.y * s, p.x * s + p.y * c, p.z);
}

// Pseudozufall für feste Nukleon-Positionen im Kern aus dem 8-Bit-Seed
vec3 nucleusOffset(uint seed) {
    vec3 p = fract(vec3(float(seed)) * vec3(0.1031, 0.1030, 0.0973));
    p += dot(p, p.yxz + 33.33);
    vec3 rnd = fract((p.xxy + p.yxx) * p.zyx) * 2.0 - 1.0;
    return normalize(rnd) * 0.11; // Kompakter Kern-Cluster
}

void main() {
    // 1. Bitfield entpacken
    uint type  =  aInfo        & 0x03u; // Bits 0-1
    uint shell = (aInfo >> 2u) & 0x07u; // Bits 2-4 (n = 1..7)
    uint tilt  = (aInfo >> 5u) & 0x07u; // Bits 5-7 (0..7)
    uint phase = (aInfo >> 8u) & 0xFFu; // Bits 8-15 (0..255)

    vType = type;
    vOrbitLine = aOrbitLine;
    vNormal = aNormal;

    if (aOrbitLine != 0u) {
        gl_Position = uProjection * uView * vec4(aPos, 1.0);
        return;
    }

    vec3 worldOffset = vec3(0.0);
    float scale = 1.0;

    if (type == 3u) {
        // --- ELEKTRON (Bohrsche Formeln) ---
        float n = float(shell);

        // Bohr-Radius: r_n = r_1 * n^2 (für bessere Sichtbarkeit oft auch linear: r_1 * n)
        float radius = 0.25 * (n * n);

        // Bohr-Geschwindigkeit: v_n = v_1 / n  => Winkelgeschwindigkeit omega = v / r ~ 1 / n^3
        float speed = 3.0 / (n * n * n);

        float startAngle = (float(phase) / 256.0) * 2.0 * PI;
        float angle = startAngle + uTime * speed;

        // Kreisbahn in XZ-Ebene
        vec3 orbitPos = vec3(cos(angle) * radius, 0.0, sin(angle) * radius);

        // Bahn-Neigung anwenden
        float tiltAngle = (float(tilt) / 8.0) * PI;
        worldOffset = rotateZ(orbitPos, tiltAngle);

        scale = 0.03; // Elektronen sind kleiner als Nukleonen
    } else {
        // --- PROTON ODER NEUTRON (Kern) ---
        worldOffset = nucleusOffset(phase);
        scale = 0.05;
    }

    vec3 worldPos = (aPos * scale) + worldOffset;
    gl_Position = uProjection * uView * vec4(worldPos, 1.0);
}
