#pragma once
#include <GameFramework_def.h>

#include <array>

#include <Game/InputBinding.hpp>
#include <Game/InputDevice.hpp>
#include <Game/InputQueue.hpp>

namespace GameFramework
{


class GAME_FRAMEWORK_API InputController : public InputProducer
{
public:
  InputController();
  ~InputController() = default;

public:
  virtual void GenerateInputEvents() override;
  virtual void SetInputBindings(const std::span<InputBinding> & bindings) override;

public:
  void OnButtonAction(InputButton code, PressState state);

private:
  std::array<PressState, static_cast<size_t>(InputButton::TOTAL)> m_pressedButtons;
  std::vector<ActionGenerator> m_generators;
};

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
