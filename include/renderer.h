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

#include "shader.h"
#include "mesh.h"

class Renderer
{
public:
    Renderer(uint32_t shader_id, Mesh &m);
    ~Renderer();
    void render(float camera_angle, float light_angle, bool is_light);
    void update();
    void clear();
    Mesh model;

private:
    uint32_t VertexArrayID;
    uint32_t programID;
    uint32_t MatrixID, ViewMatrixID, ModelMatrixID, ProjMatrixID, CameraID;
    uint32_t LightID;
    uint32_t vertexbuffer;
    uint32_t normalbuffer;
    uint32_t elementbuffer;
};

#endif