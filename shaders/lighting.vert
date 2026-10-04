#version 330 core

layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aColor;
layout(location=2) in vec3 aNormal;

out vec3 fCurrentPos;
out vec3 fColor;
out vec3 fNormal;

uniform mat4 camMatrix;
uniform mat4 model;

void main() {
    fCurrentPos = vec3(model * vec4(aPos, 1.0f));
    fColor = aColor;
    fNormal = aNormal;

    gl_Position = camMatrix * model * vec4(aPos, 1.0f);
}