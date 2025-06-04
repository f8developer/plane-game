//
// Created by ivanr on 2.6.2025 г..
//

#ifndef IGAME_H
#define IGAME_H

class Renderer;

class IGame {
public:
    virtual ~IGame() = default;

    virtual void Load() = 0;
    virtual void Unload() = 0;
    virtual void Update(float d_time) = 0;
    virtual void Draw(const Renderer& renderer) = 0;
};



#endif //IGAME_H
