#include "objectsMeneger.hpp"

ObjectMenager::ObjectMenager()
{
}

void ObjectMenager::move(const std::string &object, glm::vec3 axis, float magnitude)
{
    auto object_it  = m_object_map.find(object);
    if(object_it == m_object_map.end())
    {
        std::cerr << "No object named: " << object << " found. Aborting operation move\n";
        return;
    }
    if(magnitude == 0 || glm::length(axis) == 0.0f)
    {
        std::cerr << "Magnitude = " << magnitude << " or len(axis) = " << glm::length(axis) << " invalid. Aborting operation move.\n";
        return;
    }
    axis = glm::normalize(axis);
    axis = axis * magnitude;
    object_it->second->translate(axis); 
    object_it->second->update();  
}

void ObjectMenager::move(const std::string &object, glm::vec3 axis)
{
    auto object_it  = m_object_map.find(object);
    if(object_it == m_object_map.end())
    {
        std::cerr << "No object named: " << object << " found. Aborting operation move\n";
        return;
    }
    if(glm::length(axis) == 0.0f)
    {
        std::cerr << "Len(axis) = " << glm::length(axis) << " invalid. Aborting operation move.\n";
        return;
    }
    object_it->second->translate(axis); 
    object_it->second->update(); 
}

void ObjectMenager::rotate(const std::string &objectName, glm::vec3 axis, float angle)
{
    auto object_it  = m_object_map.find(objectName);
    if(object_it == m_object_map.end())
    {
        std::cerr << "No object named: " << objectName << " found. Aborting operation move\n";
        return;
    }
    if(glm::length(axis) == 0.0f)
    {
        std::cerr << "Len(axis) = " << glm::length(axis) << " invalid. Aborting operation move.\n";
        return;
    }
    object_it->second->rotate(angle, axis); 
    object_it->second->update();
}

int ObjectMenager::addObject(const std::string &objectName, std::unique_ptr<Object> object)
{
    auto object_it = m_object_map.find(objectName);
    if(object_it == m_object_map.end())
    {
        m_object_map.emplace(objectName, std::move(object));
        return 0;
    }
    return -1;
}

int ObjectMenager::removeObject(const std::string &objectName)
{
    auto object_it = m_object_map.find(objectName);
    if(object_it == m_object_map.end())
    {
        std::cerr << "No object named: " << objectName << " to be deleted! Aborting deletion\n";
        return -1;
    }
    m_object_map.erase(objectName);
    return 0;
}
