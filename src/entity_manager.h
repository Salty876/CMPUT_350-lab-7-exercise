#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include <cassert>
#include <cstddef>  // size_t
#include <limits>
#include <vector>
#include "registry.h"

#include "pack.h"  // Pack from prep

using EntityID = size_t;

template <typename... ComponentTypes>
class EntityManager {

public:
    EntityManager() : recycledKeys(std::vector<EntityID> {}), validSlots(std::vector<bool>{}), active(static_cast<EntityID>(0)) {};

    EntityID createEntity() {
        if (!recycledKeys.empty()) {
            EntityID ID = recycledKeys.back();
            recycledKeys.pop_back();

            validSlots.at(ID) = true;
            active = active + static_cast<size_t>(1);
            return ID;
        }
        else {
            EntityID ID = active;
            validSlots.at(ID) = true;
            active = active + static_cast<size_t>(1);
            return ID;
        }
    }

    bool validEntity(EntityID ID) {
        return validSlots.at(ID);
    }

    template <typename ComponentType> bool hasComponent(EntityID ID) {
        static constexpr int componentTypeIndex = Pack<ComponentTypes...>::template index<ComponentType>;
        static_assert(componentTypeIndex >= 0,"Component type is invalid");
        static_assert(validEntity(ID), "Invlaid ID");
        return entityComponentIndex[ID][componentTypeIndex] != invalidComponentIndex;
    }

    template <typename ComponentType> ComponentType& getComponent(EntityID ID) {
        static constexpr int componentTypeIndex = Pack<ComponentTypes...>::template index<ComponentType>;
        EntityID componentIndex = entityComponentIndex[ID][componentTypeIndex];
        static_assert(componentTypeIndex >= 0, "Invalid component type");
        Registry<ComponentType>& currRegistry = std::get<componentTypeIndex>(registries);

        return currRegistry[componentIndex];
    }

    template <typename ComponentType> void addComponent(EntityID id, ComponentType&& component) {
        using T = std::remove_reference_t<ComponentType>;
        static constexpr int componentTypeIndex = Pack<ComponentTypes...>::template index<T>;
        EntityID componentIndex = entityComponentIndex[id][componentTypeIndex];
        static_assert(componentTypeIndex >= 0, "Invalid component type");
        Registry<ComponentType>& currRegistry = std::get<componentTypeIndex>(registries);
        currRegistry.store(component);
    }

private:
    static constexpr size_t invalidComponentIndex =  std::numeric_limits<size_t>::max();
    std::vector<std::array<size_t, sizeof...(ComponentTypes)>> entityComponentIndex;
    std::tuple<Registry<ComponentTypes>...> registries;

    std::vector<EntityID>recycledKeys;
    std::vector<bool> validSlots;
    EntityID active;
};

#endif  // ENTITY_MANAGER_H
