#include <iostream>
#include "engine/GameEngine.h"
#include "engine/utils/assets/GameConfigLoader.h"
#include "engine/scenes/menu/MainMenuScene.h"
#include "engine/scenes/Gameplay/ScenePlay.h"

int main() {
    try {
        const GameConfig config = GameConfigLoader::load("game/config/game.cfg");
        GameEngine game_engine(config);
        game_engine.init();
        game_engine.registerScene<MainMenuScene>("main_menu_scene");
        game_engine.registerScene<ScenePlay>("gameplay_scene", 1.0f);
        game_engine.changeScene("main_menu_scene");
        game_engine.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
