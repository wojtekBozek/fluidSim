#pragma once
#include "objects/objectsMeneger.hpp"
#include "geometry/mesh.hpp"
class ProgramManager
{
    public:
    ProgramManager();

    void programLoop();
    void loadScene(std::string filePath="C:/Users/Wojtek/Desktop/Programowanie/Magisterka/FluidSimulator/obj/");
    private:
    ObjectMenager objectMeneger;
    std::vector<Mesh> meshes;
};