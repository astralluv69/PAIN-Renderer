#version 330 core

in vec3 fCurrentPos;
in vec3 fColor;
in vec3 fNormal;

out vec4 color;

uniform vec4 lightColor;
uniform vec3 lightPos;

void main() {
    vec3 lightVec = lightPos - fCurrentPos;

    float dist = length(lightVec);
    float a = 0.1f;
    float b = 0.45f;
    float intensity = 1.0f / (a * dist * dist + b * dist * 1.0f);

    float ambient = 0.1f;

    vec3 normal = normalize(fNormal);
    vec3 lightDirection = normalize(lightVec);
    float diffuse = max(dot(normal, lightDirection), 0.0f);

    color = (vec4(fColor, 1.0f) * (diffuse * intensity + ambient)) * lightColor;
}