#ifndef SYSTEM_MANAGER_H
#define SYSTEM_MANAGER_H

#include <vector>
#include <functional>
#include <array>
#include <entt/entt.hpp>
#include <unordered_set>

class SystemManager {
public:
    enum class UpdateType {
        Update,
        FixedUpdate,
        AsyncUpdate,
        Draw,
        Count  // Keep this last for array sizing
    };

    using SystemFunction = std::function<void(entt::registry&, float)>;
    using SystemID = size_t;

    // Add a system to a specific update type
    SystemID AddSystem(SystemFunction system, UpdateType updateType) {
        SystemID id = nextSystemID++;
        systems[static_cast<size_t>(updateType)].push_back(std::move(system));
        systemIDs[static_cast<size_t>(updateType)].push_back(id);
        return id;
    }

    // Remove a system from a specific update type
    void RemoveSystem(SystemID id, UpdateType updateType) {
        size_t typeIndex = static_cast<size_t>(updateType);
        auto& systemList = systems[typeIndex];
        auto& idList = systemIDs[typeIndex];
        for (size_t i = 0; i < idList.size(); ++i) {
            if (idList[i] == id) {
                if (i < systemList.size()) {
                    systemList.erase(systemList.begin() + i);
                }
                idList.erase(idList.begin() + i);
                pausedSystems.erase(id);
                break;
            }
        }
    }

    // Execute all systems of a specific type
    void ExecuteSystems(UpdateType updateType, entt::registry& registry, float deltaTime) {
        const auto& systemList = systems[static_cast<size_t>(updateType)];
        const auto& idList = systemIDs[static_cast<size_t>(updateType)];
        size_t count = std::min(systemList.size(), idList.size());
        for (size_t i = 0; i < count; ++i) {
            if (pausedSystems.find(idList[i]) == pausedSystems.end()) {
                systemList[i](registry, deltaTime);
            }
        }
    }

    // Pause a specific system
    void PauseSystem(SystemID id) {
        pausedSystems.insert(id);
    }

    // Resume a specific system
    void ResumeSystem(SystemID id) {
        pausedSystems.erase(id);
    }

    // Check if a system is paused
    bool IsSystemPaused(SystemID id) const {
        return pausedSystems.find(id) != pausedSystems.end();
    }

    // Clear all systems of a specific type
    void ClearSystems(UpdateType updateType) {
        size_t typeIndex = static_cast<size_t>(updateType);
        systems[typeIndex].clear();
        systemIDs[typeIndex].clear();
    }

    // Clear all systems
    void ClearAllSystems() {
        for (size_t i = 0; i < static_cast<size_t>(UpdateType::Count); ++i) {
            systems[i].clear();
            systemIDs[i].clear();
        }
        pausedSystems.clear();
    }

private:
    std::array<std::vector<SystemFunction>, static_cast<size_t>(UpdateType::Count)> systems;
    std::array<std::vector<SystemID>, static_cast<size_t>(UpdateType::Count)> systemIDs;
    std::unordered_set<SystemID> pausedSystems;
    // SystemID 0 is reserved as invalid; real IDs start from 1
    SystemID nextSystemID = 1;
};

#endif // SYSTEM_MANAGER_H 