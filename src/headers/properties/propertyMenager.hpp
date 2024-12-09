/*Unused

#pragma once
#include "propertyMenager.hpp"
#include "object.hpp"


class PropertyMenager
{
    public:
    virtual void applyProperty() = 0;
    virtual void addPropertyData(std::pair<std::shared_ptr<Object>, std::shared_ptr<Property>>) = 0;

    template <typename... Args>
    void applyProperty(void (Object::*func)(Args...), Object& obj, Args&&... args)
    {
        (obj.*func)(std::forward<Args>(args)...);
    }
};
*/