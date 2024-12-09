#pragma once
#include "mesh.hpp"
#include "vertex.hpp"
#include "vertex_hashing.hpp"
#include <map>
#include <tiny_obj_loader.h>
class MeshLoader {
public:
    MeshLoader() {}
    Mesh load(std::string filepath);
    void clean() 
    {
        m_tempVertices.clear();
        m_tempMaterial.clear();
        m_shapes.clear();
        m_materials.clear();
        m_warnings.clear();
        m_errors.clear();
    }
private:
	std::vector<vertex3D> m_tempVertices;
    std::vector<material> m_tempMaterial;
    tinyobj::attrib_t m_attributes;
    std::vector<tinyobj::shape_t> m_shapes;
    std::vector<tinyobj::material_t> m_materials;
    std::string m_warnings;
    std::string m_errors;
};