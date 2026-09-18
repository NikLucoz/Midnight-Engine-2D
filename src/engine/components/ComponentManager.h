#pragma once

#include <cstddef>
#include <cstdint>
#include <typeindex>
#include <unordered_map>
#include <vector>

// Uses a sparse-set style layout:
// dense_buffer: tightly packed array of the actual T objects (good cache locality)
// sparse_indices: maps entityId -> index inside dense_buffer
struct TypeComponentStorage {
    std::vector<std::byte> dense_buffer;                 // raw byte buffer that holds the T objects
    std::unordered_map<size_t, uint32_t> sparse_indices; // entityId -> dense index
    size_t element_size;                                 // sizeof(T)
    uint32_t count = 0;

    TypeComponentStorage(size_t size, size_t align) : element_size(size) {
        // Pre-allocate some space so the first few appends don't reallocate
        dense_buffer.reserve(1024 * size);
    }

    // Grow the dense buffer if necessary and return the new dense index
    uint32_t append() {
        count++;
        if (dense_buffer.size() < count * element_size) {
            dense_buffer.resize(count * element_size);
        }
        return count - 1;
    }

    void (*destroyFn)(void *) = nullptr;
};

class ComponentManager {
  private:
    // Maps from typeid to a type-erased storage buffer
    std::unordered_map<std::type_index, TypeComponentStorage> componentsStorage_;
    ComponentManager() = default;

  public:
    static ComponentManager &getInstance() {
        static ComponentManager instance;
        return instance;
    }

    // Register a component type (creates an empty storage for it)
    template <typename T> void registerType() {
        auto it = componentsStorage_.find(typeid(T));
        if (it != componentsStorage_.end())
            return;

        auto &storage = TypeComponentStorage(sizeof(T), alignof(T));
        componentsStorage_.emplace(typeid(T), storage);
        storage.destroyFn = [](void *ptr) { std::destroy_at(static_cast<T *>(ptr)); 
        };
    }

    template <typename T> T &get(size_t entityId) {
        auto &storage = componentsStorage_.at(typeid(T));
        uint32_t index = storage.sparse_indices.at(entityId);

        // Calculate the address of the component inside the dense byte buffer
        // and reinterpret it as a T&
        return *getAddress<T>(storage, index);
    }

    template <typename T> bool has(size_t entityId) {
        // get the specific component storage
        auto it = componentsStorage_.find(typeid(T));
        if (it == componentsStorage_.end())
            return false;

        // check if the entity has a component of that type
        auto &storage = it->second;
        auto sparseIt = storage.sparse_indices.find(entityId);
        if (sparseIt == storage.sparse_indices.end()) return false;
        return true;
    }

    bool has(size_t entityId, std::type_index type) const {
        auto it = componentsStorage_.find(type);
        if (it == componentsStorage_.end())
            return false;

        const auto& storage = it->second;
        return storage.sparse_indices.find(entityId) != storage.sparse_indices.end();
    }

    template <typename T, typename... Args> T &add(size_t entityId, Args &&...args) {
        if (componentsStorage_.find(typeid(T)) == componentsStorage_.end()) {
            registerType<T>();
        }

        auto &storage = componentsStorage_.at(typeid(T));
        uint32_t index = storage.append();
        storage.sparse_indices[entityId] = index;

        // Construct a T object in the already-allocated memory at the calculated address.
        // std::forward preserves the original value category of the arguments.
        T *ptr = getAddress<T>(storage, index);
        return *new (ptr) T(std::forward<Args>(args)...);
    }

    template <typename T> T *getAddress(TypeComponentStorage &storage, uint32_t index) { return reinterpret_cast<T *>(storage.dense_buffer.data() + index * sizeof(T)); }

    template <typename T> void remove(size_t entityId) {
        auto it = componentsStorage_.find(typeid(T));
        if (it == componentsStorage_.end())
            return; // type not registered

        auto &storage = it->second;
        auto sparseIt = storage.sparse_indices.find(entityId);
        if (sparseIt == storage.sparse_indices.end())
            return;

        uint32_t index = storage.sparse_indices[entityId];
        std::destroy_at(getAddress<T>(storage, index));

        storage.sparse_indices.erase(sparseIt);

        // TODO: I should not leave gaps in memory, for now its ok
    }

    void* getPtr(size_t entityId, std::type_index type) {
        auto it = componentsStorage_.find(type);
        if (it == componentsStorage_.end())
            return nullptr;

        auto& storage = it->second;
        auto sparseIt = storage.sparse_indices.find(entityId);
        if (sparseIt == storage.sparse_indices.end())
            return nullptr;

        uint32_t index = sparseIt->second;
        return storage.dense_buffer.data() + index * storage.element_size;
    }

    void removeEntityComponents(size_t entityId) {
        for (auto &[typeIndex, storage] : componentsStorage_) {
            auto sparseIt = storage.sparse_indices.find(entityId);
            if (sparseIt == storage.sparse_indices.end())
                continue; // the entity does not have that type of component

            uint32_t index = sparseIt->second; // { key = entityId,  value = dense index }
            void *ptr = storage.dense_buffer.data() + index * storage.element_size;

            if (storage.destroyFn) {
                storage.destroyFn(ptr);
            }

            storage.sparse_indices.erase(sparseIt);

            // TODO: Same thing of the remove method
        }
    }
};
