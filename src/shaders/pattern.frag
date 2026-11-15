#version 460 core

in vec2 vPos;

uniform float uTime;

out vec4 FragColor;

void main() {
    // vPos von [-1, 1] auf [0, 1] umrechnen
    vec2 uv = vPos * 0.5 + 0.5;

    // Animierte Farben basierend auf Position und Zeit
    float r = 0.5 + 0.5 * sin(uTime + uv.x * 3.0);
    float g = 0.5 + 0.5 * sin(uTime + uv.y * 3.0 + 2.0);
    float b = 0.5 + 0.5 * sin(uTime + (uv.x + uv.y) * 3.0 + 4.0);

    FragColor = vec4(r, g, b, 1.0);
}