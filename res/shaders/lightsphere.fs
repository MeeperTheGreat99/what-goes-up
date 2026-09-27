#version 330 core

uniform vec2 resolution;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform vec3 light_position;
uniform vec3 light_color;

out vec4 FragColor;

void main() {
    vec2 coord = gl_FragCoord.xy / resolution;
    vec3 delta = texture(gPosition, coord).xyz - light_position;
    float lambert = max(dot(texture(gNormal, coord).xyz, normalize(-delta)), 0.0);
    FragColor = vec4(light_color / dot(delta, delta) * lambert, 1.0);
}