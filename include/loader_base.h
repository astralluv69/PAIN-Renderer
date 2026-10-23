#ifndef LOADER_BASE_H
#define LOADER_BASE_H

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

#include "vbo.h"

struct FaceVertex {
    int vIdx, vtIdx, vnIdx;
};

struct Triangle {
    ObjVertex v1;
    ObjVertex v2;
    ObjVertex v3;
};

struct ObjVertexComparator {
    bool operator()(const ObjVertex& a, const ObjVertex& b) const {
        if (a.position.x != b.position.x) return a.position.x < b.position.x;
        if (a.position.y != b.position.y) return a.position.y < b.position.y;
        if (a.position.z != b.position.z) return a.position.z < b.position.z;

        return false;
    };
};

struct StlVertexComparator {
    bool operator()(const ObjVertex& a, const ObjVertex& b) const {
        if (a.position.x != b.position.x) return a.position.x < b.position.x;
        if (a.position.y != b.position.y) return a.position.y < b.position.y;
        if (a.position.z != b.position.z) return a.position.z < b.position.z;
        if (a.normal.x != b.normal.x) return a.normal.x < b.normal.x;
        if (a.normal.y != b.normal.y) return a.normal.y < b.normal.y;

        return a.normal.z < b.normal.z;
    };
};

#endif