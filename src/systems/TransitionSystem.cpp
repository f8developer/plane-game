#include "TransitionSystem.h"
#include "AssetManager.h"
#include "Log.h"
#include "GameConfig.h"

// Initialize static members
SystemManager* TransitionSystem::systemManager = nullptr;
SystemManager::SystemID TransitionSystem::movementSystemID = 0;
SystemManager::SystemID TransitionSystem::bulletSystemID = 0;
SystemManager::SystemID TransitionSystem::transitionSystemID = 0;
entt::entity TransitionSystem::shipEntity = entt::null;

void TransitionSystem::Initialize(entt::registry& registry) {
    LOG_DEBUG("Initializing Transition System");
    
    // Verify system IDs are valid
    if (movementSystemID == 0 && bulletSystemID == 0 && transitionSystemID == 0) {
        LOG_DEBUG("Warning: System IDs not properly initialized");
        return;
    }
    
    // Verify system manager is set
    if (!systemManager) {
        LOG_DEBUG("Warning: System manager not set");
        return;
    }
    
    // Spawn ship only if everything is properly initialized
    SpawnShip(registry);
    LOG_DEBUG("Transition System initialized with IDs - Movement: %llu, Bullet: %llu, Transition: %llu", 
              movementSystemID, bulletSystemID, transitionSystemID);
}

void TransitionSystem::SpawnShip(entt::registry& registry) {
    LOG_DEBUG("Spawning ship entity");
    
    // Clean up any existing ship entity first
    if (shipEntity != entt::null && registry.valid(shipEntity)) {
        registry.remove<TransformComponent>(shipEntity);
        registry.remove<SpriteComponent>(shipEntity);
        registry.destroy(shipEntity);
        shipEntity = entt::null;
        LOG_DEBUG("Cleaned up existing ship entity");
    }
    
    shipEntity = registry.create();

    auto& transform = registry.emplace<TransformComponent>(shipEntity);
    auto& ship_image = registry.emplace<SpriteComponent>(shipEntity);

    // Load ship texture
    AssetManager::AddSceneTexture("ship", "bomber_one.png", 0);  // Using 0 as scene ID for now
    auto& texture = AssetManager::GetTexture("ship");

    // Define the ship size (make it wider than tall for a boat-like appearance)
    ship_image.size.x = PLAYER_SPRITE_SIZE * 3.0f;  // Make ship wider
    ship_image.size.y = PLAYER_SPRITE_SIZE * 1.5f;  // Make ship shorter

    // Position the ship at the bottom of the screen
    transform.position.x = VIRTUAL_WIDTH / 2.0f;
    transform.position.y = -VIRTUAL_HEIGHT - ship_image.size.y/2.0f;

    // Set up the ship sprite
    ship_image.texture = &texture;
    ship_image.tint = BLUE;  // Make ship blue to distinguish it
    ship_image.origin = Vector2{ ship_image.size.x/2.0f, ship_image.size.y/2.0f };
    LOG_DEBUG("Ship spawned at position: (%.1f, %.1f)", transform.position.x, transform.position.y);
}

void TransitionSystem::Update(entt::registry& registry, float deltaTime) {
    auto view = registry.view<TransformComponent, TransitionComponent, SpriteComponent>();
    
    for (auto entity : view) {
        auto& transform = view.get<TransformComponent>(entity);
        auto& transition = view.get<TransitionComponent>(entity);
        auto& sprite = view.get<SpriteComponent>(entity);

        switch (transition.state) {
            case TransitionState::Landing:
                UpdateLanding(transform, transition, sprite, deltaTime);
                break;
                
            case TransitionState::Landed:
                // Wait for a moment before taking off
                transition.progress += deltaTime * TRANSITION_LANDED_DURATION;
                if (transition.progress >= 1.0f) {
                    LOG_DEBUG("Landing complete, transitioning to takeoff");
                    transition.state = TransitionState::TakingOff;
                    transition.progress = 0.0f;
                    transition.targetRotation = 0.0f; // Reset rotation for takeoff
                }
                break;
                
            case TransitionState::TakingOff:
                UpdateTakeoff(registry, transform, transition, sprite, deltaTime);
                break;
                
            default:
                break;
        }
    }

    // Update boat animation
    UpdateBoatAnimation(registry, deltaTime);
}

void TransitionSystem::SetSystemManager(SystemManager* manager) {
    systemManager = manager;
}

