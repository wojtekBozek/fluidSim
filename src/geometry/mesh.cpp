#include "geometry/mesh.hpp"
using namespace rendering;
Mesh::Mesh()
{
    glGenVertexArrays(1, &(this->m_VAO));
    glGenBuffers(1, &(this->m_VBO));
    glGenBuffers(1, &(this->m_EBO));
}

Mesh::Mesh(const std::vector<vertex3D> &vertices , const std::vector<uint32_t>& indices) : Mesh()
{
    m_vertices.add_vertices(vertices);
    m_indices.insert(m_indices.end(), indices.begin(), indices.end());
    for (auto& ind : indices)
    {
        std::cout << ind << "\n";
    }
}

Mesh::Mesh(vertex3D &&vertices) : Mesh()
{
    //todo
}

void Mesh::drawInstances(GLuint mvpLayout)
{
    glBindVertexArray(m_VAO);
    for (int i = 0; i < m_instances.size(); i++)
    {
        glUniformMatrix4fv(mvpLayout, 1, GL_FALSE, &m_instancesMVPs[i][0][0]);
        glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);
    }
}

void Mesh::draw()
{
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);
}

void Mesh::bind(int layoutPosition, int layoutUV, int layoutNormals, int layoutColor)
{
    m_vertices.bufferVertexData(m_VAO, m_VBO);
    if(layoutPosition>=0)
    {
        m_vertices.bindPosition(layoutPosition);
    }
    if(layoutUV>=0)
    {
        m_vertices.bindUVs(layoutUV);
    }
    if(layoutNormals>=0)
    {
        m_vertices.bindNormals(layoutNormals);
    }
    if(layoutColor>=0)
    {
        m_vertices.bindColors(layoutColor);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(m_indices.at(0)), m_indices.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);
}
