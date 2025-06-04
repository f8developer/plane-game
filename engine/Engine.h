//
// Created by ivanr on 2.6.2025 г..
//

#ifndef ENGINE_H
#define ENGINE_H

#include <string>
#include <memory>

#include "Defines.h"
#include "Renderer.h"

using std::unique_ptr;

class IGame;

class Engine {
public:
    DLLEX void Start(int windowWidth, int windowHeight, const std::string& windowTitleL,
                                        unique_ptr<IGame> gameP);

    std::string windowTitle;
    unique_ptr<IGame> game;
    Renderer renderer {};
};



#endif //ENGINE_H
