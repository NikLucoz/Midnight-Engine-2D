#pragma once
#include <functional>
#include <typeindex>
#include <unordered_map>

class Entity;

using ComponentDrawFn = std::function<void(void *)>;

class ComponentDrawerRegistry {
  public:
    static ComponentDrawerRegistry &getInstance() {
        static ComponentDrawerRegistry reg;
        return reg;
    }

    template <typename T> void registerDrawer(const char *name, ComponentDrawFn fn) { drawers_[std::type_index(typeid(T))] = {name, std::move(fn)}; }

    void drawEntity(Entity &entity);

  private:
    struct Entry {
        const char *name;
        ComponentDrawFn draw;
    };

    std::unordered_map<std::type_index, Entry> drawers_;
};

#define REGISTER_COMPONENT_DRAWER(Type, Name, ...) \
    static bool _reg_##Type = []() { \
        ComponentDrawerRegistry::getInstance().registerDrawer<Type>(Name, \
            [](void* ptr) { \
                auto& c = *static_cast<Type*>(ptr); \
                __VA_ARGS__ \
            }); \
        return true; \
    }();