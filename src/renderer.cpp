#include "renderer.h"

// class renderer
// {
// public:
//     renderer(uint32_t shader_id, mesh &m);
//     ~renderer();
//     void render(float camera_pos, float light_pos, bool is_light);
//     void update();
//     void clear();
//     mesh orig;

// private:
//     uint32_t VertexArrayID;
//     uint32_t programID;
//     uint32_t MatrixID, ViewMatrixID, ModelMatrixID;
//     uint32_t LightID;
//     uint32_t vertexbuffer;
//     uint32_t normalbuffer;
//     uint32_t elementbuffer;
// };
renderer::renderer(uint32_t shader_id, mesh &m)
{
    programID = shader_id;
    model = m;
    // Create Vertex Array Object
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    MatrixID = glGetUniformLocation(programID, "MVP");
    ViewMatrixID = glGetUniformLocation(programID, "V");
    ModelMatrixID = glGetUniformLocation(programID, "M");

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, model.get_vertex_count() * sizeof(glm::vec3), &m.vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, model.get_vertex_count() * sizeof(glm::vec3), &m.normals[0], GL_STATIC_DRAW);

    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.get_element_count() * sizeof(uint32_t), &m.elements[0], GL_STATIC_DRAW);
}
