#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inCoord;

uniform mat4 Proj;
uniform mat4 View;
uniform mat4 Model;

out vec3 Position;
out vec3 Normal;
out vec2 Coord;

void main() {
    mat4 PVM = Proj * View * Model;
    Position = vec3(Model * vec4(inPosition, 1.0));
    Normal = vec3(Model * vec4(inNormal, 1.0));
    Coord = inCoord;
    gl_Position = PVM * vec4(inPosition, 1.0);
}