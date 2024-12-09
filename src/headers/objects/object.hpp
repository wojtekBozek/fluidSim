#pragma once
#include "mesh.hpp"
#include "physics.hpp"
#include <unordered_map>
class Object {
	public:
		Object(std::shared_ptr<Mesh> mesh_ptr_)
		{
			mesh_ptr = mesh_ptr_;
			std::cout << mesh_ptr.get() << " object Mesh ptr\n";
			default();
			instance_id = mesh_ptr->addNewInstance(translation * rotation * scaling);
		}
		void default();
		void uniform_scale(float size, glm::vec3 axes);
		void scale(glm::vec3 scaling_vec);
		void translate(glm::vec3 translate);
		void rotate(float angle, glm::vec3 axis);
		void setTranslation(glm::vec3 translate);
		void update();
		void hide();

		void addProperty(std::string name, std::shared_ptr<Property> property);
		void detachProperty(std::string name);
		template <typename T>
    	std::shared_ptr<T> getProperty(const std::string& name) \
		{
        	if (m_properties.find(name) != m_properties.end()) 
			{
        	    return std::dynamic_pointer_cast<T>(m_properties[name]);
        	}
        	return nullptr;
    	}
		void updateProperties(float timeDelta);
		void changeMesh(std::shared_ptr<Mesh> mesh_ptr_);
		~Object();
	private:
		bool visible = true;
		std::shared_ptr<Mesh> mesh_ptr;
		uint32_t instance_id;
		glm::mat4 scaling, rotation, translation;
		std::unordered_map<std::string, std::shared_ptr<Property>> m_properties;
	};