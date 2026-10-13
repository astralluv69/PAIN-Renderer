#ifndef OBJ_LOADER_CLASS_H
#define OBJ_LOADER_CLASS_H

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include "camera.h"
#include "loader_base.h"

class OBJ_Load {
    public:
        const char* filepath;

        OBJ_Load(const char* filepath);

        void loadObj(std::vector<ObjVertex>& vertices, std::vector<GLuint>& indices);
};

#endif