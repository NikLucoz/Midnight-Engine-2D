#pragma once
#include "engine/utils/math/Vector2.h"
#include <string>

enum class ActionType {
    None,
    Digital, // button / key
    Axis1D,  // single axis (trigger, one stick axis…)
    Axis2D   // full stick
};

class Action {
    std::string name_ = "NONE";
    ActionType type_ = ActionType::None;

    // Digital
    bool pressed_ = false;  // true on the frame it went down
    bool released_ = false; // true on the frame it went up
    bool held_ = false;     // true while the button is down

    // Analog
    float axis1D_ = 0.f;
    Vec2f axis2D_ = {0.f, 0.f};

    Vector2<int> pos_{0, 0};

  public:
    Action() = default;

    Action(const std::string &name, bool pressed, bool released, bool held, Vector2<int> pos = {0, 0})
        : name_(name), type_(ActionType::Digital), pressed_(pressed), released_(released), held_(held), pos_(pos) {}

    Action(const std::string &name, float value) : name_(name), type_(ActionType::Axis1D), axis1D_(value) {}

    Action(const std::string &name, Vec2f value) : name_(name), type_(ActionType::Axis2D), axis2D_(value) {}

    const std::string &name() const { return name_; }
    ActionType type() const { return type_; }

    bool isPressed() const { return pressed_; }
    bool isReleased() const { return released_; }
    bool isHeld() const { return held_; }

    float axis1D() const { return axis1D_; }
    Vec2f axis2D() const { return axis2D_; }

    Vector2<int> position() const { return pos_; }

    std::string typeString() const {
        switch (type_) {
        case ActionType::Digital:
            return pressed_ ? "pressed" : (released_ ? "released" : "held");
        case ActionType::Axis1D:
            return "axis1d";
        case ActionType::Axis2D:
            return "axis2d";
        default:
            return "none";
        }
    }

    std::string toString() const { return name_ + ": " + typeString(); }
};