#pragma once

#include <GL/glew.h>
#include <vector>
#include "vertex.hpp"                  
#include "shaderProgram.hpp"
#include "positionedLight.hpp"
class Mesh
{
    public:
    Mesh();
    Mesh(const std::vector<vertex3D>& vertices, const std::vector<uint32_t>& indices);
    Mesh(vertex3D&& vertices);

    Mesh(const Mesh& other) 
        : m_vertices(other.m_vertices), m_indices(other.m_indices) 
    {
        std::cout << "Mesh Copy Constructor Called" << std::endl;
        
        // Generate new OpenGL resources (deep copy)
        glGenVertexArrays(1, &(this->m_VAO));
        glGenBuffers(1, &(this->m_VBO));
        glGenBuffers(1, &(this->m_EBO));
    }

    Mesh(Mesh&& other) noexcept
        : m_VAO(other.m_VAO), m_VBO(other.m_VBO), m_EBO(other.m_EBO),
          m_vertices(std::move(other.m_vertices)), m_indices(std::move(other.m_indices)) 
    {
        std::cout << "Mesh Move Constructor Called" << std::endl;

        // Invalidate the OpenGL resources of 'other' to avoid double deletion
        other.m_VAO = 0;
        other.m_VBO = 0;
        other.m_EBO = 0;
    }

    Mesh& operator=(Mesh&& other) noexcept {
        std::cout << "Mesh Move Assignment Operator Called" << std::endl;

        if (this != &other) { // Prevent self-assignment
            // Free existing OpenGL resources
            if (m_VAO) glDeleteVertexArrays(1, &(this->m_VAO));
            if (m_VBO) glDeleteBuffers(1, &(this->m_VBO));
            if (m_EBO) glDeleteBuffers(1, &(this->m_EBO));
 
            // Move ownership of OpenGL resources
            m_VAO = other.m_VAO;
            m_VBO = other.m_VBO;
            m_EBO = other.m_EBO;

            // Move vertex/index data
            m_vertices = std::move(other.m_vertices);
            m_indices = std::move(other.m_indices);

            // Invalidate `other`'s OpenGL handles to prevent double deletion
            other.m_VAO = 0;
            other.m_VBO = 0;
            other.m_EBO = 0;
        }
        return *this;
    }


    GLuint getVAO() const { return m_VAO; }
    const std::vector<glm::mat4>& getInstances() const { return m_instances; }
    const std::vector<glm::mat4>& getInstanceMVPs() const { return m_instancesMVPs; }
    const std::vector<unsigned int>& getIndices() const { return m_indices; }
    const std::shared_ptr<const Material>& getDefaultMaterial() const { return m_defaultMaterial; }

    ~Mesh();
    uint32_t addNewInstance(const glm::mat4& data = glm::mat4(1.0f));
    void detachInstance(uint32_t id);
    void removeInstances();
    const std::vector<uint32_t> &getDetachedInstances() const;//needed for indexing instances for object
    void computeMVPs(glm::mat4 (*func)(glm::mat4));
    void drawInstances(GLuint mvpLayout);
    void draw();
    void draw(ShaderProgram& shaderProgram, const PositionedLight& light, const glm::vec3& viewPos);
    void draw(GLuint mvpLayout, glm::mat4 (*func)(glm::mat4));
    /**
     * @brief Get a reference to the transformation matrix of a specific instance.
     *        If the ID is invalid, throws an exception.
     * 
     * @param id The ID of the instance to modify.
     * @return A reference to the instance's transformation matrix.
     */
    glm::mat4& modifyInstance(uint32_t id);

    /**
     * @brief Update the transformation matrix of a specific instance.
     *        Ensures any related data (e.g., MVPs) is also updated.
     * 
     * @param id The ID of the instance to update.
     * @param transform The new transformation matrix.
     */
    void updateInstanceMatrix(uint32_t id, const glm::mat4& transform);
    /**
     * @brief In some cases we will have shaders where for example color is ommited, as shader would use material properites
     * but sometimes it will be needed, for example in plain view, like Blender layout editor. Thus we want to initialize all 
     * layout positions as -1, which denotes them as unused, otherwise means their position. 
     * Then some higher level object would modify bindings in case of shader change. 
     * 
     * @param layoutPos 
     * @param layoutNormals 
     */
    void bind(int layoutPos=-1, int layoutNormals=-1);
    private:                  
    rendering::VerticesContainer m_vertices;
    std::vector<uint32_t> m_indices;
    std::vector<glm::mat4> m_instances;
    std::vector<glm::mat4> m_instancesMVPs;
    std::vector<std::shared_ptr<const Material>> m_materials;
    std::vector<uint32_t> m_detachedInstances_id;
    glm::mat4 m_basic_instance = glm::mat4(1.0f);
    glm::mat4 m_basic_instance_mvp = glm::mat4(1.0f);
    std::shared_ptr<const Material> m_defaultMaterial = std::make_shared<Material>(Material({0.6f, 0.7f, 0.1f}, {0.2f, 0.8f, 0.1f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 32.0f));
    GLuint m_VAO = 0, m_VBO = 0, m_EBO = 0;
};