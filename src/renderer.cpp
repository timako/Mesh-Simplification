#include "renderer.h"

// class Renderer
// {
// public:
//     Renderer(uint32_t shader_id, Mesh &m);
//     ~Renderer();
//     void render(float camera_pos, float light_pos, bool is_light);
//     void update();
//     void clear();
//     Mesh orig;

// private:
//     uint32_t VertexArrayID;
//     uint32_t programID;
//     uint32_t MatrixID, ViewMatrixID, ModelMatrixID;
//     uint32_t LightID;
//     uint32_t vertexbuffer;
//     uint32_t normalbuffer;
//     uint32_t elementbuffer;
// };
Renderer::Renderer(uint32_t shader_id, Mesh &m)
{
    programID = shader_id;
    model = m;
    // Create Vertex Array Object
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // MatrixID = glGetUniformLocation(programID, "MVP");
    ProjMatrixID = glGetUniformLocation(programID, "P");
    ViewMatrixID = glGetUniformLocation(programID, "V");
    ModelMatrixID = glGetUniformLocation(programID, "M");
    LightID = glGetUniformLocation(programID, "lightPos");
    CameraID = glGetUniformLocation(programID, "viewPos");

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, model.get_vertex_count() * sizeof(glm::vec3), &m.vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, model.get_vertex_count() * sizeof(glm::vec3), &m.normals[0], GL_STATIC_DRAW);

    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.get_element_count() * sizeof(uint32_t), &m.indices[0], GL_STATIC_DRAW);

    glUseProgram(programID);
}
extern int SCR_WIDTH;
extern int SCR_HEIGHT;
void Renderer::render(float camera_angle, float light_angle, bool is_light)
{
    glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    // detail::tmat4x4< T > glm::gtc::matrix_transform::lookAt
    // (
    //     detail::tvec3< T > const &  eye,
    //     detail::tvec3< T > const &  center,
    //     detail::tvec3< T > const &  up
    // )

    glm::mat4 ViewMatrix = glm::lookAt(
        glm::vec3(2, 1, 3),  // camera pos
        glm::vec3(0, 0, 0),  // center pos
        glm::vec3(0, 1, 0)); // up vector

    glm::mat4 ModelMatrix = glm::mat4(1.0f);

    ModelMatrix = glm::translate(ModelMatrix, -model.box.get_center());
    ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0 / (double)model.box.get_size().x));
    ModelMatrix = glm::rotate(ModelMatrix, glm::radians(camera_angle), glm::vec3(0.0f, 0.1f, 0.0f));
    glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
    glm::vec3 lightPos = glm::vec3(3.2 * std::cos(light_angle), 2.2, 3.2 * std::sin(light_angle));
    glm::vec3 viewPos = glm::vec3(2, 1, 3);
    // Send our transformation to the currently bound shader,

    // uniform mat4 M;
    // uniform mat4 V;
    // uniform mat4 P;

    // glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
    glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
    glUniformMatrix4fv(ProjMatrixID, 1, GL_FALSE, &ProjectionMatrix[0][0]);

    // uniform vec3 lightPos;
    // uniform vec3 viewPos;
    glUniform3fv(LightID, 1, &lightPos[0]);
    glUniform3fv(CameraID, 1, &viewPos[0]);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
        0,        // attribute - GLSL layout
        3,        // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        nullptr   // array buffer offset
    );

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(
        1,        // attribute - GLSL layout
        3,        // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        nullptr   // array buffer offset
    );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glDrawElements(
        GL_TRIANGLES,         // mode
        model.indices.size(), // count
        GL_UNSIGNED_SHORT,    // type
        nullptr               // element array buffer offset
    );
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}
void Renderer::update()
{
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, model.get_vertex_count() * sizeof(glm::vec3), &model.vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, model.get_vertex_count() * sizeof(glm::vec3), &model.normals[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.get_element_count() * sizeof(uint32_t), &model.indices[0], GL_STATIC_DRAW);
}
void Renderer::clear()
{
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteBuffers(1, &elementbuffer);
    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &VertexArrayID);
}