void TransitionSystem::SetSystemIDs(SystemManager::SystemID movementID, SystemManager::SystemID bulletID, SystemManager::SystemID transitionID) {
    LOG_DEBUG("Setting system IDs - Movement: %llu, Bullet: %llu, Transition: %llu", movementID, bulletID, transitionID);
    movementSystemID = movementID;
    bulletSystemID = bulletID;
    transitionSystemID = transitionID;
}

void TransitionSystem::UpdateLanding(TransformComponent& transform, TransitionComponent& transition, 
                                   SpriteComponent& sprite, float deltaTime) {
    // Update progress
    transition.progress += deltaTime * TRANSITION_LANDING_SPEED;
    
    if (transition.progress >= 1.0f) {
        // Landing complete
        LOG_DEBUG("Landing complete at position: (%.1f, %.1f)", transform.position.x, transform.position.y);
        transition.state = TransitionState::Landed;
        transition.progress = 0.0f;
        transform.position = transition.targetPosition;
        transform.rotation = TRANSITION_LANDING_FINAL_ROTATION;
    } else {
        // Interpolate position and rotation with easing
        float t = transition.progress;
        t = t * t * (TRANSITION_EASING_SMOOTHNESS - TRANSITION_EASING_STRENGTH * t); // Smoothstep interpolation
        
        // Add a slight arc to the landing path
        float arcOffset = std::sin(t * PI * TRANSITION_LANDING_ARC_SMOOTHNESS) * TRANSITION_LANDING_ARC_HEIGHT;
        
        // Calculate new position
        float newX = transform.position.x + (transition.targetPosition.x - transform.position.x) * t;
        float newY = transform.position.y + (transition.targetPosition.y - transform.position.y) * t - arcOffset;
        
        // Keep player within screen bounds
        const float halfWidth = sprite.size.x / 2.0f;
        const float halfHeight = sprite.size.y / 2.0f;
        newX = std::max(halfWidth, std::min(newX, VIRTUAL_WIDTH - halfWidth));
        newY = std::max(halfHeight, std::min(newY, VIRTUAL_HEIGHT - halfHeight));
        
        transform.position.x = newX;
        transform.position.y = newY;
        
        // Smooth rotation during landing
        float rotationProgress = t * TRANSITION_LANDING_ROTATION_SPEED;
        transform.rotation = transform.rotation + (TRANSITION_LANDING_FINAL_ROTATION - transform.rotation) * rotationProgress;
        
        if (static_cast<int>(transition.progress * 100) % 10 == 0) {  // Log every 10% progress
            LOG_DEBUG("Landing progress: %.0f%%, Position: (%.1f, %.1f), Rotation: %.1f", 
                     transition.progress * 100, transform.position.x, transform.position.y, transform.rotation);
        }
    }
}

void TransitionSystem::UpdateTakeoff(entt::registry& registry, TransformComponent& transform, TransitionComponent& transition,
                                   SpriteComponent& sprite, float deltaTime) {
    // Update progress
    transition.progress += deltaTime * TRANSITION_TAKEOFF_SPEED;
    
    if (transition.progress >= 1.0f) {
        // Takeoff complete
        LOG_DEBUG("Takeoff complete, enabling movement and bullet systems");
        transition.state = TransitionState::None;
        transition.progress = 0.0f;
        transform.rotation = TRANSITION_TAKEOFF_FINAL_ROTATION;

        // Resume movement and bullet systems
        if (systemManager) {
            LOG_DEBUG("Enabling movement and bullet systems");
            systemManager->ResumeSystem(movementSystemID);  // Movement system
            systemManager->ResumeSystem(bulletSystemID);    // Bullet system
            
            // Remove transition component from player
            registry.remove<TransitionComponent>(registry.view<TransitionComponent>().front());
            
            // Remove ship entity and clean up its resources
            if (shipEntity != entt::null) {
                // Remove components before destroying entity
                if (registry.valid(shipEntity)) {
                    registry.remove<TransformComponent>(shipEntity);
                    registry.remove<SpriteComponent>(shipEntity);
                }
                registry.destroy(shipEntity);
                shipEntity = entt::null;
                LOG_DEBUG("Ship entity cleaned up");
            }
            
            LOG_DEBUG("Transition cleanup complete");
        } else {
            LOG_DEBUG("Warning: systemManager is null during transition completion");
        }
        return;
    }

    // Calculate new position with arc
    float t = transition.progress;
    float smoothT = t * t * (TRANSITION_EASING_SMOOTHNESS - TRANSITION_EASING_STRENGTH * t); // Smoothstep interpolation
    
    // Calculate arc height and offset
    float verticalOffset = TRANSITION_TAKEOFF_ARC_HEIGHT * sinf(smoothT * PI * TRANSITION_TAKEOFF_ARC_SMOOTHNESS); // Smooth arc motion
    
    // Calculate new position
    float newX = transform.position.x;
    float newY = transform.position.y - verticalOffset;
    
    // Keep player within screen bounds
    const float halfWidth = sprite.size.x / 2.0f;
    const float halfHeight = sprite.size.y / 2.0f;
    newX = std::max(halfWidth, std::min(newX, VIRTUAL_WIDTH - halfWidth));
    newY = std::max(halfHeight, std::min(newY, VIRTUAL_HEIGHT - halfHeight));
    
    // Update position
    transform.position.x = newX;
    transform.position.y = newY;
    
    // Update rotation with easing
    transform.rotation = TRANSITION_TAKEOFF_INITIAL_ROTATION + (TRANSITION_TAKEOFF_FINAL_ROTATION - TRANSITION_TAKEOFF_INITIAL_ROTATION) * smoothT;
    
    if (static_cast<int>(transition.progress * 100) % 10 == 0) {  // Log every 10% progress
        LOG_DEBUG("Takeoff progress: %.0f%%, Position: (%.1f, %.1f), Rotation: %.1f", 
                 transition.progress * 100, transform.position.x, transform.position.y, transform.rotation);
    }
}

