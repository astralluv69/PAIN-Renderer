#include "element.h"

Element::Element(std::string name, glm::vec2 startPos, glm::vec2 endPos, glm::vec3 bgColor, std::string text, GLuint textSize, glm::vec3 textColor) {
    Element::name = name;
    textElement = Text(text, glm::vec2(startPos.x + 5.0f, startPos.y), textColor, textSize);

    Vertex vertices[] = {
        Vertex{glm::vec3(startPos.x, startPos.y, 0.1f), bgColor},
        Vertex{glm::vec3(endPos.x,   startPos.y, 0.1f), bgColor},
        Vertex{glm::vec3(endPos.x,   endPos.y,   0.1f), bgColor},
        Vertex{glm::vec3(startPos.x, endPos.y,   0.1f), bgColor}
    };

    Element::verts = std::vector <Vertex>(vertices, vertices + sizeof(vertices) / sizeof(Vertex));

    GLuint indices[] = {
        0, 1, 2,
        0, 2, 3
    };

    Element::inds = std::vector<GLuint>(indices, indices + sizeof(indices) / sizeof(GLuint));

    elementVBO.Delete();
    elementEBO.Delete();

    VBO elementVBO(verts);
    EBO elementEBO(inds);

    elementVAO.Bind();
    elementVBO.Bind();
    elementEBO.Bind();

    elementVAO.LinkAttrib(elementVBO, 0, 3, GL_FLOAT, sizeof(Vertex), 0);
    elementVAO.LinkAttrib(elementVBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));

    elementVAO.Unbind();
    elementVBO.Unbind();
    elementEBO.Unbind();
}

void Element::Delete() {
    elementVAO.Delete();
    elementVBO.Delete();
    elementEBO.Delete();
    textElement.Delete();
    shader.Delete();
}

void Element::Draw(glm::mat4 proj) {
    glDisable(GL_DEPTH_TEST);

    shader.Activate();
    elementVAO.Bind();

    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "proj"), 1, GL_FALSE, glm::value_ptr(proj));

    glDrawElements(GL_TRIANGLES, inds.size(), GL_UNSIGNED_INT, 0);

    elementVAO.Unbind();
    shader.Deactivate();

    textElement.Draw(proj);

    glEnable(GL_DEPTH_TEST);
}