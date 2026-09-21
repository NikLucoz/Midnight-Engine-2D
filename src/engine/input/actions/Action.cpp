#include "Action.h"
#include "engine/utils/math/Vector2.h"

Action::Action() = default;

Action::Action(const std::string& name, const std::string& type, const Vector2<int> position)
    : name_(name), type_(type), pos_(Vector2<int>(0,0))
{
}