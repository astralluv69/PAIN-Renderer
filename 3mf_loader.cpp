#include "3mf_loader.h"

ThreeMF_Load::ThreeMF_Load(const char* filepath, Lib3MF::PWrapper wrapper) {
    ThreeMF_Load::model = wrapper->CreateModel();

    Lib3MF::PReader reader = model->QueryReader("3mf");
    reader->ReadFromFile(filepath);
}

void ThreeMF_Load::load3mf(std::vector<ObjVertex>& vertices, std::vector<GLuint>& indices) {
    vertices.clear();
    indices.clear();

    vertices.shrink_to_fit();
    indices.shrink_to_fit();

    std::vector<Lib3MF::sPosition> tempVertices; 
    std::vector<Lib3MF::sTriangle> triangles;
    
    Lib3MF::PMeshObjectIterator meshIterator = model->GetMeshObjects();
    meshIterator->MoveNext();

    Lib3MF::PMeshObject mesh = meshIterator->GetCurrentMeshObject();

    mesh->GetTriangleIndices(triangles);
    mesh->GetVertices(tempVertices);

    vertices.reserve(tempVertices.size());
    indices.reserve(triangles.size() * 3);

    for (Lib3MF::sTriangle triangle : triangles) {
        indices.push_back(triangle.m_Indices[0]);
        indices.push_back(triangle.m_Indices[1]);
        indices.push_back(triangle.m_Indices[2]);
    }

    indices.shrink_to_fit();

    for (Lib3MF::sPosition position : tempVertices) {
        ObjVertex vertex;
        
        vertex.position.x = position.m_Coordinates[0];
        vertex.position.y = position.m_Coordinates[1];
        vertex.position.z = position.m_Coordinates[2];

        vertex.normal = glm::vec3(0.0f);

        vertices.push_back(vertex);
    }

    for (size_t i = 0; i < indices.size(); i += 3) {
        if (i + 2 >= indices.size()) break;

        GLuint idx0 = indices[i];
        GLuint idx1 = indices[i + 1];
        GLuint idx2 = indices[i + 2];

        if (idx0 >= vertices.size() || idx1 >= vertices.size() || idx2 >= vertices.size()) {
            std::cerr << "Parser Error: Index buffer excees vertex array bounds" << std::endl;
        }

        ObjVertex vertex0 = vertices[idx0];
        ObjVertex vertex1 = vertices[idx1];
        ObjVertex vertex2 = vertices[idx2];

        glm::vec3 e0 = vertex1.position - vertex0.position;
        glm::vec3 e1 = vertex2.position - vertex0.position;

        glm::vec3 normal = glm::cross(e0, e1);

        vertices[idx0].normal += normal;
        vertices[idx1].normal += normal;
        vertices[idx2].normal += normal;
    }

    for (ObjVertex& vertex : vertices) {
        float length = glm::length(vertex.normal);

        if (length > 0.0f) {
            vertex.normal = glm::normalize(vertex.normal);
        } else {
            vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        }
    }
}