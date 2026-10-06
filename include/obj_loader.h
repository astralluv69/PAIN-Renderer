#ifndef OBJ_LOADER_CLASS_H
#define OBJ_LOADER_CLASS_H

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include "obj_mesh.h"

class OBJ_Load {
    public:
        std::vector<ObjVertex> verts;
        std::vector<GLuint> inds;

        ObjMesh mesh = ObjMesh(verts, inds);

        OBJ_Load(const char* filepath);

        void Draw(Shader& shader, glm::mat4 proj);
        void Delete();
    
    private:
        void parseVertex(std::string& vertexStr, const std::vector<float>& temp_positions, const std::vector<float>& temp_texCoords, 
            const std::vector<float>& temp_normals);
};

#endif