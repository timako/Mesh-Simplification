#include "mesh.h"

// class mesh
// {
// private:
//     bool load_OFF_file(const std::string &filename, std::vector<glm::vec3> &vertices,
//                        std::vector<glm::vec3> &normals, std::vector<unsigned short> &indices,
//                        std::vector<std::vector<unsigned short>> &triangles, double &xmin, double &xmax,
//                        double &ymin, double &ymax, double &zmin, double &zmax);

// public:
//     // functions
//     mesh();
//     mesh(const char *filename);
//     void simplify(uint32_t resolution);
//     void compute_normals_vertex();
//     uint32_t get_vertex_count();
//     uint32_t get_triangle_count();
//     uint32_t get_element_count();
//     // data
//     std::vector<unsigned short> indices;
//     std::vector<glm::vec3> vertices, normals;
//     std::vector<std::vector<uint16_t>> triangles; // opengl draw mode
//     bounding_box box;
// };

bool Mesh::load_OFF_file(const std::string &filename, std::vector<glm::vec3> &vertices,
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
Mesh::Mesh() = default;

Mesh::Mesh(const char *filename)
{
    bounding_box B;
    if (!load_OFF_file(filename, vertices, normals, indices, triangles, B.xmin, B.xmax, B.ymin, B.ymax, B.zmin, B.zmax))
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
void Mesh::simplify(uint32_t resolution)
{
    // 3-dimension resolution in space.
    std::vector<std::vector<uint16_t>> vpixel;
    std::vector<uint16_t> vpixel_index_new;
    vpixel.resize(resolution * resolution * resolution);
    vpixel_index_new.resize(resolution * resolution * resolution);
    // enlarge the bounding box to make sure all the vertices are inside the box.
    bounding_box BoxProcess = box;
    BoxProcess.enlarge(0.1);
    // calculate dx, dy, dz for each volume pixel.
    float dx = (BoxProcess.xmax - BoxProcess.xmin) / resolution;
    float dy = (BoxProcess.ymax - BoxProcess.ymin) / resolution;
    float dz = (BoxProcess.zmax - BoxProcess.zmin) / resolution;
    // for each node of each triangle, calculate the volume pixel it belongs to.
    // std::vector<std::vector<uint16_t>> triangles;
    for (size_t t = 0; t < triangles.size(); ++t)
    {
        for (size_t v = 0; v < 3; ++v)
        {
            uint16_t vtmp = triangles[t][v];
            int x = (int)((vertices[vtmp].x - BoxProcess.xmin) / dx);
            int y = (int)((vertices[vtmp].y - BoxProcess.ymin) / dy);
            int z = (int)((vertices[vtmp].z - BoxProcess.zmin) / dz);
            if (count(vpixel[x * resolution * resolution + y * resolution + z].begin(), vpixel[x * resolution * resolution + y * resolution + z].end(), vtmp) == 0)
            {
                vpixel[x * resolution * resolution + y * resolution + z].push_back(vtmp);
            }
        }
    }

    // for each volume pixel, calculate the average of the vertices and the normal.
    std::vector<glm::vec3> vavg;
    std::vector<glm::vec3> vnorm;
    uint32_t ind_new = 0;
    for (size_t v = 0; v < vpixel.size(); ++v)
    {
        if (vpixel[v].size() > 0)
        {
            glm::vec3 tmp;
            glm::vec3 tmp_norm;
            for (auto vtmp : vpixel[v])
            {
                tmp += vertices[vtmp];
                tmp_norm += normals[vtmp];
            }
            tmp = tmp / (float)vpixel[v].size();
            tmp_norm = tmp_norm / (float)vpixel[v].size();
            vavg.push_back(tmp);
            vnorm.push_back(tmp_norm);
            vpixel_index_new[v] = ind_new;
            ind_new++;
        }
        // else
        // {
        //     vpixel_index_new[v] = -1;
        // }
    }
    // for each vertex in each triangle, calculate the new index.
    std::vector<std::vector<uint16_t>> triangles_new;
    for (size_t t = 0; t < triangles.size(); ++t)
    {
        std::vector<uint16_t> v_new;
        for (size_t v = 0; v < 3; ++v)
        {
            uint16_t vtmp = triangles[t][v];
            int x = (int)((vertices[vtmp].x - BoxProcess.xmin) / dx);
            int y = (int)((vertices[vtmp].y - BoxProcess.ymin) / dy);
            int z = (int)((vertices[vtmp].z - BoxProcess.zmin) / dz);
            v_new.push_back(vpixel_index_new[x * resolution * resolution + y * resolution + z]);
        }
        // if (v_new[0] != -1 && v_new[1] != -1 && v_new[2] != -1 && )
        if (v_new[0] != v_new[1] && v_new[1] != v_new[2] && v_new[0] != v_new[2])
        {
            triangles_new.push_back(v_new);
        }
    }
    triangles = triangles_new;
    vertices = vavg;
    normals = vnorm;
}
void Mesh::compute_normals_vertex()
{
    return;
}
uint32_t Mesh::get_vertex_count()
{
    return vertices.size();
}
uint32_t Mesh::get_triangle_count()
{
    return triangles.size();
}
uint32_t Mesh::get_element_count()
{
    return indices.size();
}