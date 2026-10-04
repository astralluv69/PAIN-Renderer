#include "mesh.h"

Mesh::Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices)
    : meshVertices(vertices), meshIndices(indices), meshVBO(vertices), meshEBO(indices) {

    meshVAO.Bind();

    meshVBO.Bind();
    meshEBO.Bind();

    meshVAO.LinkAttrib(meshVBO, 0, 3, GL_FLOAT, sizeof(Vertex), 0);
    meshVAO.LinkAttrib(meshVBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
    meshVAO.LinkAttrib(meshVBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));

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

void Mesh::DrawWireframe(Shader& shader, Camera& camera, glm::mat4 model) {
    shader.Activate();
    meshVAO.Bind();

    camera.Matrix(shader, "camMatrix");
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glLineWidth(5.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glDepthFunc(GL_LEQUAL);

    glDrawElements(GL_TRIANGLES, meshIndices.size(), GL_UNSIGNED_INT, 0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDepthFunc(GL_LESS);

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