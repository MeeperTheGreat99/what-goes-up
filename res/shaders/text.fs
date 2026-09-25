#version 330 core

in vec2 Coord;

uniform vec4 color;
uniform sampler2D sdftextie;

out vec4 FragColor;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main() {
    vec3 sdf = texture(sdftextie, Coord).rgb;
    float distance = median(sdf.r, sdf.g, sdf.b);
    if (distance < 0.5) {
        discard;
    }
    FragColor = color;
}