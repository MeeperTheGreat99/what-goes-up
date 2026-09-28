#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inCoord;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in vec3 inBitangent;

uniform mat4 Proj;
uniform mat4 View;
uniform mat4 Model;

out vec3 Position;
out vec3 Normal;
out vec2 Coord;
out mat3 TBN;

void main() {
    mat4 PVM = Proj * View * Model;

    mat3 normalMatrix = transpose(inverse(mat3(Model)));
    TBN = mat3(
        normalize(normalMatrix * inTangent),
        normalize(normalMatrix * inBitangent),
        normalize(normalMatrix * inNormal)
    );

    Position = vec3(Model * vec4(inPosition, 1.0));
    Normal = TBN[2];
    Coord = inCoord;
    gl_Position = PVM * vec4(inPosition, 1.0);
}