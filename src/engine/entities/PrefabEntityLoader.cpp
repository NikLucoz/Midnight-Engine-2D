#include "engine/entities/PrefabEntityLoader.h"

#include "engine/assets/Assets.h"
#include "engine/editor/components_registry/ComponentLoaderRegistry.h"
#include "engine/entities/Entity.h"
#include "engine/entities/EntityManager.h"
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include "engine/utils/parser.h"

using std::ifstream;
using std::string;

std::shared_ptr<Entity> PrefabEntityLoader::LoadEntity(const string &prefabName) {
    if (Assets::getInstance().hasPrefabDefinition(prefabName)) {
        return parseEntity(Assets::getInstance().getPrefabDefinition(prefabName));
    }

    return nullptr;
}

std::shared_ptr<Entity> PrefabEntityLoader::parseEntity(std::ifstream& file) {
    string prefabName = "prefab";
    string name = "entity";
    string tag  = "default";

    struct PendingComponent {
        std::string name;
        std::vector<std::string> args;
    };

    std::vector<PendingComponent> componentsPending;
    std::vector<std::shared_ptr<Entity>> entityChildPending;

    string line;
    while (std::getline(file, line)) {
        if (line.empty() || line._Starts_with("//"))
            continue;

        auto tokens = splitComponentArgs(line);
        if (tokens.empty() || tokens[0][0] == '#')
            continue;

        if (tokens[0] == "END_ENTITY_DEF") break;

        if (tokens[0] == "CHILD") {
            if (tokens[1] == "PREFAB") {
                entityChildPending.push_back(LoadEntity(tokens[2]));
                continue;
            }

            if (tokens[1] == "ENTITY_DEF") {
                entityChildPending.push_back(
                    parseEntity(file)
                );
                continue;
            }
        }

        if(tokens[0] == "PREFAB_NAME") {
            prefabName = tokens[1];
            continue;
        }

        if (tokens[0] == "NAME") {
            name = tokens[1];
            continue;
        }
        if (tokens[0] == "TAG") {
            tag = tokens[1];
            continue;
        }

        if (tokens[0] == "COMPONENT") {
            if (tokens.size() < 2) {
                throw std::runtime_error("COMPONENT line is missing the component name");
            }

            PendingComponent pc;
            pc.name = tokens[1];
            pc.args.assign(tokens.begin() + 2, tokens.end());
            componentsPending.push_back(std::move(pc));
            continue;
        }

        throw std::runtime_error("Unknown keyword in prefab: " + tokens[0]);
    }

    std::shared_ptr<Entity> entity = EntityManager::getInstance().addEntity(name, tag);

    for (const auto& pc : componentsPending) {
        ComponentLoaderRegistry::LoadComponent(*entity, pc.name, pc.args);
    }

    for (const auto& ecp : entityChildPending) {
        entity->addChild(*ecp);
    }

    return entity;
}