#include <GameFramework.hpp>
#include <SDL3/SDL.h>

namespace application
{
    struct DrawTool_SDL : public IDrawTool
    {
        explicit DrawTool_SDL(SDL_Window* wnd);
        virtual ~DrawTool_SDL() override;

    private:
        SDL_GPUDevice* m_gpu = nullptr;
    };
}