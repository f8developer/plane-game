//
// Created by ivanr on 4.6.2025 г..
//

#ifndef SCENEMAINMENU_H
#define SCENEMAINMENU_H

#include "IScene.h"
#include "Renderer.h"
#include "components/DrawingComponent.h"
#include "systems/SystemManager.h"
#include <string>

class SceneMainMenu final : public IScene {
public:
    void Load() override;
    void Unload() override;
    void Update(float d_time) override;
    void Draw() override;

protected:
    void SetupMenuEntities();
    SystemManager systemManager;
    static constexpr i32 SCENE_NAME = ToSceneId(SceneName::SceneMainMenu);
};

#endif //SCENEMAINMENU_H
