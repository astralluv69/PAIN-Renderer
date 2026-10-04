#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include <string>

#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include "camera.h"

class Mesh {
    public:
        std::vector <Vertex> meshVertices;
        std::vector <GLuint> meshIndices;
        
        VAO meshVAO;
        VBO meshVBO;
        EBO meshEBO;

        Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices);

        void Draw(Shader& shader, Camera& camera, glm::mat4 model);
        void DrawWireframe(Shader& shader, Camera& camera, glm::mat4 model);
        void Delete();
};

#endif