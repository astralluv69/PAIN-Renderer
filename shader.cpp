#include "shader.h"

Shader::Shader(const unsigned char* vertexFile, const unsigned char* fragmentFile) {
    const GLchar* vertexSrc = reinterpret_cast<const GLchar*>(vertexFile);
    const GLchar* fragmentSrc = reinterpret_cast<const GLchar*>(fragmentFile);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSrc, nullptr);
    glCompileShader(vertexShader);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR: Vertex Shader Compilation Failed" << infoLog << std::endl;
        return;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSrc, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR: Fragment Shader Compilation Failed" << infoLog << std::endl;
        return;
    }

    ID = glCreateProgram();

    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cerr << "ERROR: Shader Linking Failed\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::loadLightColorAndPos(glm::vec3 lightColor, glm::vec3 lightPos) {
    (*this).Activate();
    glUniform3f(glGetUniformLocation(ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
    glUniform4f(glGetUniformLocation(ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, 1.0f);
    (*this).Deactivate();
}

void Shader::Activate() {
    glUseProgram(ID);
}

void Shader::Deactivate() {
    glUseProgram(0);
}

void Shader::Delete() {
    glDeleteProgram(ID);
}