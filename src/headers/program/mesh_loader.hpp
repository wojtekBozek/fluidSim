#pragma once
#include "mesh.hpp"
#include "vertex.hpp"
#include "vertex_hashing.hpp"
#include <map>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
class MeshLoader {
public:
    MeshLoader() {}
    Mesh load(std::string filepath) 
    {
        std::unordered_map<size_t, vertex3D> vertex_map;
        std::unordered_map<size_t, uint32_t> indices_map;
        std::vector<uint32_t> indices;
        int index = 0;
        std::string inputfile = filepath;
        unsigned long pos = inputfile.find_last_of("/");
        std::string mtlbasepath = inputfile.substr(0, pos + 1);
        bool ret = tinyobj::LoadObj(&m_attributes, &m_shapes, &m_materials, &m_errors, inputfile.c_str(), mtlbasepath.c_str(), true);
        if (!m_warnings.empty()) {
            std::cerr << "Warning: " << m_warnings << std::endl;
        }

        if (!m_errors.empty()) {
            std::cerr << "Error: " << m_errors << std::endl;
        }

        if (!ret) {
            exit(1);
        }
        // Loop over shapes
        for (size_t s = 0; s < m_shapes.size(); s++) {
            // Loop over faces(triangles)
            size_t index_offset = 0;
            for (size_t f = 0; f < m_shapes[s].mesh.num_face_vertices.size(); f++) {
                int fv = m_shapes[s].mesh.num_face_vertices[f];
                int idm = m_shapes[s].mesh.material_ids[f];
                tinyobj::real_t Ka[3] = { m_materials[idm].ambient[0],m_materials[idm].ambient[1],m_materials[idm].ambient[2], };
                tinyobj::real_t Kd[3] = { m_materials[idm].diffuse[0],m_materials[idm].diffuse[1],m_materials[idm].diffuse[2] };
                tinyobj::real_t Ks[3] = { m_materials[idm].specular[0],m_materials[idm].specular[1],m_materials[idm].specular[2] };
                tinyobj::real_t Ke[3] = { m_materials[idm].emission[0],m_materials[idm].emission[1],m_materials[idm].emission[2] };
                material color;
                color.ambient = glm::vec3(Ka[0], Ka[1], Ka[2]);
                color.diffuse = glm::vec3(Kd[0], Kd[1], Kd[2]);
                color.specular =glm::vec3(Ks[0], Ks[1], Ks[2]);
                color.emissive =glm::vec3(Ke[0], Ke[1], Ke[2]);
                m_tempMaterial.push_back(color);
                // Loop over vertices in the face.
                for (size_t v = 0; v < fv; v++) {
                    // access to vertex
                    vertex3D vert;

                    tinyobj::index_t idx = m_shapes[s].mesh.indices[index_offset + v];
                    tinyobj::real_t vx = m_attributes.vertices[3 * idx.vertex_index + 0];
                    tinyobj::real_t vy = m_attributes.vertices[3 * idx.vertex_index + 1];
                    tinyobj::real_t vz = m_attributes.vertices[3 * idx.vertex_index + 2];
                    if (idx.normal_index >= 0) {
                        tinyobj::real_t nx = m_attributes.normals[3 * idx.normal_index + 0];
                        tinyobj::real_t ny = m_attributes.normals[3 * idx.normal_index + 1];
                        tinyobj::real_t nz = m_attributes.normals[3 * idx.normal_index + 2];
                        vert.normal = glm::vec3(nx, ny, nz);
                    }
                    if (idx.texcoord_index >= 0) {
                        tinyobj::real_t tx = m_attributes.texcoords[2 * idx.texcoord_index + 0];
                        tinyobj::real_t ty = m_attributes.texcoords[2 * idx.texcoord_index + 1];
                        vert.uv = glm::vec2(tx, ty);
                    }

                    vert.xyz = glm::vec3(vx, vy, vz);
                    size_t newhash = wojtek_math::vertexHash(vert);
                    auto it = vertex_map.find(newhash);
                    if (it != vertex_map.end()) {
                        // Vertex already exists, return its index
                        std::cout << "Vertex already exists at index: " << it->first << "\n";
                        indices.push_back(indices_map.at(newhash));
                    }
                    else {
                        // Vertex does not exist, add it to the container
                        vertex_map[newhash] = vert;
                        indices_map[newhash] = index;
                        m_tempVertices.push_back(vert);
                        indices.push_back(index);
                        std::cout << "Added new vertex at index: " << index << std::endl;
                        index++;
                    }
                    //temp_vertices.push_back(vert);

                }
                index_offset += fv;
            }
        }
        Mesh mesh(m_tempVertices,indices);
        clean();
        return mesh;
    }
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