#ifndef THREEMF_LOADER_CLASS_H
#define THREEMF_LOADER_CLASS_H

#include <glm/glm.hpp>
#include "lib3mf_implicit.hpp"

#include "loader_base.h"

class ThreeMF_Load {
    public:
        ThreeMF_Load(const char* filepath, Lib3MF::PWrapper wrapper);

        void load3mf(std::vector<ObjVertex>& vertices, std::vector<GLuint>& indices);
    private:
        const char* filepath;
        Lib3MF::PWrapper wrapper;
        Lib3MF::PModel model;
};

#endif