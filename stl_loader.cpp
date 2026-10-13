#include "stl_loader.h"

STL_Load::STL_Load(const char* filepath) {
    STL_Load::filepath = filepath;
}

void STL_Load::loadStl(std::vector<ObjVertex>& vertices, std::vector<GLuint>& indices, bool& willClip) {
    vertices.clear();
    indices.clear();

    std::ifstream file(filepath, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Failed to open STL File: " << filepath << std::endl;
    }

    file.seekg(0, std::ios::end);
    size_t file_size = file.tellg();
    file.seekg(5, std::ios::beg);

    std::string buffer("\0", std::min(file_size, size_t(2048)));
    file.read(&buffer[0], buffer.size());

    bool isBinary = !(buffer.find("facet") != std::string::npos || buffer.find("endsolid") != std::string::npos);

    std::vector<Triangle> triangles;

    if (isBinary) {
        file.seekg(80, std::ios::beg);

        uint32_t num_triangles = 0;
        file.read(reinterpret_cast<char*>(&num_triangles), sizeof(num_triangles));
        triangles.reserve(num_triangles * 3);

        for (uint32_t i = 0; i < num_triangles; i++) {
            Triangle triangle;
            
            glm::vec3 normal;
            file.read(reinterpret_cast<char*>(&normal), sizeof(glm::vec3));

            triangle.v1.normal = normal;
            triangle.v2.normal = normal;
            triangle.v3.normal = normal;

            file.read(reinterpret_cast<char*>(&triangle.v1.position), sizeof(glm::vec3));
            file.read(reinterpret_cast<char*>(&triangle.v2.position), sizeof(glm::vec3));
            file.read(reinterpret_cast<char*>(&triangle.v3.position), sizeof(glm::vec3));

            uint16_t attribute_byte_count = 0;
            file.read(reinterpret_cast<char*>(&attribute_byte_count), 2);

            triangles.push_back(triangle);
            triangles.push_back(triangle);
            triangles.push_back(triangle);
        }

        willClip = true;
    } else {
        std::string line;
        Triangle currentTriangle;
        int vertexIdx = 0;

        while (std::getline(file, line)) {
            std::string header;
            glm::vec3 normal;
            std::istringstream iss(line);
            std::string token;
            iss >> token >> std::ws;

            if (token == "solid") {
                std::getline(iss, header);
            } else if (token == "facet") {
                std::string subToken;
                iss >> subToken;
                iss >> normal.x >> normal.y >> normal.z;
                currentTriangle.v1.normal = normal;
                currentTriangle.v2.normal = normal;
                currentTriangle.v3.normal = normal;
                vertexIdx = 0;
            } else if (token == "vertex") {
                if (vertexIdx < 3) {
                    if (vertexIdx == 0) {
                        iss >> currentTriangle.v1.position.x
                            >> currentTriangle.v1.position.y
                            >> currentTriangle.v1.position.z;
                    } else if (vertexIdx == 1) {
                        iss >> currentTriangle.v2.position.x
                            >> currentTriangle.v2.position.y
                            >> currentTriangle.v2.position.z;
                    } else if (vertexIdx == 2) {
                        iss >> currentTriangle.v3.position.x
                            >> currentTriangle.v3.position.y
                            >> currentTriangle.v3.position.z;
                    }
                    vertexIdx++;
                }
            } else if (token == "endfacet") {
                triangles.push_back(currentTriangle);
            }
        }

        willClip = false;
    }

    vertices.clear();
    indices.clear();

    std::map<ObjVertex, GLuint, StlVertexComparator> uniqueVertexMap;

    indices.reserve(triangles.size() * 3);

    for (const auto& triangle : triangles) {
        for (int i = 0; i < 3; i++) {
            ObjVertex vertex;
            if (i == 0) {
                vertex.position = triangle.v1.position;
                vertex.normal = triangle.v1.normal;
            } else if (i == 1) {
                vertex.position = triangle.v2.position;
                vertex.normal = triangle.v2.normal;
            } else if (i == 2) {
                vertex.position = triangle.v3.position;
                vertex.normal = triangle.v3.normal;
            }

            auto it = uniqueVertexMap.find(vertex);
            if (it == uniqueVertexMap.end()) {
                GLuint newIdx = static_cast<GLuint>(vertices.size());
                vertices.push_back(vertex);
                uniqueVertexMap[vertex] = newIdx;
                indices.push_back(newIdx);
            } else {
                indices.push_back(it->second);
            }
        }
    }
}