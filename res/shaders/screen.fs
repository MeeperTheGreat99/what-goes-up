#version 330 core

in vec2 Coord;

uniform sampler2D textie;

out vec4 FragColor;

void main() {
    const float gamma = 2.2;
    vec3 color = texture(textie, Coord).rgb;
    vec3 mapped = color / (color + vec3(1.0));
    mapped = pow(mapped, vec3(1.0 / gamma));
    FragColor = vec4(mapped, 1.0);
}