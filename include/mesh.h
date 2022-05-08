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
public:
    double xmin, xmax, ymin, ymax, zmin, zmax;
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
    void enlarge(double scale)
    {
        xmin += -(1.0 + scale) * abs(xmin);
        xmax += (1.0 + scale) * abs(xmax);
        ymin += -(1.0 + scale) * abs(ymin);
        ymax += (1.0 + scale) * abs(ymax);
        zmin += -(1.0 + scale) * abs(zmin);
        zmax += (1.0 + scale) * abs(zmax);
    }
};

class mesh
{
private:
    /*

    use the simpliest file format possible: off
    example:
    -------------------------------------------
    OFF
    8 12 0
    -0.274878 -0.274878 -0.274878
    -0.274878  0.274878 -0.274878
    0.274878  0.274878 -0.274878
    0.274878 -0.274878 -0.274878
    -0.274878 -0.274878  0.274878
    -0.274878  0.274878  0.274878
    0.274878  0.274878  0.274878
    0.274878 -0.274878  0.274878
    3  0 1 3
    3  3 1 2
    3  0 4 1
    3  1 4 5
    3  3 2 7
    3  7 2 6
    3  4 0 3
    3  7 4 3
    3  6 4 7
    3  6 5 4
    3  1 5 6
    3  2 1 6
    -------------------------------------------
    the first line is OFF
    the second line is the number of vertices, faces and edges
    following vertice data(x,y,z)
    and index data(how to connect the vertices) in a triangle)
*/
    bool load_OFF_file(const std::string &filename, std::vector<glm::vec3> &vertices,
                       std::vector<glm::vec3> &normals, std::vector<unsigned short> &indices,
                       std::vector<std::vector<unsigned short>> &triangles, double &xmin, double &xmax,
                       double &ymin, double &ymax, double &zmin, double &zmax);

public:
    // functions
    mesh();
    mesh(const char *filename);
    void simplify(uint32_t resolution);
    void compute_normals_vertex();
    uint32_t get_vertex_count();
    uint32_t get_triangle_count();
    uint32_t get_element_count();
    // data
    std::vector<unsigned short> indices;
    std::vector<glm::vec3> vertices, normals;
    std::vector<std::vector<uint16_t>> triangles; // opengl draw mode
    bounding_box box;
};

#endif