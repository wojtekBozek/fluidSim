#pragma once
#include <vertex.hpp>
#include <memory>
#include <vector>

class MaterialContainer {
 public:
     MaterialContainer() = default;

      void addMaterial(std::shared_ptr<Material> material) {
        materials.push_back(std::move(material));
    }

    const std::shared_ptr<Material>& getMaterial(size_t index) const {
        if (index >= materials.size()) {
            throw std::out_of_range("Material index out of range");
        }
        return materials[index];
    }

    size_t getMaterialCount() const { return materials.size(); }

 private:
     std::vector<std::shared_ptr<Material>> materials;
 };