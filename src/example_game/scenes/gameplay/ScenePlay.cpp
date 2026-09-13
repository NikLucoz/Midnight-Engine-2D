#include "ScenePlay.h"

#include <imgui.h>
#include <SFML/Graphics.hpp>
#include "engine/actions/Action.h"
#include "engine/components/collision/CCircleCollider.h"
#include "engine/entities/EntityManager.h"
#include "example_game/entities/EPlayer.h"
#include "example_game/components/CLifespan.h"
#include "example_game/components/CSpecialBullet.h"
#include "engine/utils/physics/CollisionUtils.h"
#include <engine/utils/assets/TileMapLoader.h>
#include <engine/utils/physics/Raycast2D.h>

ScenePlay::ScenePlay(GameEngine* gameEngine, float enemySpawnTime) : Scene(gameEngine), enemySpawnMaxTime(enemySpawnTime)
{
    elapsedTimeSinceLastEnemySpawn_ = enemySpawnTime;
    registerAction(InputDevice::Keyboard, static_cast<int>(sf::Keyboard::Key::W), "Move_Up");
    registerAction(InputDevice::Keyboard, static_cast<int>(sf::Keyboard::Key::S), "Move_Down");
    registerAction(InputDevice::Keyboard, static_cast<int>(sf::Keyboard::Key::A), "Move_Left");
    registerAction(InputDevice::Keyboard, static_cast<int>(sf::Keyboard::Key::D), "Move_Right");
    registerAction(InputDevice::MouseButton, static_cast<int>(sf::Mouse::Button::Left), "Shoot");
    registerAction(InputDevice::MouseButton, static_cast<int>(sf::Mouse::Button::Right), "SpecialShoot");
}

void ScenePlay::init()
{
    tilemap_ = TileMapLoader::load("game/levels/example_level.cfg");

    const sf::Vector2f viewSize = gameEngine_->getCamera().getViewSize();
    const Vec2f cameraPosition = gameEngine_->getCamera().getPosition();

    Vec2f startPos(
        cameraPosition.x,
        cameraPosition.y
    );

    player_ = EntityManager::getInstance().addEntity<EPlayer>("player", gameEngine_->getCurrentSceneName(), "PlayerEntity");
    player_->getComponent<CTransform>().position = startPos;
    player_->startPosition = startPos;
    gameEngine_->getCamera().setTarget(startPos);
    gameEngine_->getCamera().setFollowSmoothing(10.0f);

    //const auto& gumba = EntityManager::getInstance().addEntity("enemy");
    //gumba->addComponent<CTransform>(Vec2f(300.0f, 300.0f), Vec2f(100.0f, 0.0f), 0.0f, Vec2f(6,6));
    //gumba->addRenderable<CAnimatedSprite>(gameEngine_->getAssets().getAnimation("gumbaWalkingAnimation"));
    //gumba->addComponent<CBoundingBox>(Vector2<int>(16*6, 16*6));

    auto e = EntityManager::getInstance().addEntity("enemy", gameEngine_->getCurrentSceneName(), "EnemyRaycastTest");
    e->addComponent<CTransform>(Vec2f(600, 600), Vec2f(0, 0), 0, Vec2f(1,1));
    e->addRenderable<CShape>(25, 4, sf::Color::Green, sf::Color::White, 4);
    e->addComponent<CBoundingBox>(Vector2<int>(50,50));
}

void ScenePlay::destroy()
{
    for (auto& ePtr : EntityManager::getInstance().getEntitiesInScene(gameEngine_->getCurrentSceneName())) {
        Entity& e = *ePtr;
        e.destroy();
    }
}

void ScenePlay::update(float dt)
{
    sMovement(dt);
    sCollision();
    sLifespan(dt);
    sEnemySpawner(dt);
    sAnimation(dt);

    /*
    auto hit = Raycast2D::castRay(
        player_->getComponent<CTransform>().getPosition(),
        Vec2f::RIGHT(),
        100.0f,
        player_.get()
    );
    */
}

void ScenePlay::sRender(float dt)
{   
    
    if (!gameEngine_->getDebugOptions().systems.render) return;
    
    RenderContext context{
    gameEngine_->getWindow(),
    gameEngine_->getAssets(),
    gameEngine_->getDebugOptions()
    };
    
    renderingSystem_->renderTilemapLayer(context, tilemap_, "ground");
    renderingSystem_->renderEntities(dt, context);
    
    if (gameEngine_->getDebugOptions().showCollisionGeometry) sDebug();
}

