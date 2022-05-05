#ifndef RENDERER_H
#define RENDERER_H

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <fstream>
#include <cfloat>
#include <set>
#include <algorithm>

// Include Glad
#include <glad/glad.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/transform.hpp>

#include "shader.h"
#include "mesh.h"

class renderer
{
public:
    renderer(uint32_t shader_id, mesh &m);
    ~renderer();
    void render(float camera_pos, float light_pos, bool is_light);
    void update();
    void clear();
    mesh orig;

private:
    uint32_t VertexArrayID;
    uint32_t programID;

    uint32_t MatrixID, ViewMatrixID, ModelMatrixID;
    uint32_t LightID;

    uint32_t vertexbuffer;
    uint32_t normalbuffer;
    uint32_t elementbuffer;
};

#endif