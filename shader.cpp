#include "shader.h"

std::string get_file_contents(const char* filename) {
    std::ifstream in(filename, std::ios::binary);
    if (in) {
        return std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    }
    throw std::system_error(errno, std::generic_category(), filename);
}

Shader::Shader(const char* vertexFile, const char* fragmentFile) {
    std::string vertexCode = get_file_contents(vertexFile);
    std::string fragmentCode = get_file_contents(fragmentFile);

    const char* vertexSrc = vertexCode.c_str();
    const char* fragmentSrc = fragmentCode.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSrc, NULL);
    glCompileShader(vertexShader);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR: Vertex Shader Compilation Failed" << infoLog << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSrc, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR: Fragment Shader Compilation Failed" << infoLog << std::endl;
    }

    ID = glCreateProgram();

    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        glGetProgramInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR: Shader Linking Failed" << infoLog << std::endl;
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