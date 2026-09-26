#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;

uniform mat4 Proj;
uniform mat4 View;

out vec3 Color;

void main() {
    Color = inColor;
    gl_Position = Proj * View * vec4(inPosition, 1.0);
}