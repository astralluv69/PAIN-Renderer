#version 330 core

in vec3 fCurrentPos;
in vec3 fNormal;

layout(location=0) out vec4 color;

uniform vec4 lightColor;
uniform vec3 lightPos;


vec4 lighting() {
    float ambient = 0.1f;

    vec3 normal = normalize(fNormal);
    vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 0.0f));
    float diffuse = max(dot(normal, lightDirection), 0.0f);

    return (vec4(0.0f, 0.88f, 0.88f, 1.0f) * (diffuse + ambient)) * lightColor;
}

void main() {
    color = lighting();
}