#version 330 core

layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec2 inCoord;

uniform vec2 position;
uniform vec2 scale;

out vec2 Coord;

void main() {
    Coord = inCoord;
    gl_Position = vec4((inPosition + vec2(1.0, -1.0)) * scale + position, 0.0, 1.0);
}