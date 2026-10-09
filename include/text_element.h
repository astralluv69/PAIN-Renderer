#ifndef TEXT_ELEMENT_CLASS_H
#define TEXT_ELEMENT_CLASS_H

#include <map>
#include <vector>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include "shader.h"

extern "C" {
    #include <ft2build.h>
    #include FT_FREETYPE_H
}

struct Character {
    GLuint texID;
    glm::vec2 size;
    glm::vec2 offset;
    GLuint advance;
};

class Text {
    public: 
        std::map <char, Character> characters;

        std::string text;
        glm::vec2 position;
        GLuint size;
        glm::vec3 color;
        Shader textShader = Shader("shaders/text.vert", "shaders/text.frag");

        Text(std::string text, glm::vec2 position, glm::vec3 color, GLuint size);

        void Draw(glm::mat4 proj);
        void Delete();

    private: 
        void initText(const char* fontPath, GLuint fontSize);

        GLuint VAO, VBO;
};

#endif