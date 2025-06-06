#ifndef SYSTEM_MANAGER_H
#define SYSTEM_MANAGER_H

#include <vector>
#include <functional>
#include <array>
#include <entt/entt.hpp>

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

    // Add a system to a specific update type
    void AddSystem(SystemFunction system, UpdateType updateType) {
        systems[static_cast<size_t>(updateType)].push_back(std::move(system));
    }

    // Remove a system from a specific update type
    void RemoveSystem(const SystemFunction& system, UpdateType updateType) {
        auto& systemList = systems[static_cast<size_t>(updateType)];
        systemList.erase(
            std::remove_if(systemList.begin(), systemList.end(),
                [&system](const SystemFunction& s) {
                    return s.target_type() == system.target_type();
                }),
            systemList.end()
        );
    }

    // Execute all systems of a specific type
    void ExecuteSystems(UpdateType updateType, entt::registry& registry, float deltaTime) {
        const auto& systemList = systems[static_cast<size_t>(updateType)];
        for (const auto& system : systemList) {
            system(registry, deltaTime);
        }
    }

    // Clear all systems of a specific type
    void ClearSystems(UpdateType updateType) {
        systems[static_cast<size_t>(updateType)].clear();
    }

    // Clear all systems
    void ClearAllSystems() {
        for (auto& systemList : systems) {
            systemList.clear();
        }
    }

private:
    std::array<std::vector<SystemFunction>, static_cast<size_t>(UpdateType::Count)> systems;
};

#endif // SYSTEM_MANAGER_H 