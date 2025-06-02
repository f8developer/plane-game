//
// Created by ivanr on 2.6.2025 г..
//

#ifndef GAME_H
#define GAME_H

#include "IGame.h"


class Game : public IGame {
public:
    void Load();
    void Update(float d_time);
    void Draw(const Renderer& renderer);
};



#endif //GAME_H