void ScenePlay::sDoAction(const Action& action)
{
    if (action.name() == "Move_Up")
    {
        player_->getComponent<CInput>().bUp = action.type() == "pressed" ? true : false;
    }
    
    if (action.name() == "Move_Down")
    {
        player_->getComponent<CInput>().bDown = action.type() == "pressed" ? true : false;
    }
    
    if (action.name() == "Move_Left")
    {
        player_->getComponent<CInput>().bLeft = action.type() == "pressed" ? true : false;
    }
    
    if (action.name() == "Move_Right")
    {
        player_->getComponent<CInput>().bRight = action.type() == "pressed" ? true : false;
    }

    if (action.name() == "Shoot")
    {
        player_->getComponent<CInput>().bMouseLeft = action.type() == "pressed" ? true : false;
    }
    
    if (action.name() == "SpecialShoot")
    {
        player_->getComponent<CInput>().bMouseRight = action.type() == "pressed" ? true : false;
    }
}

void ScenePlay::sAnimation(float dt)
{
    if (!gameEngine_->getDebugOptions().systems.animation) return;

    for (std::shared_ptr<Entity>& ePtr : EntityManager::getInstance().getEntities())
    if (ePtr->hasComponent<CAnimatedSprite>()) {
        auto& cAnimatedSprite = ePtr->getComponent<CAnimatedSprite>();
        cAnimatedSprite.animation->update(dt); 
    }
}

void ScenePlay::sMovement(float dt)
{    
    if (!gameEngine_->getDebugOptions().systems.movement) return;

    if (player_ == nullptr) return;
   
    auto& input = player_->getComponent<CInput>();
    if (input.hasMovementInput())
    {
        auto& transform = player_->getComponent<CTransform>();
        Vector2<int> movementDirection = input.getMovementDirection();
        transform.position.x += transform.velocity.x * movementDirection.x * dt;
        transform.position.y += transform.velocity.y * movementDirection.y * dt;
    }

    gameEngine_->getCamera().setTarget(player_->getComponent<CTransform>().getPosition());
    
    std::vector<std::string> tags{"bullet", "enemy"};
    auto movable_entities = EntityManager::getInstance().getEntities(tags);
    for (auto& e : movable_entities)
    {
        if (!e->hasComponent<CTransform>()) continue;
        auto& transform = e->getComponent<CTransform>();
        transform.position.y += transform.velocity.y * dt;
        transform.position.x += transform.velocity.x * dt;
    }
    
    tags = {"specialBullet"};
    auto specialBullets = EntityManager::getInstance().getEntities(tags);
    for (auto& sb : specialBullets)
    {
        if (!sb->hasComponent<CTransform>()) continue;
        
        if (!sb->hasComponent<CSpecialBullet>())
            sb->addComponent<CSpecialBullet>();
    
        auto& bulletData = sb->getComponent<CSpecialBullet>();
        bulletData.oscillationTime += dt;
        float oscillationAmplitude = 15.0f;
        
        float sinValue = std::sin(bulletData.oscillationTime * 8.0f);
        
        auto& transform = sb->getComponent<CTransform>();
        float velocityMagnitude = transform.velocity.magnitude();
    
        if (velocityMagnitude > 0.0f)
        {
            float dirX = transform.velocity.x / velocityMagnitude;
            float dirY = transform.velocity.y / velocityMagnitude;
            
            // per applicare l'oscillazione del sin devo ruotare di 90° in senso orario
            Vec2f rotatedDirection = Vec2f(dirY, -dirX);
            Vec2f oscillation = rotatedDirection * oscillationAmplitude * sinValue;
            
            transform.position.x += oscillation.x + transform.velocity.x * dt;
            transform.position.y += oscillation.y + transform.velocity.y * dt;
        }
    }
}

