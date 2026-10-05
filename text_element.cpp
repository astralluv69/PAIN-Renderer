#include "text_element.h"

GLuint textIndices[] = {
    0, 1, 2,
    0, 2, 3
};

std::vector <GLuint> textInd(textIndices, textIndices + sizeof(textIndices) / sizeof(GLuint));

Text::Text(std::string text, glm::vec2 position, float scale) {
    initText("fonts/default.ttf", 10);

    Text::text = text;
    Text::position = position;
    Text::scale = scale;
}

void Text::Draw(Shader& shader, glm::vec3 color, glm::mat4 proj) {
    float originalX = position.x;

    shader.Activate();
    glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniform1i(glGetUniformLocation(shader.ID, "text"), 0);

    for (std::string::const_iterator c = text.begin(); c != text.end(); c++) {
        Character character = characters[*c];

        float xPos = position.x + character.offset.x * scale;
        float yPos = position.y + character.offset.y * scale;

        float w = character.size.x * scale;
        float h = character.size.y * scale;

        TextVertex vertices[] = {
            TextVertex{glm::vec2(xPos, yPos),         glm::vec2(0.0f, 1.0f)},
            TextVertex{glm::vec2(xPos, yPos + h),     glm::vec2(0.0f, 0.0f)},
            TextVertex{glm::vec2(xPos + w, yPos + h), glm::vec2(1.0f, 1.0f)},
            TextVertex{glm::vec2(xPos + w, yPos),     glm::vec2(1.0f, 0.0f)}
        };

        std::vector <TextVertex> textVerts(vertices, vertices + sizeof(vertices) / sizeof(TextVertex));

        VAO textVAO;

        VBO textVBO(textVerts);
        EBO textEBO(textInd);

        textVAO.Bind();
        textVBO.Bind();
        textEBO.Bind();

        textVAO.LinkAttrib(textVBO, 0, 2, GL_FLOAT, sizeof(TextVertex), 0);
        textVAO.LinkAttrib(textVBO, 1, 2, GL_FLOAT, sizeof(TextVertex), (void*)(2 * sizeof(float)));

        textVAO.Unbind();
        textVBO.Unbind();
        textEBO.Unbind();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        textVAO.Bind();
        glBindTexture(GL_TEXTURE_2D, character.texID);

        glDrawElements(GL_TRIANGLES, textInd.size(), GL_UNSIGNED_INT, 0);

        glBindTexture(GL_TEXTURE_2D, 0);
        textVAO.Unbind();

        glDisable(GL_BLEND);

        position.x += (character.advance >> 6) * scale;
    }

    shader.Deactivate();

    position.x = originalX;
}

void Text::initText(const char* fontPath, GLuint fontSize) {
    FT_Library freetype;
    FT_Face face;

    FT_Init_FreeType(&freetype);
    FT_New_Face(freetype, fontPath, 0, &face);

    FT_Set_Pixel_Sizes(face, 0, fontSize);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            continue;
        }

        GLuint texture;
        
        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 
            0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            glm::vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::vec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<GLuint>(face->glyph->advance.x)
        };

        characters.insert(std::pair<char, Character>(c, character));
    }
}