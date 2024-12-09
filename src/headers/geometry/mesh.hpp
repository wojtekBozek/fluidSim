#pragma once

#include <GL/glew.h>
#include <vector>
#include "vertex.hpp"                  


class Mesh
{
    public:
    Mesh();
    Mesh(const std::vector<vertex3D>& vertices, const std::vector<uint32_t>& indices);
    Mesh(vertex3D&& vertices);

    uint32_t addNewInstance(const glm::mat4& data);
    void detachInstance(uint32_t id);
    void removeInstances();
    const std::vector<uint32_t> &getDetachedInstances() const;//needed for indexing instances for object
    void computeMVPs(glm::mat4 (*func)(glm::mat4));
    void drawInstances(GLuint mvpLayout);
    void draw();
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
     * @param layoutUV 
     * @param layoutNormals 
     * @param layoutColor 
     */
    void bind(int layoutPos=-1, int layoutUV=-1, int layoutNormals=-1, int layoutColor=-1);
    private:                  
    rendering::VerticesContainer m_vertices;
    std::vector<uint32_t> m_indices;
    std::vector<glm::mat4> m_instances;
    std::vector<uint32_t> m_detachedInstances_id;
    std::vector<glm::mat4> m_instancesMVPs;
    glm::mat4 m_basic_instance = glm::mat4(1.0f);
    glm::mat4 m_basic_instance_mvp = glm::mat4(1.0f);
    uint32_t m_VAO, m_VBO, m_EBO;
};