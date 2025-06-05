//
// Created by ivanr on 4.6.2025 г..
//

#ifndef SCENEGAME_H
#define SCENEGAME_H

#include "IScene.h"
#include "Log.h"
#include "Renderer.h"
#include "../../engine/AssetManager.h"
#include "components/DrawingComponent.h"
#include "systems/MovementSystem.h"
#include "systems/RenderSystem.h"
#include "systems/SystemManager.h"

#define SCENE_NAME ToSceneId(SceneName::SceneGame)

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
    void SpawnPlayer();
    entt::registry registry;
    SystemManager systemManager;
};



#endif //SCENEGAME_H
