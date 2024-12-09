#include <filesystem>
#include <iostream>
#include "sceneLoader.hpp"


namespace fs = std::filesystem;

int SceneLoaderObj::loadScene(const std::string &dirPath, ObjectMenager &objectManager, std::vector<Mesh> &meshVector)
{
    MeshLoader meshLoader;
    int objectCount = 0;

    // Check if the directory exists
    if (!fs::exists(dirPath) || !fs::is_directory(dirPath))
    {
        std::cerr << "Error: Directory " << dirPath << " does not exist or is not a directory.\n";
        return -1;
    }

    // Iterate through all files in the directory
    for (const auto& entry : fs::directory_iterator(dirPath))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".obj")
        {
            // Extract the filename without path and extension
            std::string filename = entry.path().stem().string();

            std::cout << "Loading OBJ file: " << entry.path() << std::endl;

            // Load the mesh
            //Mesh mesh = meshLoader.load(entry.path().string());
            meshVector.push_back(meshLoader.load(entry.path().string()));
            std::shared_ptr<Mesh> mesh_ptr = std::shared_ptr<Mesh>(&(meshVector).back());
            // Create an Object for this mesh
            auto newObject = std::make_unique<Object>(mesh_ptr);
            std::cout << mesh_ptr.get() << " mesh pointer" << &(meshVector.back()) << "\n";
            // Add the object to the ObjectManager
            if (objectManager.addObject(filename, std::move(newObject)) == 0)
            {
                std::cout << "Added object: " << filename << std::endl;
                objectCount++;
            }
            else
            {
                std::cerr << "Warning: Object with name '" << filename << "' already exists. Skipping.\n";
            }
        }
    }

    std::cout << "Scene loading complete. " << objectCount << " objects loaded.\n";
    return objectCount;
}
