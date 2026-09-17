#pragma once
#include "engine/rendering/RenderingSystem.h"
#include <map>
#include <string>

class Action;
class Entity;
class GameEngine;

enum class InputDevice
{
    Keyboard,
    MouseButton,
    Gamepad
};

struct InputBinding
{
    InputDevice device;
    int code;

    bool operator<(const InputBinding& other) const
    {
        if (device != other.device)
            return device < other.device;

        return code < other.code;
    }
};

using ActionMap = std::map<InputBinding, std::string>;

class Scene
{
    int currentFrame_ = 0;
    ActionMap actionMap_;
    bool bIsPaused_ = false;
    bool bHasEnded_ = false;
    protected:
    GameEngine* gameEngine_ = nullptr;
    std::unique_ptr<IRenderingSystem> renderingSystem_;

public:
    explicit Scene(GameEngine* gameEngine, std::unique_ptr<IRenderingSystem> renderingSytem);
    virtual void init() = 0;
    virtual void destroy() = 0;
    virtual void update(float dt) = 0;
    virtual void sRender(float dt) = 0;
    virtual void sDebug() = 0;
    virtual void sDebugUI() {}
    virtual void sDoAction(const Action& action) = 0;

    void defaultEntityRender(float dt);
    void simulate(int);
    void doAction(const Action& action);
    void registerAction(InputDevice device, int code, const std::string& actionName);
    ActionMap& getActionMap();
};