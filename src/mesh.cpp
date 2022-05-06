#include "mesh.h"
bool mesh::load_OFF_file(const std::string &filename, std::vector<glm::vec3> &vertices,
                         std::vector<glm::vec3> &normals, std::vector<uint16_t> &indices,
                         std::vector<std::vector<uint16_t>> &triangles, double &xmin, double &xmax,
                         double &ymin, double &ymax, double &zmin, double &zmax)
{
    std::ifstream myfile;
    myfile.open(filename.c_str());
    if (!myfile.is_open())
    {
        std::cout << "Failure to open " << filename << " file" << std::endl;
        return false;
    }

    std::string isOFFfile;
    myfile >> isOFFfile;
    if (isOFFfile != "OFF")
    {
        std::cerr << "File " << filename << " isn't an OFF format file" << std::endl;
        myfile.close();
        return false;
    }

    uint32_t numV, numF, numEd;
    myfile >> numV >> numF >> numEd;

    vertices.resize(numV);
    normals.resize(numV);

    std::vector<std::vector<glm::vec3>> normalsTmp;
    normalsTmp.resize(numV);

    for (size_t v = 0; v < numV; ++v)
    {
        glm::vec3 vertex;
        myfile >> vertex.x >> vertex.y >> vertex.z;
        vertices[v] = vertex;

        if (v == 0)
        {
            xmin = vertex.x;
            xmax = vertex.x;
            ymin = vertex.y;
            ymax = vertex.y;
            zmin = vertex.z;
            zmax = vertex.z;
        }
        else
        {
            if (xmin > vertex.x)
                xmin = vertex.x;
            if (xmax < vertex.x)
                xmax = vertex.x;
            if (ymin > vertex.y)
                ymin = vertex.y;
            if (ymax < vertex.y)
                ymax = vertex.y;
            if (zmin > vertex.z)
                zmin = vertex.z;
            if (zmax < vertex.z)
                zmax = vertex.z;
        }
    }

    for (size_t f = 0; f < numF; ++f)
    {
        uint32_t numberOfVerticesOnFace;
        myfile >> numberOfVerticesOnFace;
        if (numberOfVerticesOnFace == 3)
        {
            uint16_t v1, v2, v3;
            std::vector<uint16_t> v;
            myfile >> v1 >> v2 >> v3;

            v.push_back(v1);
            v.push_back(v2);
            v.push_back(v3);

            triangles.push_back(v);

            indices.push_back(v1);
            indices.push_back(v2);
            indices.push_back(v3);

            glm::vec3 normal;
            glm::vec3 tmp1 = vertices[v2] - vertices[v1];
            glm::vec3 tmp2 = vertices[v3] - vertices[v1];
            normal = glm::normalize(glm::cross(tmp1, tmp2));
            normalsTmp[v1].push_back(normal);
            normalsTmp[v2].push_back(normal);
            normalsTmp[v3].push_back(normal);
        }
        else
        {
            std::cerr << "Error: In object file: " << filename << " faces must be triangles." << std::endl;
            myfile.close();
            return false;
        }
    }

    for (size_t v = 0; v < numV; ++v)
    {
        float sizeTmp = normalsTmp[v].size();
        glm::vec3 tmp;
        for (auto n : normalsTmp[v])
        {
            tmp += n;
        }
        tmp = tmp / sizeTmp;
        normals[v] = tmp;
    }

    myfile.close();
    return true;
}
mesh::mesh() = delete;

mesh::mesh(const char *filename)
{
    bounding_box B;
    if (!load_OFF_file(filename, indexed_vertices, indexed_normals, indices, triangles, B.xmin, B.xmax, B.ymin, B.ymax, B.zmin, B.zmax))
    {
        std::cerr << "Failure to load " << filename << " file" << std::endl;
    }
    else
    {
        std::cout << "Loaded " << filename << " file" << std::endl;
        printf("view center: %f %f %f\n", B.get_center().x, B.get_center().y, B.get_center().z);
        printf("view size: %f %f %f\n", B.get_size().x, B.get_size().y, B.get_size().z);
    }
}
void mesh::simplify(uint32_t resolution)
{
    // 3-dimension resolution in space.
    std::vector<glm::vec3> vpixel;
    std::vector<glm::vec3> vpixel_index;
    vpixel.resize(resolution * resolution * resolution);
    vpixel_index.resize(resolution * resolution * resolution);

    bounding_box BoxProcess = box;
    BoxProcess.enlarge(0.2);
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