#pragma once

#include "engine/entities/Entity.h"
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

using ComponentLoadFn = std::function<void(Entity &, const std::vector<std::string> &)>;

class ComponentLoaderRegistry {
  public:
    static void registerLoader(const char *name, ComponentLoadFn fn);
    static void LoadComponent(Entity &entity, const std::string &name, const std::vector<std::string> &args);

  private:
    static std::unordered_map<std::string, ComponentLoadFn> &getLoaders();
};

#define REGISTER_COMPONENT_LOADER(Name, ...)                                                                                                                                                           \
    namespace {                                                                                                                                                                                        \
    struct Reg_##Name {                                                                                                                                                                                \
        Reg_##Name() {                                                                                                                                                                                 \
            ComponentLoaderRegistry::registerLoader(#Name, [](Entity &e, const std::vector<std::string> &args) { __VA_ARGS__ });                                                                       \
        }                                                                                                                                                                                              \
    };                                                                                                                                                                                                 \
    static Reg_##Name reg_##Name;                                                                                                                                                                      \
    }