#pragma once
#include <glm/glm.hpp>
enum VERTEX_TYPE
{
    BASE = 0,
    POSCOL = 1,
    TEXCOL = 2
};


struct BaseVertex
{
    BaseVertex(glm::vec4 pos) : position(pos){type = VERTEX_TYPE::BASE;}
    BaseVertex(glm::vec3 pos) : position({pos,1.0}){type = VERTEX_TYPE::BASE;}
    glm::vec4 position;
    VERTEX_TYPE getType(){return type;}
    protected:
        VERTEX_TYPE type;
};

struct PosColVertex : BaseVertex
{
    PosColVertex(glm::vec4 pos, glm::vec4 col) : BaseVertex(pos){type = VERTEX_TYPE::POSCOL; color = col;}
    PosColVertex(glm::vec3 pos, glm::vec3 col) : BaseVertex(pos){type = VERTEX_TYPE::POSCOL; color = {col, 1.0f};}
    glm::vec4 color;
};

struct TexColVertex : PosColVertex
{
    TexColVertex(glm::vec4 pos, glm::vec4 col, glm::vec2 _tCord):PosColVertex(pos,col){type = VERTEX_TYPE::TEXCOL; texCord=_tCord;}
    TexColVertex(glm::vec3 pos, glm::vec3 col, glm::vec2 _tCord):PosColVertex(pos,col){type = VERTEX_TYPE::TEXCOL; texCord=_tCord;}
    glm::vec2 texCord;
};