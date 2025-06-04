//
// Created by ivanr on 2.6.2025 г..
//

#ifndef GAME_H
#define GAME_H

#include "IGame.h"


class Game : public IGame {
public:
    void Load() override;
    void Unload() override;
    void Update(float d_time) override;
    void Draw(const Renderer& renderer) override;
};



#endif //GAME_H
