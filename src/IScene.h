//
// Created by ivanr on 4.6.2025 г..
//

#ifndef ISCENE_H
#define ISCENE_H
#include "Defines.h"
#include <future>

#include "entt/entity/registry.hpp"

class Renderer;

enum class SceneName {
    SceneGame = 0,
    SceneMainMenu,
};

class IScene {
public:
    virtual ~IScene() {}

    virtual void Load() {}
    virtual void Unload() {}
    virtual void Update(float d_time) {}
    virtual void FixedUpdate(float fixed_d_time) {}
    virtual void AsyncUpdate(float d_time) {}
    virtual void Draw() {}

    bool GetLocking() const { return isLocking; }
    bool GetTransparent() const { return isTransparent; }

    static i32 ToSceneId(SceneName name) {return static_cast<i32>(name);}

protected:
    bool isLocking { true };
    bool isTransparent { false };
    entt::registry registry;
};

#endif //ISCENE_H
