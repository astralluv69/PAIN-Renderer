#include "ui_element.h"

UiElement::UiElement(std::string name) {
    UiElement::name = name;
    elementDepth = 0.1f;
}

void UiElement::AddElement(Element elementToAdd) {
    baseElements.push_back(elementToAdd);
}

void UiElement::AddElement(ButtonElement elementToAdd) {
    buttons.push_back(elementToAdd);
}

void UiElement::Delete() {
    for (Element element : baseElements) {
        element.Delete();
    }
    for (ButtonElement button : buttons) {
        button.Delete();
    }
}

void UiElement::Draw(glm::mat4 proj) {
    for (Element element : baseElements) {
        element.Draw(proj);
    }
    for (ButtonElement button : buttons) {
        button.Draw(proj);
    }
}