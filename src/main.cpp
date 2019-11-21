#include <cstring>

#include "racingGame/gameManager.h"

int main(int argc, char** argv)
{
    GameConfig config;
    config.enableRendering = argc > 1 ? strcmp(argv[1], "--no-render") != 0 : true;
    config.attachCamera = true;
    config.debugInfo = true;

    GameManager gameManager(config, nullptr);
    
    return gameManager.Run();
}
