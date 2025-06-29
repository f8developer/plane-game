#include "TransitionSystem.h"
#include "GameConfig.h"
#include "AssetManager.h"
#include "Log.h"
#include <algorithm>
#include <cmath>

TransitionSystem::TransitionSystem(SystemManager* systemManager)
    : m_systemManager(systemManager) {
    if (!m_systemManager) {
        throw std::invalid_argument("SystemManager cannot be null");
    }
    m_stateHandlers[TransitionState::Landing] =
        [this](entt::entity entity, entt::registry& registry, float deltaTime) {
            UpdateLandingState(entity, registry, deltaTime);
        };
    m_stateHandlers[TransitionState::Landed] =
        [this](entt::entity entity, entt::registry& registry, float deltaTime) {
            UpdateLandedState(entity, registry, deltaTime);
        };
    m_stateHandlers[TransitionState::TakingOff] =
        [this](entt::entity entity, entt::registry& registry, float deltaTime) {
            UpdateTakeoffState(entity, registry, deltaTime);
        };
}

void TransitionSystem::Initialize(entt::registry& registry) {
    LOG_DEBUG("Initializing Transition System");
    CreateShipEntity(registry);
    CacheViews(registry);
    LOG_DEBUG("Transition System initialized successfully");
}

void TransitionSystem::Update(entt::registry& registry, float deltaTime) {
    ProcessTransitions(registry, deltaTime);
    UpdateShipAnimation(registry, deltaTime);
}

void TransitionSystem::ProcessTransitions(entt::registry& registry, float deltaTime) {
    if (!m_viewsCached) {
        CacheViews(registry);
    }
    auto view = registry.view<TransformComponent, TransitionComponent, SpriteComponent>();
    for (auto entity : view) {
        auto& transition = registry.get<TransitionComponent>(entity);
        if (transition.state == TransitionState::Landing && !m_shipSpawned) {
            SpawnShipForLanding(registry, transition.targetPosition);
            m_shipSpawned = true;
        }
        auto handlerIt = m_stateHandlers.find(transition.state);
        if (handlerIt != m_stateHandlers.end()) {
            handlerIt->second(entity, registry, deltaTime);
        }
    }
    UpdateShipMovement(registry, deltaTime);
}

void TransitionSystem::UpdateLandingState(entt::entity entity, entt::registry& registry, float deltaTime) {
    if (!registry.valid(entity) ||
        !registry.all_of<TransformComponent, TransitionComponent, SpriteComponent>(entity))
        return;

    auto& transform = registry.get<TransformComponent>(entity);
    auto& transition = registry.get<TransitionComponent>(entity);
    auto& sprite = registry.get<SpriteComponent>(entity);

    // Use a realistic landing duration (e.g., 2.0 seconds)
    constexpr float landingDuration = 2.0f;
    transition.progress += deltaTime / landingDuration;
    if (transition.progress >= 1.0f) {
        LOG_DEBUG("Landing complete at position: (%.1f, %.1f)", transform.position.x, transform.position.y);
        transition.state = TransitionState::Landed;
        transition.progress = 0.0f;
        transform.position = transition.targetPosition;
        transform.rotation = TRANSITION_LANDING_FINAL_ROTATION;
        PositionShipAtLanding(registry, transition.targetPosition);
        return;
    }
    // Use ease-out for progress
    float easedProgress = EaseOutCubic(transition.progress);
    Vector2 startPos = {transform.position.x, transform.position.y - 100.0f};
    transform.position = CalculateInterpolatedPosition(
        startPos, transition.targetPosition, easedProgress, TRANSITION_LANDING_ARC_HEIGHT);
    ClampToScreenBounds(transform.position, sprite.size);
    float rotationProgress = easedProgress * TRANSITION_LANDING_ROTATION_SPEED;
    float startRotation = 45.0f;
    transform.rotation = std::lerp(startRotation, TRANSITION_LANDING_FINAL_ROTATION, rotationProgress);
}

