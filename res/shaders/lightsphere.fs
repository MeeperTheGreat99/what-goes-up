#version 330 core

uniform vec2 resolution;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform samplerCube shadow_tex;
uniform vec3 light_position;
uniform vec3 light_color;
uniform float light_far;

out vec4 FragColor;

void main() {
    vec2 coord = gl_FragCoord.xy / resolution;
    vec3 delta = texture(gPosition, coord).xyz - light_position;
    vec3 normal = texture(gNormal, coord).xyz;
    vec3 toLight = normalize(-delta);
    float lambert = max(dot(normal, toLight), 0.0);
    float closestDepth = texture(shadow_tex, delta).r * light_far;
    float currentDepth = length(delta);
    float bias = max(0.1 * (1.0 - dot(normal, toLight)), 0.005);
    float shadow = currentDepth - bias > closestDepth ? 0.0 : 1.0;

    FragColor = vec4(light_color / dot(delta, delta) * lambert * shadow, 1.0);
}