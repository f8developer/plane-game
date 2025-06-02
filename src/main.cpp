#include "Game.h"
#include "../engine/Engine.h"

#define GAME_WIDTH 800
#define GAME_HEIGHT 600

#define GAME_TITLE "A Plane Game"

// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
int main() {
    unique_ptr<Game> game = std::make_unique<Game>();

    Engine engine;
    engine.Start(GAME_WIDTH,GAME_HEIGHT, GAME_TITLE, reinterpret_cast<unique_ptr<Game>&&>(game));

    return 0;
}