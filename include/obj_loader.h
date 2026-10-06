#ifndef OBJ_LOADER_CLASS_H
#define OBJ_LOADER_CLASS_H

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

#include "obj_mesh.h"

struct FaceVertex {
    int vIdx, vtIdx, vnIdx;
};

struct VertexComparator {
    bool operator()(const ObjVertex& a, const ObjVertex& b) const {
        if (a.position.x != b.position.x) return a.position.x < b.position.x;
        if (a.position.y != b.position.y) return a.position.y < b.position.y;
        if (a.position.z != b.position.z) return a.position.z < b.position.z;

        return false;
    };
};

class OBJ_Load {
    public:
        const char* filepath;

        OBJ_Load(const char* filepath);

        void loadObj(std::vector<ObjVertex>& vertices, std::vector<GLuint>& indices);
};

#endif