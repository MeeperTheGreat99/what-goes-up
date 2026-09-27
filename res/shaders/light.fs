#version 330 core

in vec2 Coord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gLight;

out vec4 FragColor;

void main() {
    // vec3 fragPos = texture(gPosition, Coord).rgb;
    // vec3 normal = texture(gNormal, Coord).rgb;
    vec4 albedoSpec = texture(gAlbedoSpec, Coord);
    vec3 light = texture(gLight, Coord).rgb;
    FragColor = vec4(albedoSpec.rgb * light, 1.0);
}