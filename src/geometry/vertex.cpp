#include "vertex.hpp"

using namespace rendering;

void VerticesContainer::add_vertex(const vertex3D& vertex)
{
    vertices.push_back(vertex);
}
void VerticesContainer::add_vertices(const std::vector<vertex3D>& vertices_)
{
    vertices.insert(vertices.end(), vertices_.begin(), vertices_.end());
}
const uint32_t& VerticesContainer::get_current_next_index() const
{
    return vertices.size();
}
const uint32_t& VerticesContainer::get_current_max_index() const
{
    return vertices.size() - 1;
}
const uint32_t& VerticesContainer::get_next_max_index(const uint32_t val) const
{
    return vertices.size() + val - 1;
}
const vertex3D& VerticesContainer::get_vertex(const uint32_t id) const
{
    try {
        if (id >= vertices.size()) {
            throw std::invalid_argument("Vertice index out of range!!!");
        }
        return vertices.at(id);
    }
    catch (...) {
    }
}
void rendering::VerticesContainer::bindAttribute(unsigned int layout, GLint size, const void *offset)
{
    std::cout << offset << "\n";
    glVertexAttribPointer(layout, size, GL_FLOAT, GL_FALSE, sizeof(vertex3D), offset);
    glEnableVertexAttribArray(layout);
}

void rendering::VerticesContainer::bufferVertexData(GLuint vao, GLuint vbo)
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex3D), vertices.data(), GL_STATIC_DRAW);
}

void VerticesContainer::bindPosition(unsigned int layout)
{
    bindAttribute(layout, 3, (void*)offsetof(vertex3D, xyz));
}

void VerticesContainer::bindUVs(unsigned int layout)
{
    bindAttribute(layout, 2, (void*)offsetof(vertex3D, uv));
}
void VerticesContainer::bindNormals(unsigned int layout)
{
    bindAttribute(layout, 3, (void*)offsetof(vertex3D, normal));
}
void VerticesContainer::bindColors(unsigned int layout)
{
    bindAttribute(layout, 3, (void*)offsetof(vertex3D, rgb));
}