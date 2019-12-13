#include <cstring>

#include <racingGame/gameManager.h>
#include <racingGame/scenarios/humanSinglePlayerScenario.h>
#include <racingGame/scenarios/humanMultiplayerScenario.h>

int main(int argc, char** argv)
{
    GameConfig config;
    config.enableRendering = argc > 1 ? strcmp(argv[1], "--no-render") != 0 : true;
    //config.enableRendering = true;
    config.attachCamera = true;
    config.debugInfo = true;

    HumanSinglePlayerScenario scenario;
    //HumanMultiplayerScenario scenario(2);
    GameManager gameManager(config, &scenario);
    
    return gameManager.Run();
}
