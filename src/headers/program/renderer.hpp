#pragma once
#include <GL/glew.h>
#include <vector>
#include "mesh.hpp"
#include "camera3D.hpp" 
#include "shaderProgram.hpp"
#include "positionedLight.hpp"

class BaseRenderer
{
public:
    virtual void render() = 0;
    virtual ~BaseRenderer() = default;
};

class MeshRenderer : public BaseRenderer
{
public:
    MeshRenderer() = default;
    MeshRenderer(const std::vector<std::shared_ptr<Mesh>>& meshes_,
                 const std::shared_ptr<ShaderProgram>& shaderProgram_,
                 const std::shared_ptr<PositionedLight>& light_,
                 const std::shared_ptr<rendering::PerspectiveCamera>& camera_)
        : meshes(meshes_), shaderProgram(shaderProgram_), light(light_), camera(camera_) {}   
    void render() override
    {
        if(shaderProgram == nullptr || light == nullptr || camera == nullptr) {
            throw std::runtime_error("ShaderProgram, Light, or Camera not set in MeshRenderer");
        }
        shaderProgram->useProgram();

        // Scene-wide uniforms
        shaderProgram->setVec3("light.position", light->position);
        shaderProgram->setVec3("light.ambient", light->ambient);
        shaderProgram->setVec3("light.diffuse", light->diffuse);
        shaderProgram->setVec3("light.specular", light->specular);
        shaderProgram->setVec3("viewPos", camera->getViewPosition());
        shaderProgram->setMat4("view", camera->getView());
        shaderProgram->setMat4("projection", camera->getProjection());
        // Per-mesh
        for (const auto& mesh : meshes)
        {
            glBindVertexArray(mesh->getVAO());

            const auto& instances = mesh->getInstances();
            const auto& mvps = mesh->getInstanceMVPs();
            const auto& indices = mesh->getIndices();
            const auto mat = mesh->getDefaultMaterial();
            // Set default material properties
            shaderProgram->setVec3("material.ambient", mat->ambient);
            shaderProgram->setVec3("material.diffuse", mat->diffuse);
            shaderProgram->setVec3("material.specular", mat->specular);
            shaderProgram->setFloat("material.shininess", mat->shininess);

            for (size_t i = 0; i < instances.size(); i++)
            {
                shaderProgram->setMat4("model", instances[i]);

                glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
            }
        }
    }
    void setMeshes(const std::vector<std::shared_ptr<Mesh>>& meshes_)
    {
        meshes = meshes_;
    }
    void setShaderProgram(const std::shared_ptr<ShaderProgram>& shaderProgram_)
    {
        shaderProgram = shaderProgram_;
    }
    void setLight(const std::shared_ptr<PositionedLight>& light_)
    {
        light = light_;
    }
    void setCamera(const std::shared_ptr<rendering::PerspectiveCamera>& camera_)
    {
        camera = camera_;
    }
private:
    std::vector<std::shared_ptr<Mesh>> meshes;
    std::shared_ptr<ShaderProgram> shaderProgram;
    std::shared_ptr<PositionedLight> light;
    std::shared_ptr<rendering::PerspectiveCamera> camera;
};

class Renderer{
public:
    Renderer() = default;
    void render(const std::vector<std::shared_ptr<Mesh>>& meshes,
                ShaderProgram& shaderProgram,
                const PositionedLight& light,
                const rendering::PerspectiveCamera& camera)
    {
        shaderProgram.useProgram();

        // Scene-wide uniforms
        shaderProgram.setVec3("light.position", light.position);
        shaderProgram.setVec3("light.ambient", light.ambient);
        shaderProgram.setVec3("light.diffuse", light.diffuse);
        shaderProgram.setVec3("light.specular", light.specular);
        shaderProgram.setVec3("viewPos", camera.getViewPosition());
        shaderProgram.setMat4("view", camera.getView());
        shaderProgram.setMat4("projection", camera.getProjection());    
        // Per-mesh
        for (const auto& mesh : meshes)
        {
            glBindVertexArray(mesh->getVAO());

            const auto& instances = mesh->getInstances();
            const auto& mvps = mesh->getInstanceMVPs();
            const auto& indices = mesh->getIndices();
            const auto mat = mesh->getDefaultMaterial();
            // Set default material properties
            shaderProgram.setVec3("material.ambient", mat->ambient);
            shaderProgram.setVec3("material.diffuse", mat->diffuse);
            shaderProgram.setVec3("material.specular", mat->specular);
            shaderProgram.setFloat("material.shininess", mat->shininess);

            for (size_t i = 0; i < instances.size(); i++)
            {
                shaderProgram.setMat4("model", instances[i]);

                glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
            }
        }
    }
};