void TransitionSystem::UpdateLandedState(entt::entity entity, entt::registry& registry, float deltaTime) {
    if (!registry.valid(entity) || !registry.all_of<TransitionComponent>(entity)) return;
    auto& transition = registry.get<TransitionComponent>(entity);
    transition.progress += deltaTime / TRANSITION_LANDED_DURATION;
    if (transition.progress >= 1.0f) {
        LOG_DEBUG("Landing complete, transitioning to takeoff");
        transition.state = TransitionState::TakingOff;
        transition.progress = 0.0f;
        transition.targetRotation = TRANSITION_TAKEOFF_FINAL_ROTATION;
    }
}

void TransitionSystem::UpdateTakeoffState(entt::entity entity, entt::registry& registry, float deltaTime) {
    if (!registry.valid(entity) ||
        !registry.all_of<TransformComponent, TransitionComponent, SpriteComponent>(entity))
        return;

    auto& transform = registry.get<TransformComponent>(entity);
    auto& transition = registry.get<TransitionComponent>(entity);
    auto& sprite = registry.get<SpriteComponent>(entity);

    transition.progress += deltaTime * TRANSITION_TAKEOFF_SPEED;
    if (transition.progress >= 1.0f) {
        LOG_DEBUG("Takeoff complete, enabling systems");
        StartShipDeparture(registry);
        CompleteTransition(entity, registry);
        return;
    }
    float easedProgress = CalculateEasedProgress(transition.progress);
    float verticalOffset = TRANSITION_TAKEOFF_ARC_HEIGHT * std::sin(easedProgress * PI);
    Vector2 startPos = transition.targetPosition;
    transform.position.y = startPos.y - verticalOffset;
    ClampToScreenBounds(transform.position, sprite.size);
    float startRotation = TRANSITION_TAKEOFF_INITIAL_ROTATION;
    transform.rotation = std::lerp(startRotation, TRANSITION_TAKEOFF_FINAL_ROTATION, easedProgress);
}

void TransitionSystem::CompleteTransition(entt::entity entity, entt::registry& registry) {
    if (m_movementSystemID != 0) m_systemManager->ResumeSystem(m_movementSystemID);
    if (m_bulletSystemID != 0) m_systemManager->ResumeSystem(m_bulletSystemID);
    if (registry.valid(entity) && registry.all_of<TransitionComponent>(entity))
        registry.remove<TransitionComponent>(entity);

    // Remove the ship entity immediately when transition is done
    DestroyShipEntity(registry);
    m_shipSpawned = false;
    // Reset system IDs to avoid using stale ones
    m_movementSystemID = 0;
    m_bulletSystemID = 0;
    m_transitionSystemID = 0;
    LOG_DEBUG("Transition completed successfully");
}

void TransitionSystem::UpdateShipAnimation(entt::registry& registry, float deltaTime) {
    if (m_shipEntity == entt::null || !registry.valid(m_shipEntity) ||
        !registry.all_of<TransformComponent>(m_shipEntity))
        return;
    auto& transform = registry.get<TransformComponent>(m_shipEntity);
    static float bobbingTime = 0.0f;
    bobbingTime += deltaTime * TRANSITION_SHIP_BOBBING_SPEED;
    float bobbingOffset = std::sin(bobbingTime + TRANSITION_SHIP_BOBBING_PHASE_OFFSET) *
                          TRANSITION_SHIP_BOBBING_AMOUNT;
    transform.position.y += bobbingOffset * deltaTime;
    transform.rotation = std::sin(bobbingTime * 0.5f) * TRANSITION_SHIP_ROTATION_AMOUNT;
}

