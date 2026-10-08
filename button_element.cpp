#include "button_element.h"

ButtonElement::ButtonElement(Element element, std::vector<ButtonElement>& buttons, std::function<void()> onPress, std::function<void()> onHover) {
    parentElement = element;
    buttons.push_back(*this);
    ButtonElement::onPress = onPress;
    ButtonElement::onHover = onHover;
}

ButtonElement::ButtonElement(std::string name, glm::vec2 startPos, glm::vec2 endPos, glm::vec3 bgColor, glm::vec3 hoverColor, std::vector<ButtonElement>& buttons, 
        std::function<void()> onPress, std::function<void()> onHover, std::string text, GLuint textSize, glm::vec3 textColor): 
            Element(name, startPos, endPos, bgColor, text, textSize, textColor), onHover(std::move(onHover)), onPress(std::move(onPress)) {
    parentElement = Element(name, startPos, endPos, bgColor, text, textSize, textColor);
    hoverElement = Element(name, startPos, endPos, hoverColor, text, textSize, textColor);
    activeElement = parentElement;
    buttons.push_back(*this);
    ButtonElement::startPos = startPos;
    ButtonElement::endPos = endPos;
}

void ButtonElement::update(GLFWwindow* window, double dt) {
    double cursorPosX, cursorPosY;
    glfwGetCursorPos(window, &cursorPosX, &cursorPosY);

    if((cursorPosX >= startPos.x && cursorPosX <= endPos.x) && (cursorPosY >= startPos.y && cursorPosY <= endPos.y)) {
        activeElement = hoverElement;
        if(!isHovering) {
            isHovering = true;
            std::cout << "Attempting to run hover func" << std::endl;
            onHover();
        }
    } else if(((cursorPosX < startPos.x && cursorPosX > endPos.x) && (cursorPosY < startPos.y && cursorPosY > endPos.y)) && isHovering) {
        std::cout << "resetting hover var" << std::endl;
        activeElement = parentElement;
        isHovering = false;
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

void ButtonElement::Draw(glm::mat4 proj) {
    activeElement.Draw(proj);
}