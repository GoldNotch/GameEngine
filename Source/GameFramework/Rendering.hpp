#pragma once
#include <GameFramework_def.h>

#include <glm/glm.hpp>
#include <Resources.hpp>

namespace GameFramework
{

struct DrawParams3D
{
  glm::vec3 cameraPos;
  glm::vec3 cameraView;

  DrawParams3D() = default;
  explicit DrawParams3D(const glm::vec3 & cameraPos, const glm::vec3 & cameraView)
    : cameraPos(cameraPos)
    , cameraView(cameraView)
  {
  }
};

struct DrawParams2D
{
};

/// Инструмент для рисования. Предоставляет высокоуровневые операции для рисования
struct GAME_FRAMEWORK_API IDrawTool
{
  virtual ~IDrawTool() = default;
  // These function must be called before BeginDraw call
  virtual void SetClearColor(const glm::vec4 & color) = 0;
  virtual void SetViewport(int x, int y, int width, int height) = 0;
  virtual void SetClientRect(int width, int height) = 0;

  // these function must be called between BeginDraw/EndDraw
  virtual void DrawMesh(IStaticMeshResource * mesh) = 0;
  virtual void DrawRect(float left, float top, float right, float bottom) = 0;
};

} // namespace GameFramework
