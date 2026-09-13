#pragma once
#include "engine/utils/math/Vector2.h"
#include <string>

class Action
{
    std::string name_ = "NONE";
    std::string type_ = "NONE";
    Vector2<int> pos_;
public:
    Action();
    Action(const std::string& name, const std::string& type, const Vector2<int> position = Vector2(0,0));

    const std::string& name() const
    {
        return name_;
    }

    const std::string& type() const
    {
        return type_;
    }

    const Vector2<int> position() const {
        return pos_;
    }

    std::string toString() const
    {
        return name_ + ": " + type_;
    }
};
