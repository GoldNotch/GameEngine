/// It's functions which should be implemented in dll for Game Framework detected it as Game
#pragma once
#include <string>
#include <vector>

#include <DrawTool.hpp>

struct GameAction
{
  std::string name;       ///< name of the action, can be "Jump", "MoveForward", etc
  int code;               ///< code ID of the action, use enum to define it
  bool isPressed = false; ///< true if action is pressed or active
  float xAxisValue = 0.0f;
  float dxAxisValue = 0.0f;
  float yAxisValue = 0.0f;
  float dyAxisValue = 0.0f;
};

struct ProtoGameAction
{
  std::string name;     ///< name of the action, can be "Jump", "MoveForward", etc
  int code;             ///< code ID of the action, use enum to define it
  std::string bindings; ///< string to declare keys that represents this action
};

struct ProtoWindow
{
  std::string title;
  int width;
  int height;
};

/// Get name of the game
GAME_API std::string GetGameName();
/// declares game action in system
GAME_API std::vector<ProtoGameAction> GetInputConfiguration();
/// declares game output in system
GAME_API std::vector<ProtoWindow> GetOutputConfiguration();

/// creates global game instance
GAME_API void InitGame();
/// tick is a logical frame of game
GAME_API void TickGame(float deltaTime);
/// render game
GAME_API void RenderGame(GameFramework::IDrawTool & drawTool);
/// process input events
GAME_API void ProcessInput(const GameAction & action);
/// Get name of the game
GAME_API void TerminateGame();
