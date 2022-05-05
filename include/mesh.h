#ifndef MESH_H
#define MESH_H

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <float.h>
#include <algorithm>
#include <memory>
#include <stdint.h>
// Include GLM
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/transform.hpp>
#include <unordered_map>

class bounding_box
{
private:
    double xmin, xmax, ymin, ymax, zmin, zmax;

public:
    bounding_box()
    {
        xmin = xmax = ymin = ymax = zmin = zmax = 0.0;
    }
    glm::vec3 get_center()
    {
        return glm::vec3((xmin + xmax) / 2.0, (ymin + ymax) / 2.0, (zmin + zmax) / 2.0);
    }
    glm::vec3 get_size()
    {
        return glm::vec3(xmax - xmin, ymax - ymin, zmax - zmin);
    }
};

class mesh
{
private:
    std::vector<unsigned short> indices;
    std::vector<glm::vec3> indexed_vertices, indexed_normals;
    std::vector<std::vector<uint16_t>> triangles;
    bounding_box box;
    bool load_OFF_file(const std::string &filename, std::vector<glm::vec3> &vertices,
                       std::vector<glm::vec3> &normals, std::vector<unsigned short> &indices,
                       std::vector<std::vector<unsigned short>> &triangles, glm::vec2 &xpos,
                       glm::vec2 &ypos, glm::vec2 &zpos);

public:
    mesh();
    mesh(const char *filename);
    void simplify();
    void compute_normals_vertex();
    uint32_t get_vertex_count();
    uint32_t get_triangle_count();
};

#endif