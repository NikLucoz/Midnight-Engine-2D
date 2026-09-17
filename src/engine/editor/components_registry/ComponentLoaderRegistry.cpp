#include "ComponentLoaderRegistry.h"
#include "engine/entities/Entity.h"
#include <stdexcept>
#include <string>
#include <vector>

std::unordered_map<std::string, ComponentLoadFn>& ComponentLoaderRegistry::getLoaders()
{
    static std::unordered_map<std::string, ComponentLoadFn> loaders;
    return loaders;
}

void ComponentLoaderRegistry::registerLoader(const char* name, ComponentLoadFn fn)
{
    getLoaders()[name] = std::move(fn);
}

void ComponentLoaderRegistry::LoadComponent(Entity& entity, const std::string& name, const std::vector<std::string>& args)
{
    auto& loaders = getLoaders();
    auto it = loaders.find(name);

    if (it == loaders.end()) {
        throw std::runtime_error("Unknown component: " + name);
    }

    it->second(entity, args);
}