void TransitionSystem::UpdateBoatAnimation(entt::registry& registry, float deltaTime) {
    if (!registry.valid(shipEntity)) return;
    
    auto& transform = registry.get<TransformComponent>(shipEntity);
    auto& sprite = registry.get<SpriteComponent>(shipEntity);
    
    // Get the player's transition state if it exists
    auto playerView = registry.view<TransitionComponent>();
    if (!playerView.empty()) {
        auto& transition = playerView.get<TransitionComponent>(playerView.front());
        
        // Move ship to landing point during landing phase
        if (transition.state == TransitionState::Landing) {
            float t = transition.progress;
            t = t * t * (TRANSITION_EASING_SMOOTHNESS - TRANSITION_EASING_STRENGTH * t); // Smoothstep interpolation
            
            // Calculate ship's target position (slightly offset from player's landing position)
            Vector2 targetPos = transition.targetPosition;
            targetPos.y += TRANSITION_SHIP_LANDING_OFFSET; // Offset the ship slightly below the landing point
            
            // Interpolate ship position
            transform.position.x = transform.position.x + (targetPos.x - transform.position.x) * t;
            transform.position.y = transform.position.y + (targetPos.y - transform.position.y) * t;
            
            // Add gentle bobbing during movement with phase offset
            transform.position.y += std::sin((transition.progress + TRANSITION_SHIP_BOBBING_PHASE_OFFSET) * PI * 2) 
                                  * TRANSITION_SHIP_BOBBING_AMOUNT * deltaTime;
        }
        // Move ship down and out of screen after takeoff
        else if (transition.state == TransitionState::TakingOff) {
            float t = transition.progress;
            float smoothT = t * t * (TRANSITION_EASING_SMOOTHNESS - TRANSITION_EASING_STRENGTH * t); // Smoothstep interpolation
            
            // Move ship down and out of screen
            transform.position.y += TRANSITION_SHIP_EXIT_SPEED * deltaTime; // Move down faster
            
            // Add gentle bobbing during exit with phase offset
            transform.position.y += std::sin((transition.progress + TRANSITION_SHIP_BOBBING_PHASE_OFFSET) * PI * 2) 
                                  * TRANSITION_SHIP_BOBBING_AMOUNT * deltaTime;
            
            // Remove ship if it's far enough off screen
            if (transform.position.y > VIRTUAL_HEIGHT + sprite.size.y) {
                LOG_DEBUG("Ship moved off screen, removing entity");
                registry.destroy(shipEntity);
                shipEntity = entt::null;
            }
        }
        // Normal bobbing animation when not in transition
        else {
            static float time = 0.0f;
            time += deltaTime;
            transform.position.y += std::sin(time * TRANSITION_SHIP_BOBBING_SPEED) * TRANSITION_SHIP_BOBBING_AMOUNT * deltaTime;
            
            // Add a slight rotation to simulate waves
            transform.rotation = std::sin(time * TRANSITION_SHIP_BOBBING_SPEED * TRANSITION_SHIP_BOBBING_PHASE_OFFSET) 
                               * TRANSITION_SHIP_ROTATION_AMOUNT;
        }
    }
} 