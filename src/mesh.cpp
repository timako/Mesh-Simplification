#include "mesh.h"
bool mesh::load_OFF_file(const std::string &filename, std::vector<glm::vec3> &vertices,
                         std::vector<glm::vec3> &normals, std::vector<unsigned short> &indices,
                         std::vector<std::vector<unsigned short>> &triangles, glm::vec2 &xpos,
                         glm::vec2 &ypos, glm::vec2 &zpos)
{
}
mesh::mesh()
{
}
mesh::mesh(const char *filename)
{
}
void mesh::simplify()
{
}
void mesh::compute_normals_vertex()
{
}
uint32_t mesh::get_vertex_count()
{
}
uint32_t mesh::get_triangle_count()
{
}