#include "mesh.h"

Mesh::Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices)
    : meshVertices(vertices), meshIndices(indices), meshVBO(vertices), meshEBO(indices) {

    meshVAO.Bind();

    meshVBO.Bind();
    meshEBO.Bind();

    meshVAO.LinkAttrib(meshVBO, 0, 3, GL_FLOAT, sizeof(Vertex), 0);
    meshVAO.LinkAttrib(meshVBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));

    meshVAO.Unbind();
    meshVBO.Unbind();
    meshEBO.Unbind();
}

void Mesh::Draw(Shader& shader, Camera& camera, glm::mat4 model) {
    shader.Activate();
    meshVAO.Bind();

    camera.Matrix(shader, "camMatrix");
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glDrawElements(GL_TRIANGLES, meshIndices.size(), GL_UNSIGNED_INT, 0);

    meshVAO.Unbind();
    shader.Deactivate();
}

void Mesh::Delete() {
    meshVAO.Unbind();
    meshVBO.Unbind();
    meshEBO.Unbind();

    meshVAO.Delete();
    meshVBO.Delete();
    meshEBO.Delete();
}