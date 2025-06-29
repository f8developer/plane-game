#ifndef TRANSITIONSYSTEM_H
#define TRANSITIONSYSTEM_H

#include <entt/entt.hpp>
#include <memory>
#include <unordered_map>
#include <functional>
#include "components/BasicComponent.h"
#include "components/PlayerComponent.h"
#include "components/DrawingComponent.h"
#include "raylib.h"
#include "systems/SystemManager.h"

class TransitionSystem {
public:
    explicit TransitionSystem(SystemManager* systemManager);
    ~TransitionSystem() = default;

    // Non-copyable, non-movable
    TransitionSystem(const TransitionSystem&) = delete;
    TransitionSystem& operator=(const TransitionSystem&) = delete;
    TransitionSystem(TransitionSystem&&) = delete;
    TransitionSystem& operator=(TransitionSystem&&) = delete;

    void Initialize(entt::registry& registry);
    void Update(entt::registry& registry, float deltaTime);
    void Shutdown(entt::registry& registry);

    void SetSystemIDs(SystemManager::SystemID movementID,
                     SystemManager::SystemID bulletID,
                     SystemManager::SystemID transitionID);

private:
    // Core functionality
    void ProcessTransitions(entt::registry& registry, float deltaTime);
    void UpdateShipAnimation(entt::registry& registry, float deltaTime);

    // Entity management
    void CreateShipEntity(entt::registry& registry);
    void DestroyShipEntity(entt::registry& registry);

    // State-specific updates - using entt::registry& consistently
    void UpdateLandingState(entt::entity entity, entt::registry& registry, float deltaTime);
    void UpdateLandedState(entt::entity entity, entt::registry& registry, float deltaTime);
    void UpdateTakeoffState(entt::entity entity, entt::registry& registry, float deltaTime);
    void CompleteTransition(entt::entity entity, entt::registry& registry);

    // Ship management methods
    void SpawnShipForLanding(entt::registry& registry, const Vector2& landingPosition);
    void UpdateShipMovement(entt::registry& registry, float deltaTime);
    void PositionShipAtLanding(entt::registry& registry, const Vector2& landingPosition);
    void StartShipDeparture(entt::registry& registry);

    // Utility functions
    Vector2 CalculateInterpolatedPosition(const Vector2& start, const Vector2& target,
                                        float progress, float arcHeight = 0.0f) const;
    float CalculateEasedProgress(float rawProgress) const;
    void ClampToScreenBounds(Vector2& position, const Vector2& spriteSize) const;
    void CacheViews(entt::registry& registry);

    // Member variables
    SystemManager* m_systemManager;
    SystemManager::SystemID m_movementSystemID{0};
    SystemManager::SystemID m_bulletSystemID{0};
    SystemManager::SystemID m_transitionSystemID{0};

    entt::entity m_shipEntity{entt::null};
    bool m_viewsCached{false};

    // Ship state tracking
    bool m_shipSpawned{false};
    float m_shipAnimationTime{0.0f};

    // Ship movement interpolation for landing
    Vector2 m_shipStartPosition{0.0f, 0.0f};
    float m_shipMoveProgress{0.0f};
    bool m_shipMovingToLanding{false};
    // Helper for ease-out cubic
    float EaseOutCubic(float t) const;

    // State machine using function pointers
    using StateHandler = std::function<void(entt::entity, entt::registry&, float)>;
    std::unordered_map<TransitionState, StateHandler> m_stateHandlers;
};

#endif // TRANSITIONSYSTEM_H
