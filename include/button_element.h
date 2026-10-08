#ifndef BUTTON_ELEMENT_CLASS_H
#define BUTTON_ELEMENT_CLASS_H

#include <functional>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "element.h"

class ButtonElement: public Element {
    public:
        bool isHovering = false;
        bool firstClick = true;
        std::function<void()> onPress = [](){};
        std::function<void()> onHover = [](){};

        Element parentElement, hoverElement, activeElement;

        ButtonElement(std::string name, glm::vec2 startPos, glm::vec2 endPos, glm::vec3 bgColor, glm::vec3 hoverColor, std::vector<ButtonElement>& buttons, 
            std::function<void()> onPress, std::function<void()> onHover, std::string text = "", GLuint textSize = 0, glm::vec3 textColor = glm::vec3(1.0f, 1.0f, 1.0f));
        ButtonElement(Element element, std::vector<ButtonElement>& buttons, std::function<void()> onPress, std::function<void()> onHover);

        void update(GLFWwindow* window, double dt);
        void Draw(glm::mat4 proj) override;
};

#endif