#ifndef STL_LOADER_CLASS_H
#define STL_LOADER_CLASS_H

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

class STL_Load {
    public:
        const char* filepath;

        STL_Load(const char* filepath);

        void loadStl(std::vector<ObjVertex>& vertices, std::vector<GLuint>& indices, bool& willClip);
};

#endif