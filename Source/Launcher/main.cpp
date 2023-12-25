#include <cassert>
#include <cstdio>
#include <cstdlib>
#define GLEW_STATIC
#include <GL/glew.h>

#include <Engine.h>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "Process.hpp"

void ConsoleLog(usLogStatus status, int code, const char * message)
{
  switch (status)
  {
    case US_LOG_INFO:
      std::printf("INFO: %s\n", message);
      break;
    case US_LOG_WARNING:
      std::printf("WARNING: %s\n", message);
      break;
    case US_LOG_ERROR:
      std::printf("ERROR(%i): %s\n", code, message);
      break;
    case US_LOG_FATAL_ERROR:
      std::printf("FATAL_ERROR(%i): %s\nProgram is gonna abort!!!", code, message);
      system("cls");
      std::abort();
      break;
  }
}


int main()
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

  GLFWwindow * window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  if (window == NULL)
  {
    std::printf("Failed to create GLFW window\n");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  if (glewInit() != GLEW_OK)
  {
    std::printf("Failed to init glew\n");
    glfwTerminate();
    return -1;
  }

  usConstructOptions opts{};
  usSetLoggingFunc(ConsoleLog);
  if (!usEngineInit(opts))
  {
    std::printf("Failed to init engine\n");
    glfwTerminate();
    return -1;
  }

  App::MainProcess app_process;
  app_process.Start();

  GLuint quad_texture;
  glGenTextures(1, &quad_texture);
  glBindTexture(GL_TEXTURE_2D, quad_texture);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glBindTexture(GL_TEXTURE_2D, 0);

  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    app_process.Pause();
    //collect FrameData
    app_process.Resume();
    usBeginFrame(usVideoOptions{width, height});
    Frame frame = usWaitForResult();

    glBindTexture(GL_TEXTURE_2D, quad_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 frame.video_frame);
    glGenerateMipmap(GL_TEXTURE_2D);
    usEndFrame();

    glEnable(GL_TEXTURE_2D);
    glClearColor(0.4, 0.1, 0.3, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1, -1, 0); // bottom left
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(1, -1, 0); // bottom right
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(1, 1, 0); // top right
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1, 1, 0.0f); // top left
    glEnd();
    GLenum err = glGetError();
    glfwSwapBuffers(window);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
  }
  usEngineTerminate();

  glfwTerminate();
  return 0;
}
