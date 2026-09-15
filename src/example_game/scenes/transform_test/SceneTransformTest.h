#pragma once

#include "engine/scenes/Scene.h"
#include "example_game/entities/EPlayer.h"

class SceneTransformTest : public Scene
{
    std::shared_ptr<EPlayer> player_;
    std::shared_ptr<Entity> transformParent_;

public:
    explicit SceneTransformTest(GameEngine* gameEngine);

    void init() override;
    void destroy() override;
    void update(float dt) override;
    void sRender(float dt) override;
    void sDebug() override;
    void sDoAction(const Action& action) override;

private:
    void updatePlayer(float dt);
};