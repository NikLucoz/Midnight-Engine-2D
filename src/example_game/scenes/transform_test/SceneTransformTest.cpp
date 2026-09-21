#include "SceneTransformTest.h"

#include <SFML/Window/Keyboard.hpp>

#include "engine/GameEngine.h"
#include "engine/components/CTransform.h"
#include "engine/components/rendering/CShape.h"
#include "engine/entities/EntityManager.h"
#include "engine/entities/PrefabEntityLoader.h"
#include "example_game/components/CInput.h"


SceneTransformTest::SceneTransformTest(GameEngine *gameEngine) : Scene(gameEngine, nullptr) {

}

void SceneTransformTest::init() {
    const std::string &sceneName = gameEngine_->getCurrentSceneName();
    const Vec2f center = gameEngine_->getCamera().getPosition();

    player_ = PrefabEntityLoader::LoadEntity("PlayerPrefab");
    player_->setSceneName(sceneName);
    player_->getComponent<CTransform>().position = center;
    player_->getComponent<CRenderable>().setRenderLayer(1);
    player_->getComponent<CRenderable>().setOrderInLayer(999);
    gameEngine_->getCamera().setTarget(center);
    gameEngine_->getCamera().setFollowSmoothing(10.0f);

    transformParent_ = EntityManager::getInstance().addEntity("transform_parent", "RotatingParent");
    transformParent_->setSceneName(sceneName);
    transformParent_->addComponent<CTransform>(center + Vec2f(180.0f, 0.0f), Vec2f(0.0f, 0.0f), 0.0f, Vec2f(1.0f, 1.0f));
    transformParent_->addRenderable<CShape>(40, 8, sf::Color(70, 70, 80), sf::Color::White, 3);
    transformParent_->getComponent<CRenderable>().setRenderLayer(1);

    const auto redChild = EntityManager::getInstance().addEntity("transform_child", "RedChild");
    redChild->setSceneName(sceneName);
    redChild->addComponent<CTransform>(Vec2f(100.0f, 0.0f), Vec2f(0.0f, 0.0f), 0.0f, Vec2f(1.0f, 1.0f));
    redChild->addRenderable<CShape>(18, 6, sf::Color::Red, sf::Color::White, 2);

    const auto greenChild = EntityManager::getInstance().addEntity("transform_child", "GreenChild");
    greenChild->setSceneName(sceneName);
    greenChild->addComponent<CTransform>(Vec2f(0.0f, 80.0f), Vec2f(1.25f, 1.25f), 25.0f, Vec2f(1.0f, 1.0f));
    greenChild->addRenderable<CShape>(18, 6, sf::Color::Green, sf::Color::White, 2);

    const auto blueChild = EntityManager::getInstance().addEntity("transform_child", "BlueChild");
    blueChild->setSceneName(sceneName);
    blueChild->addComponent<CTransform>(Vec2f(-100.0f, 0.0f), Vec2f(0.8f, 0.8f), -25.0f, Vec2f(1.0f, 1.0f));
    blueChild->addRenderable<CShape>(18, 6, sf::Color::Blue, sf::Color::White, 2);

    transformParent_->addChild(*redChild);
    transformParent_->addChild(*greenChild);
    transformParent_->addChild(*blueChild);

    const auto marker = EntityManager::getInstance().addEntity("root_marker", "RootMarker");
    marker->setSceneName(sceneName);
    marker->addComponent<CTransform>(center + Vec2f(-220.0f, 0.0f), Vec2f(0.0f, 0.0f), 0.0f, Vec2f(1.0f, 1.0f));
    marker->addRenderable<CShape>(24, 4, sf::Color::Yellow, sf::Color::White, 2);
    marker->getComponent<CRenderable>().setRenderLayer(2);
}

void SceneTransformTest::destroy() {
    for (auto &entity : EntityManager::getInstance().getEntitiesInScene(gameEngine_->getCurrentSceneName())) {
        entity->destroy();
    }
}

void SceneTransformTest::update(float dt) {
    updatePlayer(dt);

    if (transformParent_ != nullptr) {
        auto &transform = transformParent_->getComponent<CTransform>();
        transform.rotation += 45.0f * dt;
        if (transform.rotation >= 360.0f)
            transform.rotation -= 360.0f;
    }
}

void SceneTransformTest::updatePlayer(float dt) {
    if (player_ == nullptr)
        return;

    auto &input = player_->getComponent<CInput>();
    auto &transform = player_->getComponent<CTransform>();
    const Vector2<int> direction = input.getMovementDirection();

    transform.position.x += transform.velocity.x * direction.x * dt;
    transform.position.y += transform.velocity.y * direction.y * dt;
    gameEngine_->getCamera().setTarget(transform.getPosition());
}

void SceneTransformTest::sRender(float dt) {
    if (!gameEngine_->getDebugOptions().systems.render)
        return;

    RenderContext context{gameEngine_->getWindow(), EntityManager::getInstance().getEntities(), gameEngine_->getDebugOptions()};

    renderingSystem_->renderEntities(dt, context);
}

void SceneTransformTest::sDebug() {}

void SceneTransformTest::sDoAction(const Action &action) {

}