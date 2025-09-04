#include "mesh.hpp"
#include <algorithm>


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
}

Mesh::Mesh(vertex3D &&vertices) : Mesh()
{
    //todo
}

Mesh::~Mesh()
{
    std::cout << "Deleting mesh\n";
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
    if (m_EBO) glDeleteBuffers(1, &m_EBO);
}

void Mesh::removeInstances() 
{
    if(m_detachedInstances_id.size()>0)
    {
        std::sort(m_detachedInstances_id.begin(), m_detachedInstances_id.end());
        for(const uint32_t id : m_detachedInstances_id)
        {
            m_instancesMVPs.erase(m_instancesMVPs.begin() + id);
        }
    }
}

uint32_t Mesh::addNewInstance(const glm::mat4& data)
{
    m_instances.push_back(glm::mat4(1.0f));
    m_instancesMVPs.push_back(glm::mat4(1.0f));
    return m_instances.size()-1;
}

void Mesh::detachInstance(uint32_t id)
{
    m_detachedInstances_id.push_back(id);
}

const std::vector<uint32_t> &Mesh::getDetachedInstances() const
{
    return m_detachedInstances_id;
}

void Mesh::computeMVPs(glm::mat4 (*func)(glm::mat4))
{
     for (int i = 0; i < m_instances.size(); i++)
     {
         m_instancesMVPs.at(i) = func(m_instances.at(i));
     }
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

void Mesh::draw(GLuint mvpLayout, glm::mat4 (*func)(glm::mat4))
{
    m_basic_instance_mvp = func(m_basic_instance);
    glBindVertexArray(m_VAO);
    glUniformMatrix4fv(mvpLayout, 1, GL_FALSE, &m_basic_instance_mvp[0][0]);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);
}

glm::mat4& Mesh::modifyInstance(uint32_t id)
{
    if (id >= m_instances.size())
    {
        throw std::out_of_range("Invalid instance ID in modifyInstance.");
    }
    return m_instances[id];
}

void Mesh::updateInstanceMatrix(uint32_t id, const glm::mat4& transform)
{
    if (id >= m_instances.size())
    {
        throw std::out_of_range("Invalid instance ID in updateInstanceMatrix.");
    }
    
    m_instances[id] = transform;

    // Update related data, such as precomputed MVPs
    // Assuming MVP computation is handled separately; if needed here, we could add a callback or inline logic.
    // Example inline logic:
    m_instancesMVPs[id] = transform; // Replace with actual MVP computation if necessary.
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
