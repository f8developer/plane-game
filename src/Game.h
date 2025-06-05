//
// Created by ivanr on 2.6.2025 г..
//

#ifndef GAME_H
#define GAME_H

#include <memory>
#include "IGame.h"
#include <vector>
#include "scenes/IScene.h"

class Game : public IGame {
public:
    void Load() override;
    void Unload() override;
    void Update(float d_time) override;
    void FixedUpdate(float fixed_d_time) override;
    void AsyncUpdate(float d_time) override;  // This will run in worker threads
    void Draw() override;

    static void AddScene(std::unique_ptr<IScene> newScene);
    static void RemoveTopScene();
    static void SwitchScene(std::unique_ptr<IScene> newScene);

private:
    static std::vector<std::unique_ptr<IScene>> scenes;
    static Game* instance;
};

#endif //GAME_H
