#version 430 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec3 aCol;
layout(location = 2) in mat3 iModel;

out vec3 vCol;

void main() {
    vec3 final = iModel * vec3(aPos, 1.0);

    vCol = aCol;
    gl_Position = vec4(final.xy, 0.0, 1.0);
}