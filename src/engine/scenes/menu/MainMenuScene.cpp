#include "MainMenuScene.h"
#include <SFML/Window/Keyboard.hpp>
#include <iostream>
#include "SFML/Graphics/Color.hpp"
#include "engine/actions/Action.h"
#include "engine/scenes/Gameplay/ScenePlay.h"
#include "engine/components/rendering/CSprite.h"
#include "engine/components/CTransform.h"
#include "engine/entities/EntityManager.h"
#include "engine/utils/math/Vector2.h"

MainMenuScene::MainMenuScene(GameEngine* gameEngine) : Scene(gameEngine) {
    registerAction(InputDevice::Keyboard, static_cast<int>(sf::Keyboard::Key::Enter), "UI_Enter");
    registerAction(InputDevice::Keyboard, static_cast<int>(sf::Keyboard::Key::Escape), "UI_Escape");
    registerAction(InputDevice::Keyboard, static_cast<int>(sf::Keyboard::Key::Up), "UI_Up");
    registerAction(InputDevice::Keyboard, static_cast<int>(sf::Keyboard::Key::Down), "UI_Down");
}

void MainMenuScene::init()
{
    const auto& card = EntityManager::getInstance().addEntity("card", gameEngine_->getCurrentSceneName(), "Fireball Card");
    card->addComponent<CTransform>(Vec2f(100, 100), Vec2f(0,0), 0, Vec2f(1,1));
    card->addRenderable<CSprite>(gameEngine_->getAssets().getTexture("fireballCardTexture"), Vec2f(200, 400), Vec2f(0,0));
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
    gameEngine_->getWindow().setView(gameEngine_->getWindow().getDefaultView());
    defaultEntityRender(dt);
}

void MainMenuScene::sDebug()
{
}

void MainMenuScene::sDoAction(const Action &action)
{
    if (action.name() == "UI_Enter") {
        gameEngine_->changeScene("gameplay_scene");
    }
}
