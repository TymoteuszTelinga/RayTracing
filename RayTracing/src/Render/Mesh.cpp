
#include "Mesh.h"
#include <fstream>
#include <sstream>
#include <iostream>

std::istream& operator>>(std::istream& is, glm::vec3& vec)
{
    float x, y, z;
    is >> x >> y >> z;
    vec = glm::vec3(x, y, z);
    return is;
}

std::istream& operator>>(std::istream& is, glm::ivec3& vec)
{
    int x, y, z;
    is >> x >> y >> z;
    vec = glm::ivec3(x, y, z);
    return is;
}

// curentlu return simple cube
Mesh::Mesh(const std::string& filePath)
    :m_MaxBox(0.f), m_MinBox(0.f)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cerr << "Unable to load mesh at loaction: " << filePath << std::endl;
        return;
    }

    std::string line;

    std::vector<glm::vec3> Positions;
    std::vector<glm::ivec3> Triangles;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string prefix;

        ss >> prefix;

        // vertex position
        if (prefix == "v")
        {
            glm::vec3 pos;
            ss >> pos;
            Positions.emplace_back(pos);
        }

        // face
        if (prefix == "f")
        {
            glm::ivec3 triangle;
            ss >> triangle;
            Triangles.emplace_back(triangle);
        }

    }

    
    for (auto pos : Positions)
    {
        Vertex v;
        v.Position = pos;
        m_Vertesis.emplace_back(v);
        UpdateBBox(pos);
    }

    for (auto tri : Triangles)
    {
        Triangle t;
        t.Vertexis = tri - glm::ivec3(1);
        m_Faces.emplace_back(t);
    }
}

Mesh::~Mesh()
{
}

void Mesh::UpdateBBox(const glm::vec3& position)
{
    //glm::min
    m_MinBox = glm::min(m_MinBox, position);
    m_MaxBox = glm::max(m_MaxBox, position);
}
