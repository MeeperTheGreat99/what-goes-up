#version 330 core

in vec3 Position;
in vec3 Normal;
in vec2 Coord;

uniform sampler2D tex_grass;
uniform sampler2D tex_dirt;
uniform sampler2D tex_rock;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

void main() {
    gPosition = Position;
    gNormal = normalize(Normal);

    vec3 color;
    float height = Position.y / TERRAIN_DEPTH;
    if (height > 0.7) {
        color = mix(texture(tex_grass, Coord).rgb, texture(tex_rock, Coord).rgb, (height - 0.7) / 0.3);
    } else if (height > 0.25) {
        color = mix(texture(tex_dirt, Coord).rgb, texture(tex_grass, Coord).rgb, (height - 0.25) / 0.45);
    } else {
        color = texture(tex_dirt, Coord).rgb;
    }
    float labert = max(0, dot(normalize(vec3(1, 1, 0.5)), gNormal));
    gAlbedoSpec = vec4(color * labert, 0.5);
}