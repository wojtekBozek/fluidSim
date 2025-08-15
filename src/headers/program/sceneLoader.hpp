#pragma once
#include <string>
#include <objectsMeneger.hpp>
#include "mesh_loader.hpp"

template <typename T>
class SceneLoader 
{
    public:
    static int loadScene(const std::string& dirPath, ObjectMenager& objectManager, std::vector<std::shared_ptr<Mesh>>& meshVector)
    {
        return T::loadScene(dirPath, objectManager, meshVector);
    }
};


class SceneLoaderObj
{
    public:
    /**
     * @brief loads all obj files from directory, names object with the name of file bstripted of full path and .obj
     * 
     * @param dirPath 
     * @param oobjectManager 
     * @param meshVector 
     * @return int 
     */
    static int loadScene(const std::string& dirPath, ObjectMenager& objectManager, std::vector<std::shared_ptr<Mesh>>& meshVector);
};