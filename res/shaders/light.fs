#version 330 core

in vec2 Coord;

uniform vec3 ViewPos;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gReflectivity;
uniform sampler2D gLight;
uniform samplerCube tex_reflection;

out vec4 FragColor;

void main() {
    vec3 fragPos = texture(gPosition, Coord).rgb;
    vec3 normal = texture(gNormal, Coord).rgb;
    vec4 albedoSpec = texture(gAlbedoSpec, Coord);
    vec3 light = texture(gLight, Coord).rgb;
    vec3 diffuse = albedoSpec.rgb * light;
    vec3 reflected = texture(tex_reflection, reflect(normalize(fragPos - ViewPos), normal)).rgb * length(light);
    vec3 final = mix(diffuse, reflected, texture(gReflectivity, Coord).r);
    FragColor = vec4(final, 1.0);
}