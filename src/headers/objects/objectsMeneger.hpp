#pragma once
#include <vector>
#include "object.hpp"
#include <unordered_map>
#include "properties.hpp"
class ObjectMenager
{
public:
ObjectMenager();
/**
 * @brief move funcion with three parameters
 * 
 * @param objectName objectName reference
 * @param axis axis we move through (will be normalized!!!)
 * @param magnitude absolute distance we want the objectName to be moved
 */
    void move(const std::string& objectName, glm::vec3 axis, float magnitude);

    /**
     * @brief move function with two parameters
     * 
     * @param objectName objectName name reference
     * @param axis axis with values we want objectName to be translated
     */
    void move(const std::string& objectName, glm::vec3 axis);

    void rotate(const std::string& objectName, glm::vec3 axis, float angle);
    /**
     * @brief add singular object
     * 
     * @param objectName 
     * @param object 
     * @return int -1 if object under given name already exsists, 0 on success
     */
    int addObject(const std::string& objectName, const Object& object);
    int removeObject(const std::string& objectName);
    
    int addProperty(const std::string& name, const std::string& propName, std::shared_ptr<Property> prop);
    void update(float deltaTime);
    void clearObjects();

private:
    std::unordered_map<std::string, size_t> m_object_map;
    std::vector<Object> m_objectVector;
};