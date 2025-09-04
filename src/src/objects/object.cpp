#include "object.hpp"
#include "quaternion.hpp"
#include "physics.hpp"

void Object::default()
{
	translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));
	scaling = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
	rotation = quaternionRotation(0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
}

void Object::uniform_scale(float size, glm::vec3 axes)
{
    scaling = glm::scale(glm::mat4(1.0f), size * glm::normalize(axes));
}

void Object::scale(glm::vec3 scaling_vec)
{
    scaling = glm::scale(glm::mat4(1.0f), scaling_vec);
}

void Object::translate(glm::vec3 translate_vec)
{
    translation = glm::translate(glm::mat4(1.0f), translate_vec);
}

void Object::rotate(float angle, glm::vec3 axis)
{
    glm::mat4 incrementalRotation = quaternionRotation(glm::radians(angle), glm::normalize(axis));
    rotation = incrementalRotation * rotation; // Update rotation cumulatively
}

void Object::setTranslation(glm::vec3 translate_vec)
{
    translation = glm::translate(glm::mat4(1.0f), translate_vec);
    if(this->getProperty<PhysicalProperties>("physics") != nullptr)
    {
        this->getProperty<PhysicalProperties>("physics")->forcePosition(translate_vec);
    }
}

void Object::updateProperties(float timeDelta)
{
     if(this->getProperty<PhysicalProperties>("physics") != nullptr)
    {
        this->getProperty<PhysicalProperties>("physics")->updatePhysics(timeDelta);
    }
}

void Object::update()
{
    if(this->getProperty<PhysicalProperties>("physics") != nullptr)
    {
        this->translate(this->getProperty<PhysicalProperties>("physics")->getPosition());
    }
    mesh_ptr->updateInstanceMatrix(instance_id, translation * rotation * scaling);
}

void Object::hide()
{
	visible = (visible == true) ? false : true;
}

void Object::addProperty(std::string name, std::shared_ptr<Property> property)
{
    m_properties[name] = property;
}

void Object::detachProperty(std::string name)
{
    m_properties.erase(name);
}

void Object::changeMesh(std::shared_ptr<Mesh> mesh_ptr_)
{
    if(mesh_ptr_ != mesh_ptr)
    {
        uint32_t new_id = mesh_ptr_->addNewInstance(mesh_ptr->modifyInstance(instance_id));
        mesh_ptr->detachInstance(instance_id);
        instance_id = new_id;
        mesh_ptr = mesh_ptr_;
    }
}
Object::~Object()
{
    std::cout << "Deleteing object: " << mesh_ptr.use_count() << "\n";
    m_properties.clear();
    mesh_ptr->detachInstance(instance_id);
}