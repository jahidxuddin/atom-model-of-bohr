#version 460 core

in vec3 vNormal;
flat in uint vType;
flat in uint vOrbitLine;

out vec4 FragColor;

void main() {
    if (vOrbitLine != 0u) {
        FragColor = vec4(0.34, 0.43, 0.54, 0.62);
        return;
    }

    vec3 color = vec3(1.0);
    if (vType == 1u)      color = vec3(0.9, 0.2, 0.2); // Proton: Rot
    else if (vType == 2u) color = vec3(0.6, 0.6, 0.6); // Neutron: Grau
    else if (vType == 3u) color = vec3(0.2, 0.6, 1.0); // Elektron: Blau

    float diff = max(dot(normalize(vNormal), normalize(vec3(1.0, 1.0, 1.0))), 0.2);
    FragColor = vec4(color * diff, 1.0);
}
