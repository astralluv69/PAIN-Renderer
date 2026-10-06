#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 normal;
};

struct TextVertex {
    glm::vec2 position;
    glm::vec2 texUV;
};

class VBO {
    public:
        GLuint ID;
        
        VBO(const std::vector <Vertex>& vertices);
        VBO(std::vector <TextVertex>& vertices);

        void Bind();
        void Unbind();
        void Delete();
};

#endif