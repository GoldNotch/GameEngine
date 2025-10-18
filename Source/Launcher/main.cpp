#include <cstdio>
#include <filesystem>
#include <memory>
#include <stdexcept>

#include <GameFramework.hpp>
#include <GameInstance/GameInstance.hpp>

void RenderThread()
{
  /*while (true)
  {
    if (auto renderTarget = m_fbo.BeginFrame())
    {
      m_instance->RenderFrame(drawTool);
      m_fbo.EndFrame();
    }
  }*/
}

int main(int argc, const char * argv[])
{
  if (argc < 2)
  {
    std::printf("Incorrect launch format");
    return -1;
  }
  std::filesystem::path gamePath = argv[1];
  GameFramework::GameInstance game(gamePath);

  while (true)
  {
    //glfwPollEvents();
    //ProcessInput();
  }

  return 0;
}
