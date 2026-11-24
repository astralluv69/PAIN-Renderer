#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION

#include "obj_mesh.h"

ObjMesh::ObjMesh(const char* filepath, bool& willClip) {
    std::string path(filepath);
    std::string fileType = path.substr(path.size() - 3);
    ObjMesh::fileType = fileType;
    if (fileType == "obj") {
        OBJ_Load objFile(filepath);
        objFile.loadObj(meshVertices, meshIndices);
        willClip = false;
    } else if (fileType == "stl") {
        STL_Load stlFile(filepath);
        stlFile.loadStl(meshVertices, meshIndices, willClip);
    } else if (fileType == "3mf") {
        ThreeMF_Load threeMFFile(filepath, Lib3MF::CWrapper::loadLibrary());
        threeMFFile.load3mf(meshVertices, meshIndices);
        willClip = true;
    }

    /*
    std::cout << "--- Mesh Loaded ---" << std::endl;
    std::cout << "Unique Vertices: " << meshVertices.size() << std::endl;
    std::cout << "Indices Count: " << meshIndices.size() << std::endl;
    std::cout << "Total Triangles: " << meshIndices.size() / 3 << std::endl;
    */

    meshVBO.Delete();
    meshEBO.Delete();

    VBO meshVBO(meshVertices);
    EBO meshEBO(meshIndices);

    meshVAO.Bind();

    meshVBO.Bind();
    meshEBO.Bind();

    meshVAO.LinkAttrib(meshVBO, 0, 3, GL_FLOAT, sizeof(ObjVertex), 0);
    meshVAO.LinkAttrib(meshVBO, 1, 3, GL_FLOAT, sizeof(ObjVertex), (void*)(3 * sizeof(float)));

    meshVAO.Unbind();
    meshVBO.Unbind();
    meshEBO.Unbind();
}

ObjMesh::ObjMesh(const std::vector<std::string>& filepaths) {
    int SOLVER_SIZE = 128;

    Reconstructor::GenerateIcosphere(3, 0.8f, meshVertices, meshIndices);

    std::vector<CameraViewSetup> setups = {
        {filepaths[0].c_str(), 0.0f, 0.0f},
        {filepaths[1].c_str(), 90.0f, 0.0f},
        {filepaths[2].c_str(), 180.0f, 0.0f},
        {filepaths[3].c_str(), 270.0f, 0.0f},
        {filepaths[4].c_str(), 0.0f, 90.0f}
    };

    std::vector<CameraView> views;
    for (CameraViewSetup setup : setups) {
        CameraView view;
        view.view_proj = Reconstructor::GenerateViewProj(
            setup.azimuth_deg,
            setup.elevation_deg
        );

        view.target_rgba = Reconstructor::LoadImage(
            setup.filepath,
            SOLVER_SIZE,
            SOLVER_SIZE
        );

        views.push_back(view);
    }

    Reconstructor reconstructor;
    reconstructor.Optimize(meshVertices, meshIndices, views, SOLVER_SIZE, SOLVER_SIZE);

    meshVBO.Delete();
    meshEBO.Delete();

    VBO meshVBO(meshVertices);
    EBO meshEBO(meshIndices);

    meshVAO.Bind();

    meshVBO.Bind();
    meshEBO.Bind();

    meshVAO.LinkAttrib(meshVBO, 0, 3, GL_FLOAT, sizeof(ObjVertex), 0);
    meshVAO.LinkAttrib(meshVBO, 1, 3, GL_FLOAT, sizeof(ObjVertex), (void*)(3 * sizeof(float)));

    meshVAO.Unbind();
    meshVBO.Unbind();
    meshEBO.Unbind();
}

void ObjMesh::Draw(Shader& shader, Camera& camera, glm::mat4 model) {
    shader.Activate();
    meshVAO.Bind();

    camera.Matrix(shader, "camMatrix");
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glDrawElements(GL_TRIANGLES, meshIndices.size(), GL_UNSIGNED_INT, 0);

    meshVAO.Unbind();
    shader.Deactivate();
}

void ObjMesh::DrawWireframe(Shader& shader, Camera& camera, glm::mat4 model) {
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

void ObjMesh::Delete() {
    meshVAO.Unbind();
    meshVBO.Unbind();
    meshEBO.Unbind();

    meshVAO.Delete();
    meshVBO.Delete();
    meshEBO.Delete();
}