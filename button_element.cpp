#include "button_element.h"

ButtonElement::ButtonElement(Element element, std::function<void()> onPress, std::function<void()> onHover):
    Element(element) {
    parentElement = element;
    activeElement = parentElement;
    this->onPress = std::move(onPress);
    this->onHover = std::move(onHover);
}

ButtonElement::ButtonElement(std::string name, glm::vec2 startPos, glm::vec2 endPos, glm::vec3 bgColor, glm::vec3 hoverColor,
        std::function<void()> onPress, std::function<void()> onHover, std::string text, GLuint textSize, glm::vec3 textColor): 
            Element(name, startPos, endPos, bgColor, text, textSize, textColor), onHover(std::move(onHover)), onPress(std::move(onPress)) {
    this->parentElement = Element("parentElement", startPos, endPos, bgColor, text, textSize, textColor);
    this->hoverElement = Element("hoverElement", startPos, endPos, hoverColor, text, textSize, textColor);
    this->activeElement = parentElement;
    ButtonElement::startPos = startPos;
    ButtonElement::endPos = endPos;
}

void ButtonElement::update(GLFWwindow* window, double dt) {
    double cursorPosX, cursorPosY;
    glfwGetCursorPos(window, &cursorPosX, &cursorPosY);

    const bool hovering = (cursorPosX >= startPos.x && cursorPosX <= endPos.x) &&
                          (cursorPosY >= startPos.y && cursorPosY <= endPos.y);
    if (hovering) {
        if(!isHovering) {
            onHover();
        }
    }
    isHovering = hovering;

    if (hovering) {
        if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            if (firstClick) {
                onPress();
                firstClick = false;
            }
        }
        
        if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
            firstClick = true;
        }
    }

    activeElement = isHovering ? hoverElement : parentElement;
}

void ButtonElement::Draw(glm::mat4 proj) {
    activeElement.Draw(proj);
}