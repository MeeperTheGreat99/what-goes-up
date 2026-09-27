#version 330 core

in vec2 Coord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};

uniform Light lights[32];

out vec4 FragColor;

void main() {
    vec3 fragPos = texture(gPosition, Coord).rgb;
    vec3 normal = normalize(texture(gNormal, Coord).rgb);
    vec4 albedoSpec = texture(gAlbedoSpec, Coord);
    vec3 totalLight = vec3(0.0);
    for (int i = 0; i < 32; ++i) {
        Light light = lights[i];
        vec3 lightDir = normalize(light.position - fragPos);
        float distance2 = dot(light.position - fragPos, light.position - fragPos);
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = light.color * light.intensity / distance2;
        totalLight += diffuse * albedoSpec.rgb;
    }
    FragColor = vec4(totalLight, 1.0);
}