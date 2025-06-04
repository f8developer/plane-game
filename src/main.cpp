#include "Game.h"
#include "../engine/Engine.h"

#define GAME_WIDTH 800
#define GAME_HEIGHT 600

#define GAME_TITLE "A Plane Game"

int main() {
    unique_ptr<Game> game = std::make_unique<Game>();

    Engine engine;
    engine.Start(GAME_WIDTH, GAME_HEIGHT, GAME_TITLE, std::move(game));

    return 0;
}