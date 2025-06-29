#include <cstdio>
#include <stdexcept>
#include <memory>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>

#include "GameFramework.hpp"

namespace application
{
    class Window
    {
        SDL_Window* m_window = nullptr;
        SDL_GPUDevice* m_gpu;

    public:
        explicit Window();
        ~Window();

        void RenderFrame();
    };

    Window::Window()
    {
        // create a window
        m_window = SDL_CreateWindow("Hello, Triangle!", 960, 540, SDL_WINDOW_RESIZABLE);
        if (!m_window)
            throw std::runtime_error("Failed to create window");
        m_gpu = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_MSL, true, NULL);
        if (!m_gpu)
            throw std::runtime_error("Failed to connect to GPU");

        SDL_ClaimWindowForGPUDevice(m_gpu, m_window);
    }

    Window::~Window()
    {
        // destroy the GPU device
        SDL_DestroyGPUDevice(m_gpu);
        // destroy the window
        SDL_DestroyWindow(m_window);
    }

    void Window::RenderFrame()
    {
        // acquire the command buffer
        SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(m_gpu);

        // get the swapchain texture
        SDL_GPUTexture* swapchainTexture;
        Uint32 width, height;
        SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, m_window, &swapchainTexture, &width, &height);

        // end the frame early if a swapchain texture is not available
        if (swapchainTexture == NULL)
        {
            // you must always submit the command buffer
            SDL_SubmitGPUCommandBuffer(commandBuffer);
            return;
        }

        // create the color target
        SDL_GPUColorTargetInfo colorTargetInfo{};
        colorTargetInfo.clear_color = { 240 / 255.0f, 240 / 255.0f, 240 / 255.0f, 255 / 255.0f };
        colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
        colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
        colorTargetInfo.texture = swapchainTexture;

        // begin a render pass
        SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, NULL);

        // draw something


        // end the render pass
        SDL_EndGPURenderPass(renderPass);

        // submit the command buffer
        SDL_SubmitGPUCommandBuffer(commandBuffer);
    }
}

static std::unique_ptr<application::Window> g_window;


SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv)
{
    application::details::InitGameFramework();
    g_window = std::make_unique<application::Window>();
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
    g_window->RenderFrame();
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    // close the window on request
    if (event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
    {
        return SDL_APP_SUCCESS;
    }


    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    g_window.reset();
    application::details::TerminateGameFramework();
}