void TransitionSystem::UpdateShipMovement(entt::registry& registry, float deltaTime) {
    if (m_shipEntity == entt::null || !registry.valid(m_shipEntity) ||
        !registry.all_of<TransformComponent, SpriteComponent>(m_shipEntity))
        return;
    auto& transform = registry.get<TransformComponent>(m_shipEntity);
    auto& sprite = registry.get<SpriteComponent>(m_shipEntity);
    m_shipAnimationTime += deltaTime;
    auto transitionView = registry.view<TransitionComponent>();
    if (transitionView.empty()) return;
    auto& transition = registry.get<TransitionComponent>(*transitionView.begin());
    
    if (transition.state == TransitionState::Landing) {
        if (m_shipMovingToLanding) {
            float duration = 1.0f;
            m_shipMoveProgress += deltaTime / duration;
            if (m_shipMoveProgress >= 1.0f) {
                m_shipMoveProgress = 1.0f;
                m_shipMovingToLanding = false;
            }
            float eased = EaseOutCubic(m_shipMoveProgress);
            Vector2 target = {transition.targetPosition.x, transition.targetPosition.y + TRANSITION_SHIP_LANDING_OFFSET};
            transform.position.x = std::lerp(m_shipStartPosition.x, target.x, eased);
            transform.position.y = std::lerp(m_shipStartPosition.y, target.y, eased);
        }
    } else if (transition.state == TransitionState::Landed) {
        float bobbingOffset = std::sin(m_shipAnimationTime * TRANSITION_SHIP_BOBBING_SPEED) *
                              TRANSITION_SHIP_BOBBING_AMOUNT;
        Vector2 basePosition = transition.targetPosition;
        basePosition.y += TRANSITION_SHIP_LANDING_OFFSET;
        transform.position.y = basePosition.y + bobbingOffset;
        transform.rotation = std::sin(m_shipAnimationTime * 0.5f) * TRANSITION_SHIP_ROTATION_AMOUNT;
    } else if (transition.state == TransitionState::TakingOff) {
        transform.position.y += TRANSITION_SHIP_EXIT_SPEED * deltaTime;
        // Ship will be destroyed when it goes off-screen or in CompleteTransition
    }
}


void TransitionSystem::SpawnShipForLanding(entt::registry& registry, const Vector2& landingPosition) {
    DestroyShipEntity(registry);
    m_shipEntity = registry.create();
    auto& transform = registry.emplace<TransformComponent>(m_shipEntity);
    auto& sprite = registry.emplace<SpriteComponent>(m_shipEntity);
    try {
        AssetManager::AddSceneTexture("ship", "bomber_one.png", 0);
        auto& texture = AssetManager::GetTexture("ship");
        sprite.texture = &texture;
        sprite.size = {PLAYER_SPRITE_SIZE * 3.0f, PLAYER_SPRITE_SIZE * 1.5f};
        sprite.tint = BLUE;
        sprite.origin = {sprite.size.x / 2.0f, sprite.size.y / 2.0f};
        // Ship starts from above the screen
        transform.position = {
            landingPosition.x,  // Align horizontally with landing position
            -sprite.size.y      // Start above the screen
        };
        transform.rotation = 0.0f;
        m_shipAnimationTime = 0.0f;
        // Initialize movement interpolation
        m_shipStartPosition = transform.position;
        m_shipMoveProgress = 0.0f;
        m_shipMovingToLanding = true;
        LOG_DEBUG("Ship spawned for landing at position: (%.1f, %.1f)",
                  transform.position.x, transform.position.y);
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to spawn ship: %s", e.what());
        DestroyShipEntity(registry);
        throw;
    }
}

void TransitionSystem::PositionShipAtLanding(entt::registry& registry, const Vector2& landingPosition) {
    if (m_shipEntity == entt::null || !registry.valid(m_shipEntity) ||
        !registry.all_of<TransformComponent>(m_shipEntity))
        return;
    auto& transform = registry.get<TransformComponent>(m_shipEntity);
    transform.position.x = landingPosition.x;
    transform.position.y = landingPosition.y + TRANSITION_SHIP_LANDING_OFFSET;
    LOG_DEBUG("Ship positioned at landing location: (%.1f, %.1f)",
              transform.position.x, transform.position.y);
}

