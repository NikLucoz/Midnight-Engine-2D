#pragma once

#include "engine/entities/Entity.h"
#include <memory>
class PrefabEntityLoader {
public:
    static std::shared_ptr<Entity> LoadEntity(const std::string &prefabFilePath);
    
private:
    static std::shared_ptr<Entity> parseEntity(std::ifstream& file);
};