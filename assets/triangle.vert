#version 430 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec3 aCol;

out vec3 vCol;

void main() {
    vCol = aCol;
    gl_Position = vec4(aPos, 0.0, 1.0);
}