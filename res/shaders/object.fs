#version 330 core

in vec3 Position;
in vec3 Normal;
in vec2 Coord;

uniform sampler2D tex_albedo;
uniform vec3 albedo;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

void main() {
    gPosition = Position;
    gNormal = normalize(Normal);
    gAlbedoSpec = vec4(texture(tex_albedo, Coord).rgb * albedo, 0.5);
}