//
// Created by ivanr on 2.6.2025 г..
//

#ifndef IGAME_H
#define IGAME_H

class IGame {
public:
    virtual ~IGame() = default;

    virtual void Load() = 0;
    virtual void Unload() = 0;
    virtual void Update(float d_time) = 0;
    virtual void FixedUpdate(float fixed_d_time) = 0;
    virtual void AsyncUpdate(float d_time) = 0;  // This will run in worker threads
    virtual void Draw() = 0;
};

#endif //IGAME_H
