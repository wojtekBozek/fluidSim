#pragma once
#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENvertex3DAL
#include <unordered_map>
#include <mutex>
#include <stdexcept>
#include <iostream>


struct material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 emissive;
    float shininess;
};

struct vertex3D {
    glm::vec3 xyz;
    glm::vec3 rgb = {0.0f, 0.0f, 0.0f};
    glm::vec2 uv;
    glm::vec3 normal;
};

namespace rendering {
    class VerticesContainer {
    public:
        VerticesContainer() = default;

        VerticesContainer(const VerticesContainer& other) 
        : vertices(other.vertices), // Move vector ownership
          current_max_vertex(other.current_max_vertex) // Copy simple integer
    {
        std::cout << "VerticesContainer Copy Constructor Called" << std::endl;
    }

        VerticesContainer(VerticesContainer&& other) noexcept
        : vertices(std::move(other.vertices)), // Move vector ownership
          current_max_vertex(other.current_max_vertex) // Copy simple integer
    {
        std::cout << "VerticesContainer Move Constructor Called" << std::endl;
        other.current_max_vertex = 0; // Optional: Reset other to a default state
    }

    // Move assignment operator
    VerticesContainer& operator=(VerticesContainer&& other) noexcept {
        std::cout << "VerticesContainer Move Assignment Operator Called" << std::endl;

        if (this != &other) { // Prevent self-assignment
            vertices = std::move(other.vertices);
            current_max_vertex = other.current_max_vertex; // Copy simple integer
            other.current_max_vertex = 0; // Optional: Reset other
        }
        return *this;
    }

        void add_vertex(const vertex3D& vertex);

        void add_vertices(const std::vector<vertex3D>& vertices_);
        void add_vertices(std::vector<vertex3D>&& vertices_);//TODO

        const vertex3D& get_vertex(const uint32_t id) const;
        const uint32_t& get_current_max_index() const;
        const uint32_t& get_current_next_index() const;
        const uint32_t& get_next_max_index(const uint32_t val) const;

        ~VerticesContainer() {};
        void bufferVertexData(GLuint vao, GLuint vbo);
        void bindPosition(unsigned int position_layout);
        void bindUVs(unsigned int uvs_layout);
        void bindNormals(unsigned int normal_layout);
        void bindColors(unsigned int color_layout);
    private:
        void bindAttribute(unsigned int layout, GLint size, const void* offset);
        std::vector<vertex3D>  vertices;
        uint32_t current_max_vertex = 0;
    };
}