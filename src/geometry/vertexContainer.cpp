#include "headers/geometry/vertexContainer.hpp"
#include "vertexContainer.hpp"

template <typename T>
VertexContainer<T>::VertexContainer()
{
    m_vertices.reserve(256);
}

template <typename T>
VertexContainer<T>::VertexContainer(uint32_t initialSize)
{
    m_vertices.reserve(initialSize);
}

template <typename T>
void VertexContainer<T>::reserveSpaceForVertices(uint32_t newSize)
{
    m_vertices.reserve(newSize);
}

template <typename T>
void VertexContainer<T>::shrink()
{
    m_vertices.shrink_to_fit();
}

template <typename T>
void VertexContainer<T>::addVertex(const T &vertex)
{
    m_vertices.push_back(vertex);
}

template <typename T>
void VertexContainer<T>::addVertex(T &&vertex)
{
    m_vertices.push_back(std::move(vertices));
}

template <typename T>
void VertexContainer<T>::addVertices(const std::vector<T> &vertices)
{
    m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());
}

template <typename T>
void VertexContainer<T>::addVertices(T &&vertices)
{
    m_vertices.insert(m_vertices.end(), std::make_move_iterator(vertices.begin()), std::make_move_iterator(vertices.end()));
}

template <typename T>
const T &VertexContainer<T>::getVertex(const uint32_t id) const
{
    if(id < m_vertices.size())
    {
        return m_vertices.at(id);
    }
}

template <typename T>
const uint32_t &VertexContainer<T>::getCurrentMaxIndex() const
{
    return m_vertices.size() -1;
}

template <typename T>
const uint32_t &VertexContainer<T>::getCurentNextIndex() const
{
    return m_vertices.size();
}

template <typename T>
const uint32_t &VertexContainer<T>::getNextMaxIndex(const uint32_t val) const
{
    return m_vertices.size() -1 + val;
}

template <typename T>
VertexContainer<T>::~VertexContainer()
{
}

template class VertexContainer<glm::vec3>;