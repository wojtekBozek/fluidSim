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
    m_objectVector.at(object_it->second).translate(axis); 
    m_objectVector.at(object_it->second).update();  
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
    m_objectVector.at(object_it->second).translate(axis); 
    m_objectVector.at(object_it->second).update(); 
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
    m_objectVector.at(object_it->second).rotate(angle, axis); 
    m_objectVector.at(object_it->second).update();
}

int ObjectMenager::addObject(const std::string &objectName,const Object& object)
{
    auto object_it = m_object_map.find(objectName);
    if(object_it == m_object_map.end())
    {
        m_objectVector.push_back(object);
        m_object_map.emplace(objectName, m_objectVector.size()-1);
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
    m_objectVector.erase(m_objectVector.begin() + object_it->second);
    m_object_map.erase(objectName);
    return 0;
}

int ObjectMenager::addProperty(const std::string& objectName, const std::string& propName, std::shared_ptr<Property> prop)
{
    auto object_it  = m_object_map.find(objectName);
    if(object_it == m_object_map.end())
    {
        std::cerr << "No object named: " << objectName << " to add properrty! Aborting\n";
        return -1;
    }
    m_objectVector.at(object_it->second).addProperty(propName, prop);
    return 0;
}

void ObjectMenager::update(float deltaTime)
{
    for(auto& object : m_objectVector)
    {
        object.updateProperties(deltaTime);
    }
}

void ObjectMenager::clearObjects()
{
    m_object_map.clear();
    m_objectVector.clear();
}
