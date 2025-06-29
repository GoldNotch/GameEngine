#pragma once
#include <array>
#include <memory>

namespace application
{
    /// Инструмент для рисования. Предоставляет высокоуровневые операции для рисования 
    struct IDrawTool
    {
        virtual ~IDrawTool() = default;
    };

    struct IGame
    {
        virtual ~IGame() = default;
        /// process one tick of the game
        virtual void Tick(float deltaTime) = 0;
        /// Loop over game object and choose the way to render it
        virtual void Render(IDrawTool& drawTool) = 0;
    };

    namespace details
    {
        /// creates global game instance
        std::unique_ptr<IGame> CreateGame();
        /// Get global game instance
        IGame& GetGameInstance();
        /// initializes game instance
        int InitGameFramework();
        /// terminates game and all resources
        void TerminateGameFramework();
    }
}

#define SET_GAME_CLASS(ClassName) \
namespace application::details {\
std::unique_ptr<IGame> CreateGame() \
{\
    return std::make_unique<ClassName>();\
    }\
}

