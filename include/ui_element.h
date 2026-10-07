#ifndef UI_ELEMENT_CLASS_H
#define UI_ELEMENT_CLASS_H

#include <string>
#include <initializer_list>
#include <optional>

#include "element.h"
#include "button_element.h"

class UiElement {
    public:
        std::string name;
        std::vector <Element> elements;

        UiElement(std::string name);

        void AddElement(Element elementToAdd);
        void Draw(glm::mat4 proj);
        void Delete();
};

#endif