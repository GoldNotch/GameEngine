#pragma once
#include <GameFramework_def.h>

#include <memory>

#include <Input/Input.hpp>
#include <Input/InputDevice.hpp>
#include <Input/InputQueue.hpp>

namespace GameFramework
{

struct InputController;

/// @brief provides low-level input events
struct InputBackend
{
  virtual ~InputBackend() = default;
  virtual void BindController(InputController * controller) = 0;
  virtual PressState CheckButtonState(InputDevice device, InputButton btn) const noexcept = 0;
  virtual std::optional<Vec3f> CheckAxisState(InputDevice device,
                                              InputAxis axis) const noexcept = 0;
};

/// @brief it's the core of input system. Links window's low-level input system and game
struct InputController : public InputProducer
{
  virtual ~InputController() = default;

  /// @brief generates input actions in game
  virtual void GenerateInputEvents() = 0;

  /// @brief applies new input binding
  /// @param bindings - a set of bindings for actions
  virtual void SetInputBindings(const std::span<InputBinding> & bindings) = 0;

  /// @brief event on connection of new input device
  /// @param device - id of device
  /// @param connected - bool flag of connection state
  virtual void OnNewInputDeviceConnected(InputDevice device, bool connected) = 0;
};

using InputControllerUPtr = std::unique_ptr<InputController>;

GAME_FRAMEWORK_API InputControllerUPtr CreateInputController(InputBackend & backend);

/*
* Нажатие характеризуется двумя вещами: кнопка и длительность нажатия. 
    Еще возможно сила нажатия (для определенных кнопок)
* Хотя скорее сила нажатия говорит об Axis-кнопке
* 
Варианты комбинация взаимодействия с клавишами
0) нажать одну клавишу (с модом)
    например Ctrl & S, W, Shift & S
1) нажать все клавиши одновременно 
    - например LeftCtrl + S, W, RightShift + Shift & S, W + S,
2) нажать серию клавиш одну за другой с определенной задержкой по времени
    LeftCtrl > S(1s), LBM > LBM, LeftCtrl > NULL(1s) > S,
можно использовать несколько >, чтобы указать, что задержка между клавишами может быть большой
3) Нажать серию клавиш последовательно, в какой-то момент могут быть одновременные наборы
    LeftCtrl >>> E + Shift & S >>> T+R+E(1s) >>> S


Для 0 и 1 достаточно массива с флагами нажат/не нажат
Для 2 и 3 нужен массив последовательности нажатий + проверка этого массива
*/

} // namespace GameFramework