void TransitionSystem::StartShipDeparture(entt::registry& registry) {
    if (m_shipEntity == entt::null || !registry.valid(m_shipEntity)) return;
    LOG_DEBUG("Ship starting departure");
    // Ship is destroyed in CompleteTransition now
}

void TransitionSystem::CreateShipEntity(entt::registry& registry) {
    DestroyShipEntity(registry);
    m_shipEntity = registry.create();
    auto& transform = registry.emplace<TransformComponent>(m_shipEntity);
    auto& sprite = registry.emplace<SpriteComponent>(m_shipEntity);
    try {
        AssetManager::AddSceneTexture("ship", "bomber_one.png", 0);
        auto& texture = AssetManager::GetTexture("ship");
        sprite.texture = &texture;
        sprite.size = {PLAYER_SPRITE_SIZE * 3.0f, PLAYER_SPRITE_SIZE * 1.5f};
        sprite.tint = BLUE;
        sprite.origin = {sprite.size.x / 2.0f, sprite.size.y / 2.0f};
        transform.position = {
            VIRTUAL_WIDTH / 2.0f,
            -VIRTUAL_HEIGHT - sprite.size.y / 2.0f - TRANSITION_SHIP_LANDING_OFFSET
        };
        LOG_DEBUG("Ship entity created successfully");
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to create ship entity: %s", e.what());
        DestroyShipEntity(registry);
        throw;
    }
}

void TransitionSystem::DestroyShipEntity(entt::registry& registry) {
    if (m_shipEntity != entt::null && registry.valid(m_shipEntity)) {
        registry.destroy(m_shipEntity);
        m_shipEntity = entt::null;
        LOG_DEBUG("Ship entity destroyed");
    }
}

void TransitionSystem::Shutdown(entt::registry& registry) {
    DestroyShipEntity(registry);
    m_stateHandlers.clear();
    m_viewsCached = false;
    m_shipSpawned = false;
    m_shipAnimationTime = 0.0f;
    // Reset movement interpolation
    m_shipMoveProgress = 0.0f;
    m_shipMovingToLanding = false;
    // Reset system IDs to avoid using stale ones
    m_movementSystemID = 0;
    m_bulletSystemID = 0;
    m_transitionSystemID = 0;
}

Vector2 TransitionSystem::CalculateInterpolatedPosition(const Vector2& start,
                                                       const Vector2& target,
                                                       float progress,
                                                       float arcHeight) const {
    Vector2 result;
    result.x = std::lerp(start.x, target.x, progress);
    result.y = std::lerp(start.y, target.y, progress);
    if (arcHeight > 0.0f) {
        float arcOffset = std::sin(progress * PI) * arcHeight;
        result.y -= arcOffset;
    }
    return result;
}

float TransitionSystem::CalculateEasedProgress(float rawProgress) const {
    return rawProgress * rawProgress *
           (TRANSITION_EASING_SMOOTHNESS - TRANSITION_EASING_STRENGTH * rawProgress);
}

void TransitionSystem::ClampToScreenBounds(Vector2& position, const Vector2& spriteSize) const {
    const float halfWidth = spriteSize.x / 2.0f;
    const float halfHeight = spriteSize.y / 2.0f;
    position.x = std::clamp(position.x, halfWidth, VIRTUAL_WIDTH - halfWidth);
    position.y = std::clamp(position.y, halfHeight, VIRTUAL_HEIGHT - halfHeight);
}

void TransitionSystem::CacheViews(entt::registry& registry) {
    m_viewsCached = true;
}

void TransitionSystem::SetSystemIDs(SystemManager::SystemID movementID,
                                    SystemManager::SystemID bulletID,
                                    SystemManager::SystemID transitionID) {
    m_movementSystemID = movementID;
    m_bulletSystemID = bulletID;
    m_transitionSystemID = transitionID;
    LOG_DEBUG("System IDs set - Movement: %llu, Bullet: %llu, Transition: %llu",
              movementID, bulletID, transitionID);
}

float TransitionSystem::EaseOutCubic(float t) const {
    return 1.0f - std::pow(1.0f - t, 3);
}
