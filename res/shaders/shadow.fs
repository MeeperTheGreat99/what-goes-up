#version 330 core

in vec3 Position;

uniform vec3 light_pos;
uniform float far_plane;

void main() {
    gl_FragDepth = distance(Position, light_pos) / far_plane;
}