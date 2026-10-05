#version 330 core
layout(location=0) in vec2 aPos;
layout(location=0) in vec2 aTexCoords;

out vec2 fTexCoords;

uniform mat4 proj;

void main() {
    fTexCoords = aTexCoords;

    gl_Position = proj * vec4(aPos, 0.0f, 1.0f);
}