//
// Created by ivanr on 4.6.2025 г..
//

#ifndef SCENEGAME_H
#define SCENEGAME_H

#include "../IScene.h"
#include "raylib.h"


class SceneGame final : public IScene {
public:
    ~SceneGame() override;

    void Load() override;
    void Unload() override;
    void Update(float d_time) override;
    void FixedUpdate(float fixed_d_time) override;
    void AsyncUpdate(float d_time) override;
    void Draw() override;

private:
    Texture2D* texture = {};
};



#endif //SCENEGAME_H
