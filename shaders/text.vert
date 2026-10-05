#version 330 core
layout(location=0) in vec4 aVertex;

out vec2 fTexCoords;

uniform mat4 proj;

void main() {
    fTexCoords = aVertex.zw;

    gl_Position = proj * vec4(aVertex.xy, 0.0f, 1.0f);
}