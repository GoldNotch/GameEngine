#include "GameFramework.hpp"
#include <thread>


int GameThreadMain();
static std::unique_ptr<std::thread> s_gameThread;
static std::unique_ptr<application::IGame> s_gameInstance;

namespace application::details
{

    IGame& GetGameInstance()
    {
        return *s_gameInstance;
    }

    int InitGameFramework()
    {
        try {
            s_gameInstance = application::details::CreateGame();
            s_gameThread = std::make_unique<std::thread>(GameThreadMain);
        }
        catch (const std::exception& e)
        {
            return -1;
        }
        catch (...)
        {
            return -2;
        }
        return 0;
    }

    void TerminateGameFramework()
    {
        if (s_gameThread)
            s_gameThread->join();
        s_gameThread.reset();
        s_gameInstance.reset();
    }

}


int GameThreadMain()
{
    auto&& game = application::details::GetGameInstance();
    while (true)
    {
        game.Tick(0.0f);
    }
}


