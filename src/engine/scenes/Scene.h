#pragma once
#include "SFML/Window/Joystick.hpp"
#include "engine/input/InputManager.h"
#include "engine/rendering/RenderingSystem.h"
#include <string>

class Action;
class Entity;
class GameEngine;

class Scene
{
    int currentFrame_ = 0;
    bool bIsPaused_ = false;
    bool bHasEnded_ = false;
protected:
    GameEngine* gameEngine_ = nullptr;
    std::unique_ptr<IRenderingSystem> renderingSystem_;

public:
    explicit Scene(GameEngine* gameEngine, std::unique_ptr<IRenderingSystem> renderingSystem = nullptr);
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
};