#include <cstring>

#include <racingGame/gameManager.h>
#include <racingGame/scenarios/humanSinglePlayerScenario.h>

int main(int argc, char** argv)
{
    GameConfig config;
    config.enableRendering = argc > 1 ? strcmp(argv[1], "--no-render") != 0 : true;
    //config.enableRendering = true;
    config.attachCamera = true;
    config.debugInfo = true;

    HumanSinglePlayerScenario scenario;
    GameManager gameManager(config, &scenario);
    
    return gameManager.Run();
}
