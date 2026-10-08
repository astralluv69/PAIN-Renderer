#include "button_element.h"

ButtonElement::ButtonElement(Element element, std::vector<ButtonElement>& buttons, std::function<void()> onPress, std::function<void()> onHover):
    Element(element) {
    parentElement = element;
    buttons.push_back(*this);
    ButtonElement::onPress = onPress;
    ButtonElement::onHover = onHover;
}

ButtonElement::ButtonElement(std::string name, glm::vec2 startPos, glm::vec2 endPos, glm::vec3 bgColor, glm::vec3 hoverColor, std::vector<ButtonElement>& buttons, 
        std::function<void()> onPress, std::function<void()> onHover, std::string text, GLuint textSize, glm::vec3 textColor): 
            Element(name, startPos, endPos, bgColor, text, textSize, textColor), onHover(std::move(onHover)), onPress(std::move(onPress)) {
    parentElement = Element("parentElement", startPos, endPos, bgColor, text, textSize, textColor);
    hoverElement = Element("hoverElement", startPos, endPos, hoverColor, text, textSize, textColor);
    activeElement = parentElement;
    buttons.push_back(*this);
    ButtonElement::startPos = startPos;
    ButtonElement::endPos = endPos;
}

void ButtonElement::update(GLFWwindow* window, double dt) {
    double cursorPosX, cursorPosY;
    glfwGetCursorPos(window, &cursorPosX, &cursorPosY);

    static bool isHovering = false;

    if((cursorPosX >= startPos.x && cursorPosX <= endPos.x) && (cursorPosY >= startPos.y && cursorPosY <= endPos.y)) {
        if(!isHovering) {
            isHovering = true;
            onHover();
        }
        activeElement = hoverElement;
    } else {
        if(isHovering) {
            activeElement = parentElement;
            isHovering = false;
        }
    }

    if((cursorPosX >= startPos.x && cursorPosX <= endPos.x) && (cursorPosY >= startPos.y && cursorPosY <= endPos.y)) {
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
}