#ifndef OBJ_MESH_CLASS_H
#define OBJ_MESH_CLASS_H

#include <string>

#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include "camera.h"
#include "obj_loader.h"

class ObjMesh {
    public:
        std::vector <ObjVertex> meshVertices;
        std::vector <GLuint> meshIndices;
        
        VAO meshVAO;
        VBO meshVBO = VBO(meshVertices);
        EBO meshEBO = EBO(meshIndices);

        ObjMesh(const char* filepath);

        void Draw(Shader& shader, Camera& camera, glm::mat4 model);
        void DrawWireframe(Shader& shader, Camera& camera, glm::mat4 model);
        void Delete();
};

#endif