void ScenePlay::sEnemySpawner(float dt)
{
    if (!gameEngine_->getDebugOptions().systems.enemySpawner) return;

    elapsedTimeSinceLastEnemySpawn_ -= dt;
    if (elapsedTimeSinceLastEnemySpawn_ <= 0.0f) {
        srand(time(0)*dt);
        float randomX = static_cast<float>(rand() % gameEngine_->getWindow().getSize().x);
        float randomY = static_cast<float>(rand() %  gameEngine_->getWindow().getSize().y);
        float randomAngle = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f * 3.14f;

        float speed = 300.0f;
        float velX = std::cos(randomAngle) * speed;
        float velY = std::sin(randomAngle) * speed;
        
        int randomPoints = 3 + rand() % (6 + 1);
        
        int r = rand() % 255;
        int g = rand() % 255;
        int b = rand() % 255;

        auto e = EntityManager::getInstance().addEntity("enemy", gameEngine_->getCurrentSceneName(), "MovingEnemy");
        e->addComponent<CTransform>(Vec2f(randomX, randomY), Vec2f(velX, velY), 0, Vec2f(1,1));
        e->addRenderable<CShape>(25, randomPoints, sf::Color(r, g, b), sf::Color::White, 4);
        e->addComponent<CBoundingBox>(Vector2<int>(50,50));

        elapsedTimeSinceLastEnemySpawn_ = enemySpawnMaxTime;
    }
}

void ScenePlay::sCollision()
{
    if (!gameEngine_->getDebugOptions().systems.collision) return;

    auto& enemies = EntityManager::getInstance().getEntities("enemy");
    
    std::vector<std::string> tags{"bullet", "specialBullet"};
    auto bullets = EntityManager::getInstance().getEntities(tags);
    
    if (player_ == nullptr) return;
    
    auto& pTransform = player_->getComponent<CTransform>();
    auto& pCollider = player_->getComponent<CBoundingBox>();
    
    for (auto& enemy : enemies)
    {
        
        auto& eTransform = enemy->getComponent<CTransform>();
        auto& eBoundingBox = enemy->getComponent<CBoundingBox>();
        if (!enemy->hasComponent<CBoundingBox>()) continue;
        if (IsColliding(player_.get(), enemy.get()))
        {
            spawnEnemyDeathParticles(enemy.get());
            enemy->destroy();
            pTransform.position = player_->startPosition;
            break;
        }

        for (auto& bullet : bullets)
        {
            if (!bullet->hasComponent<CCircleCollider>()) continue;
            auto& bTransform = bullet->getComponent<CTransform>();
            auto& bCollider = bullet->getComponent<CBoundingBox>();
            
            float distanceToBullet = eTransform.getPosition().distanceToSquared(bTransform.getPosition());
             if (IsColliding(enemy.get(), bullet.get()))
            {
                spawnEnemyDeathParticles(enemy.get());
                enemy->destroy();
                bullet->destroy();
                break;
            }
        }
        
        if (eTransform.getPosition().x - eBoundingBox.size.x < 0 ||
            eTransform.getPosition().x + eBoundingBox.size.x > gameEngine_->getWindow().getSize().x)
        {
            eTransform.velocity = eTransform.velocity.reflectionVector({1.0f, 0.0f});
        }
        
        // Horizontal wall collision
        if (eTransform.getPosition().y - eBoundingBox.size.y < 0 ||
            eTransform.getPosition().y + eBoundingBox.size.y > gameEngine_->getWindow().getSize().y)
        {
            eTransform.velocity = eTransform.velocity.reflectionVector({0.0f, 1.0f});
        }
    }
}

void ScenePlay::sLifespan(float dt)
{
    if (!gameEngine_->getDebugOptions().systems.lifespan) return;

    auto& entities = EntityManager::getInstance().getEntities();
    
    for (auto& entity : entities)
    {
        if (!entity->hasComponent<CLifespan>()) continue;
        CLifespan& cLifespan = entity->getComponent<CLifespan>();
        cLifespan.remainingSeconds_ -= dt;
        if (cLifespan.remainingSeconds_ <= 0) {
            cLifespan.remainingSeconds_ = 0;
        }

        if (entity->hasComponent<CShape>()) {
            CShape& cShape = entity->getComponent<CShape>();
            uint8_t alpha = (cLifespan.remainingSeconds_ / cLifespan.lifespanSeconds_) * 255;
            cShape.fillColor_.a = alpha;
            cShape.outlineColor_.a = alpha;
            cShape.getShape()->setFillColor(cShape.fillColor_);
            cShape.getShape()->setOutlineColor(cShape.outlineColor_);
        }

        if (cLifespan.remainingSeconds_ <= 0)
        {
            entity->destroy();
        }
    }
}

