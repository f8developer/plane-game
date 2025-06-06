#include "Game.h"
#include "../engine/Engine.h"
#include "GameConfig.h"

int main() {
    unique_ptr<Game> game = std::make_unique<Game>();

    Engine engine;
    Engine::SetProfilingEnabled(false);
    Engine::SetFrameStatsEnabled(false);  // Disable frame stats reporting
    //Engine::SetProfilingEnabled(true, 165*2);
    engine.Start(GAME_WIDTH, GAME_HEIGHT, GAME_TITLE, std::move(game));

    return 0;
}