#include "ui_element.h"

UiElement::UiElement(std::string name) {
    UiElement::name = name;
}

void UiElement::AddElement(Element elementToAdd) {
    elements.push_back(elementToAdd);
}

void UiElement::Delete() {
    for (Element element : elements) {
        element.Delete();
    }
}

void UiElement::Draw(glm::mat4 proj) {
    for (Element element : elements) {
        element.Draw(proj);
    }
}