void ScenePlay::spawnEnemyDeathParticles(Entity* enemy)
{
    if (enemy->hasComponent<CShape>())
    {
        auto& cShape = enemy->getComponent<CShape>();
        auto& eTransform = enemy->getComponent<CTransform>();

        float angleStep = 360.0f / cShape.point_count_;
        for (int i = 0; i < cShape.point_count_; ++i)
        {
            float angle = i * angleStep;
            angle += angleStep/2;
            float radiandAngles = sf::degrees(angle).asRadians();
            auto velX = std::cosf(radiandAngles) * 300;
            auto velY = -1 * std::sinf(radiandAngles) * 300;

            std::shared_ptr<Entity> enemyParticle = EntityManager::getInstance().addEntity("enemy", gameEngine_->getCurrentSceneName(), "EnemyDeathParticle");
            enemyParticle->addRenderable<CShape>(10, cShape.point_count_, cShape.fillColor_, cShape.outlineColor_, 2);
            enemyParticle->addComponent<CLifespan>(0.4);
            enemyParticle->addComponent<CTransform>(eTransform.getPosition(), Vec2f(velX, velY), eTransform.getRotation(), Vec2f(1,1));
        }
    }
}

void ScenePlay::sDebug()
{
    for (std::shared_ptr<Entity>& ePtr : EntityManager::getInstance().getEntities())
    {
        Entity& e = *ePtr;
        if (e.hasComponent<CTransform>())
        {
            CTransform& transform = e.getComponent<CTransform>();
            Vec2f pos = transform.getPosition();
            if (e.hasComponent<CCircleCollider>())
            {
                CCircleCollider& c = e.getComponent<CCircleCollider>();
                sf::CircleShape sfShape(c.radius_);
                sfShape.setPointCount(20);
                sfShape.setPosition(sf::Vector2f(pos.x, pos.y));
                sfShape.setOrigin({c.radius_, c.radius_});
                sfShape.setFillColor(sf::Color::Transparent);
                sfShape.setOutlineColor(sf::Color::Green);
                sfShape.setOutlineThickness(2);
                gameEngine_->getWindow().draw(sfShape);
            }
            
            if (gameEngine_->getDebugOptions().showCollisionGeometry && e.hasComponent<CBoundingBox>())
            {
                CBoundingBox& b = e.getComponent<CBoundingBox>();
                sf::RectangleShape sfShape;
                sfShape.setOutlineColor(sf::Color::Green);
                sfShape.setOutlineThickness(2);
                sfShape.setFillColor(sf::Color::Transparent);
                sfShape.setPosition(sf::Vector2f(pos.x - b.size.x/2, pos.y - b.size.y/2));
                sfShape.setSize(sf::Vector2f(b.size.x, b.size.y));
                gameEngine_->getWindow().draw(sfShape);
            }
        }
    }
    
    const auto& player_pos = player_->getComponent<CTransform>().getPosition();
    const Vec2f end = player_pos + Vec2f::RIGHT() * 100.0f;
    sf::VertexArray ray(sf::PrimitiveType::Lines, 2);
    ray[0].position = sf::Vector2f(player_pos.x, player_pos.y);
    ray[0].color = sf::Color::Red;
    ray[1].position = sf::Vector2f(end.x, end.y);
    ray[1].color = sf::Color::Red;
    gameEngine_->getWindow().draw(ray);
}

void ScenePlay::sDebugUI()
{
    ImGui::Text("Gameplay scene");
    ImGui::TextDisabled("Toggle runtime systems without restarting the scene.");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Systems", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("Movement system", &gameEngine_->getDebugOptions().systems.movement);
        ImGui::Checkbox("Collision system", &gameEngine_->getDebugOptions().systems.collision);
        ImGui::Checkbox("Lifespan system", &gameEngine_->getDebugOptions().systems.lifespan);
        ImGui::Checkbox("Animation system", &gameEngine_->getDebugOptions().systems.animation);
        ImGui::Checkbox("Enemy spawner", &gameEngine_->getDebugOptions().systems.enemySpawner);
    }
}