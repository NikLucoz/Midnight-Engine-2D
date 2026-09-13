#pragma once
#include "engine/scenes/scene.h"

class Action;

class MainMenuScene : public Scene
{ 
    public:
    MainMenuScene(GameEngine* gameEngine);
    void init() override;
    void destroy() override;
    void update(float dt) override;
    void sRender(float dt) override;
    void sDebug() override;
    void sDoAction(const Action& action) override;
};