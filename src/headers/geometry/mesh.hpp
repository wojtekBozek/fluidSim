#pragma once

#include <GL/glew.h>
#include <vector>
#include "geometry/vertex.hpp"                  


class Mesh
{
    public:
    Mesh();
    Mesh(const std::vector<vertex3D>& vertices, const std::vector<uint32_t>& indices);
    Mesh(vertex3D&& vertices);


    void drawInstances(GLuint mvpLayout);
    void draw();
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
    std::vector<glm::mat4> m_instancesMVPs;
    uint32_t m_VAO, m_VBO, m_EBO;
};