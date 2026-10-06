#version 330 core

in vec3 Position;

uniform samplerCube tex_albedo;

out vec4 FragColor;

void main() {
    FragColor = vec4(texture(tex_albedo, Position).rgb, 1.0);
}