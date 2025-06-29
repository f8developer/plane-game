//
// Created by ivanr on 4.6.2025 г..
//

#ifndef SCENEGAME_H
#define SCENEGAME_H

#include "IScene.h"
#include "Log.h"
#include "Renderer.h"
#include "AssetManager.h"
#include "components/DrawingComponent.h"
#include "systems/MovementSystem.h"
#include "systems/RenderSystem.h"
#include "systems/BulletSystem.h"
#include "../systems/game/TransitionSystem.h"
#include "systems/SystemManager.h"
#include <memory>

using SystemID = SystemManager::SystemID;

class SceneGame final : public IScene {
public:
    ~SceneGame() override;
    void Load() override;
    void Unload() override;
    void Update(float d_time) override;
    void FixedUpdate(float fixed_d_time) override;
    void AsyncUpdate(float d_time) override;
    void Draw() override;

protected:
    void SetupCamera();
    void SpawnPlayer();
    void SpawnEnemy();
    void StartTransition();
    void SpawnShip();

    entt::registry registry;
    SystemManager systemManager;
    static constexpr i32 SCENE_NAME = ToSceneId(SceneName::SceneGame);

    SystemID movementSystemID;
    SystemID bulletSystemID;
    SystemID transitionSystemID;

    entt::entity shipEntity;
    bool isTransitioning = false;

    // Add TransitionSystem instance
    std::unique_ptr<TransitionSystem> transitionSystem;
};

#endif //SCENEGAME_H
