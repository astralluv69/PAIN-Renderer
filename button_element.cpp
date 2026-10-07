#include "button_element.h"

ButtonElement::ButtonElement(Element element, std::vector<ButtonElement>& buttons, std::function<void()> onPress) {
    parentElement = element;
    buttons.push_back(*this);
    ButtonElement::onPress = onPress;
}

ButtonElement::ButtonElement(std::string name, glm::vec2 startPos, glm::vec2 endPos, glm::vec3 bgColor, std::vector<ButtonElement>& buttons, 
        std::function<void()> onPress, std::string text, GLuint textSize, glm::vec3 textColor) {
    glm::vec3 hoverColor = glm::vec3(

    );
    
    parentElement = Element(name, startPos, endPos, bgColor, text, textSize, textColor);
    hoverElement = Element(name, startPos, endPos, hoverColor, text, textSize, textColor);
    activeElement = parentElement;
    buttons.push_back(*this);
    ButtonElement::onPress = onPress;
    ButtonElement::startPos = startPos;
    ButtonElement::endPos = endPos;
}

void ButtonElement::update(GLFWwindow* window, double dt) {
    double cursorPosX, cursorPosY;
    glfwGetCursorPos(window, &cursorPosX, &cursorPosY);

    if((cursorPosX >= startPos.x && cursorPosX <= endPos.x) && (cursorPosY >= startPos.y && cursorPosY <= endPos.y)) {
        activeElement = hoverElement;
    } else {
        activeElement = parentElement;
    }

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if ((cursorPosX >= startPos.x && cursorPosX <= endPos.x) && (cursorPosY >= startPos.y && cursorPosY <= endPos.y)) {
            onPress();
        }
    }
}

void ButtonElement::Draw(glm::mat4 proj) {
    activeElement.Draw(proj);
}