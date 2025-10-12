/// It's functions which should be implemented in dll for Game Framework detected it as Game
#ifndef GAME_FRAMEWORK_BUILD

#ifdef __cplusplus
extern "C"
{
#endif

  /// creates global game instance
  GAME_API void InitGame();
  /// tick is a logical frame of game
  GAME_API void TickGame(float deltaTime);
  /// Get name of the game
  GAME_API void TerminateGame();
  /// Get name of the game
  GAME_API const char * GetGameName();

#ifdef __cplusplus
}
#endif

#endif
