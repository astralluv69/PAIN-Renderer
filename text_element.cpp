#include "text_element.h"

Text::Text(std::string text, glm::vec2 position, glm::vec3 color, GLuint size) {
    initText("fonts/default.ttf", size);

    Text::text = text;
    Text::position = position;
    Text::size = size;
    Text::color = color;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Text::Draw(glm::mat4 proj) {
    float originalX = position.x;

    textShader.Activate();
    glUniform3f(glGetUniformLocation(textShader.ID, "textColor"), color.x, color.y, color.z);
    glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniform1i(glGetUniformLocation(textShader.ID, "text"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (std::string::const_iterator c = text.begin(); c != text.end(); c++) {
        Character character = characters[*c];

        float xPos = position.x + character.offset.x;
        float yPos = position.y - character.offset.y + size;

        float w = character.size.x;
        float h = character.size.y;

        float vertices[6][4] = {
            { xPos,     yPos,       0.0f,  0.0f }, // Top-Left of upside-down quad
            { xPos + w, yPos,       1.0f,  0.0f }, // Top-Right of upside-down quad
            { xPos,     yPos + h,   0.0f,  1.0f }, // Bottom-Left of upside-down quad

            { xPos + w, yPos,       1.0f,  0.0f }, 
            { xPos + w, yPos + h,   1.0f,  1.0f }, // Bottom-Right of upside-down quad
            { xPos,     yPos + h,   0.0f,  1.0f }
        };

        glBindTexture(GL_TEXTURE_2D, character.texID);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        position.x += (character.advance >> 6);
    }

    glDisable(GL_BLEND);

    textShader.Deactivate();

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
            std::cout << "ERROR: Freetype failed to load Glyph " << c << std::endl;
            continue;
        }

        GLuint texture;
        
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D, 
            0, 
            GL_RED, 
            face->glyph->bitmap.width, 
            face->glyph->bitmap.rows, 
            0, 
            GL_RED, 
            GL_UNSIGNED_BYTE, 
            face->glyph->bitmap.buffer
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            glm::vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::vec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };

        characters.insert(std::pair<char, Character>(c, character));
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

void Text::Delete() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    textShader.Delete();
}