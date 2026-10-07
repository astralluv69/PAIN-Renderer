#ifndef ELEMENT_CLASS_H
#define ELEMENT_CLASS_H

#include <vector>
#include <string>

#include "text_element.h"

class Element {
    public:
        Shader shader = Shader("shaders/elementDefault.vert", "shaders/elementDefault.frag");
        VAO elementVAO;
        VBO elementVBO = VBO(std::vector<Vertex> {});
        EBO elementEBO = EBO(std::vector<GLuint> {});
        std::vector<Vertex> verts;
        std::vector<GLuint> inds;

        std::string name;
        Text textElement = Text("", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0);
        glm::vec2 startPos;
        glm::vec2 endPos;

        Element();

        Element(std::string name, glm::vec2 startPos, glm::vec2 endPos, glm::vec3 bgColor, std::string text = "", GLuint textSize = 0, 
            glm::vec3 textColor = glm::vec3(1.0f, 1.0f, 1.0f));

        void Draw(glm::mat4 proj);
        void Delete();
};

#endif