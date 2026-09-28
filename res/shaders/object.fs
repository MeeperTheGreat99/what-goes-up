#version 330 core

in vec3 Position;
in vec3 Normal;
in vec2 Coord;
in mat3 TBN;

uniform sampler2D tex_albedo;
uniform vec3 albedo;
uniform sampler2D tex_normal;
uniform bool tex_normal_present;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

void main() {
    gPosition = Position;
    if (tex_normal_present) {
        vec3 normal = texture(tex_normal, Coord).rgb * 2.0 - 1.0;
        normal.x = -normal.x;
        gNormal = normalize(TBN * normal);
    } else {
        gNormal = Normal;
    }
    gAlbedoSpec = vec4(texture(tex_albedo, Coord).rgb * albedo, 0.5);
}