#include "obj_loader.h"

OBJ_Load::OBJ_Load(const char* filepath) {
    OBJ_Load::filepath = filepath;
}

void OBJ_Load::loadObj(std::vector<ObjVertex>& vertices, std::vector<GLuint>& indices) {
    vertices.clear();
    indices.clear();

    std::ifstream file(filepath);

    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ File: " << filepath << std::endl;
    }

    std::vector<glm::vec3> temp_positions;
    std::vector<glm::vec2> temp_texCoords;
    std::vector<FaceVertex> face_vertices;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            glm::vec3 pos;
            ss >> pos.x >> pos.y >> pos.z;
            temp_positions.push_back(pos);
        } else if (prefix == "vt") {
            glm::vec2 tex;
            ss >> tex.x >> tex.y;
            temp_texCoords.push_back(tex);
        } else if (prefix == "f") {
            for (int i = 0; i < 3; i++) {
                std::string vertexStr;
                ss >> vertexStr;

                FaceVertex fVert = {0, 0, 0};
                std::stringstream vertexSS(vertexStr);
                std::string v, vt, vn;

                std::getline(vertexSS, v, '/');
                std::getline(vertexSS, vt, '/');
                std::getline(vertexSS, vn, '/');

                fVert.vIdx = !v.empty() ? std::stoi(v) : 0;
                fVert.vtIdx = !vt.empty() ? std::stoi(vt) : 0;
                fVert.vnIdx = !vn.empty() ? std::stoi(vn) : 0;

                face_vertices.push_back(fVert);
            }
        }
    }

    file.close();

    std::map<ObjVertex, GLuint, VertexComparator> uniqueVertices;

    for (const auto& fVert : face_vertices) {
        ObjVertex vertex;

        if (fVert.vIdx > 0) {
            vertex.position = temp_positions[fVert.vIdx - 1];
        }
        if (fVert.vtIdx > 0) continue;
        
        auto it = uniqueVertices.find(vertex);
        if(it == uniqueVertices.end()) {
            GLuint newIdx = vertices.size();
            uniqueVertices[vertex] = newIdx;
            vertices.push_back(vertex);
            indices.push_back(newIdx);
        } else {
            indices.push_back(it->second);
        }
    }

    for (ObjVertex& v : vertices) v.normal = glm::vec3(0.0f);

    for (ObjVertex vertex : vertices) {
        vertex.normal = glm::vec3(0.0f);
    }

    for (size_t i = 0; i < indices.size(); i += 3) {
        if (i + 2 >= indices.size()) break;

        GLuint idx0 = indices[i];
        GLuint idx1 = indices[i + 1];
        GLuint idx2 = indices[i + 2];
            
        if (idx0 >= vertices.size() || idx1 >= vertices.size() || idx2 >= vertices.size()) {
            std::cout << "Parser Error: Index buffer excees vertex array bounds" << std::endl;
        }

        ObjVertex v0 = vertices[idx0];
        ObjVertex v1 = vertices[idx1];
        ObjVertex v2 = vertices[idx2];

        glm::vec3 e1 = v1.position - v0.position;
        glm::vec3 e2 = v2.position - v0.position;

        glm::vec3 normal = glm::cross(e1, e2);

        vertices[idx0].normal += normal;
        vertices[idx1].normal += normal;
        vertices[idx2].normal += normal;
    }

    for (ObjVertex vertex : vertices) {
        float len = glm::length(vertex.normal);

        if (len > 0.0f) {
            vertex.normal = glm::normalize(vertex.normal);
        } else {
            vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        }
    }
}