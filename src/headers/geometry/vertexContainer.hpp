#pragma once
#include <iostream>
#include <vector>
#include <glm/glm.hpp>

template <typename T>
class VertexContainer
{
    public:
    VertexContainer();
    VertexContainer(uint32_t initialSize);
    void reserveSpaceForVertices(uint32_t newSize);
    void shrink();
    void addVertex(const T& vertex);
    void addVertex(T&& vertex);
    void addVertices(const std::vector<T>& vertices);
    void addVertices(T&& vertices);
    const T& getVertex(const uint32_t) const;
    const uint32_t& getCurrentMaxIndex() const;
    const uint32_t& getCurentNextIndex() const;
    const uint32_t& getNextMaxIndex(const uint32_t val) const;
    ~VertexContainer();

    private:
    std::vector<T> m_vertices; 
    uint32_t m_reservedSize;
};