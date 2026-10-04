#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>
#include <vector>
#include <glm/glm.hpp>

std::string get_file_contents(const char* filename);

class Shader {
    public: 
        GLuint ID;
        
        Shader(const char* vertexFile, const char* fragmentFile);

        void Activate();
        void Deactivate();
        void Delete();
        void loadLightColorAndPos(glm::vec3 lightColor, glm::vec3 lightPos);
};

#endif