#ifndef TEXT_ELEMENT_CLASS_H
#define TEXT_ELEMENT_CLASS_H

#include <map>
#include <vector>
#include <string>
#include <ft2build.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include "shader.h"

#include FT_FREETYPE_H

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
        float scale;

        Text(std::string text, glm::vec2 position, float scale);

        void Draw(Shader& shader, glm::vec3 color, glm::mat4 proj);

    private: 
        void initText(const char* fontPath, GLuint fontSize);
};

#endif