#include <GameFramework.hpp>

class Game : public application::IGame
{
public:
    virtual ~Game() = default;
    /// 
    virtual void Tick(float deltaTime) override
    {

    }

    /// Loop over game object and choose the way to render it
    virtual void Render(application::IDrawTool& drawTool) override
    {

    }
};


SET_GAME_CLASS(Game);