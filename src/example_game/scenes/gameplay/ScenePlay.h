#pragma once
#include "engine/entities/Entity.h"
#include "engine/scenes/Scene.h"
#include "engine/GameEngine.h"
#include "engine/assets/tilemap/Tilemap.h"

class ScenePlay : public Scene
{
    float elapsedTimeSinceLastEnemySpawn_ = 0.0f;
    float enemySpawnMaxTime;
    std::shared_ptr<Entity> player_;
    TileMap tilemap_;

public:
    ScenePlay(GameEngine* gameEngine, float enemySpawnTime);
    void init() override;
    void destroy() override;
    void update(float dt) override;
    void sRender(float dt) override;
    void sDebugUI() override;
    void sDoAction(const Action& action) override;
    
    // systems
    void sAnimation(float dt);
    void sMovement(float dt);
    void sEnemySpawner(float dt);
    void sCollision();
    void sLifespan(float dt);
    void sDebug();
    
    void spawnEnemyDeathParticles(Entity* get);
};
