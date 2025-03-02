#include "program/programManager.hpp"
#include "program/sceneLoader.hpp"


void ProgramManager::programLoop()
{
    
}

void ProgramManager::loadScene(std::string filePath)
{
    SceneLoader<SceneLoaderObj>::loadScene(filePath, 
        objectMeneger, meshes);
    for (auto& mesh : meshes)
    {
        mesh.bind(0,-1,-1,1);
    }


}

ProgramManager::ProgramManager()
{
}