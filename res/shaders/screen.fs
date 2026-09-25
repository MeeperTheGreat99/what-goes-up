#version 330 core

in vec2 Coord;

uniform sampler2D textie;

out vec4 FragColor;

void main() {
    FragColor = vec4(texture(textie, Coord).rgb, 1.0);
}