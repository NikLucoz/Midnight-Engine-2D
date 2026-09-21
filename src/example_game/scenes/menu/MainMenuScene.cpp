#include "MainMenuScene.h"
#include <SFML/Window/Keyboard.hpp>
#include <iostream>
#include "SFML/Graphics/Color.hpp"
#include "engine/entities/PrefabEntityLoader.h"
#include "example_game/scenes/Gameplay/ScenePlay.h"
#include "engine/components/rendering/CSprite.h"
#include "engine/components/CTransform.h"
#include "engine/entities/EntityManager.h"
#include "engine/utils/math/Vector2.h"

MainMenuScene::MainMenuScene(GameEngine* gameEngine) : Scene(gameEngine, nullptr) {
}

void MainMenuScene::init()
{
    const auto& card = EntityManager::getInstance().addEntity("card", "Fireball Card");
    card->setSceneName(gameEngine_->getCurrentSceneName());
    card->addComponent<CTransform>(Vec2f(100, 100), Vec2f(0,0), 0, Vec2f(1,1));
    card->addRenderable<CSprite>(Assets::getInstance().getTexture("fireballCardTexture"), Vec2f(200, 400), Vec2f(0,0));

    //const auto& player = PrefabEntityLoader::LoadEntity("playerEntity");
    //player->setSceneName(gameEngine_->getCurrentSceneName());
}

void MainMenuScene::destroy()
{
    for (auto& ePtr : EntityManager::getInstance().getEntitiesInScene(gameEngine_->getCurrentSceneName())) {
        Entity& e = *ePtr;
        e.destroy();
    }
}

void MainMenuScene::update(float dt)
{
}

void MainMenuScene::sRender(float dt)
{
    if (!gameEngine_->getDebugOptions().systems.render) return;

    RenderContext context{
    gameEngine_->getWindow(),
    EntityManager::getInstance().getEntities(),
    gameEngine_->getDebugOptions()
    };

    gameEngine_->getWindow().setView(gameEngine_->getWindow().getDefaultView());
    renderingSystem_->renderEntities(dt, context);
}

void MainMenuScene::sDebug()
{
}

void MainMenuScene::sDoAction(const Action &action)
{
    if (action.name() == "UI_Enter") {
        //gameEngine_->changeScene("gameplay_scene");
    }
}
