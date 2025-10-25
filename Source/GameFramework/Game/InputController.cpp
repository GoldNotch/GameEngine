#include "InputController.hpp"

#include <algorithm>
#include <ranges>

#include <Game/Time.hpp>
#include <GameFramework.hpp>

namespace GameFramework
{

InputController::InputController()
  : m_pressedButtons{PressState::RELEASED}
{
}

void InputController::GenerateInputEvents()
{
  double time = GetTimeManager().Now();
  for (auto && binding : m_generators)
  {
    binding.TickAction(time);
    auto event = binding.GetAction();
    if (event.has_value())
      PushInputEvent(*event);
  }
}

void InputController::SetInputBindings(const std::span<InputBinding> & bindings)
{
  auto checkState = [this](InputButton btn)
  {
    return m_pressedButtons[static_cast<size_t>(btn)];
  };

  std::vector<ActionGenerator> newBindings;
  newBindings.reserve(bindings.size());
  for (auto && srcBinding : bindings)
  {
    auto && generator = newBindings.emplace_back(srcBinding);
    generator.SetCheckButtonStateFunc(checkState);
  }
  m_generators = std::move(newBindings);
}

void InputController::OnButtonAction(InputButton code, PressState state)
{
  auto && keyInfo = m_pressedButtons[static_cast<size_t>(code)];
  keyInfo = state;
  //if (state & GameFramework::PressState::JUST_PRESSED)
  //{
  //}
  //else if (state & GameFramework::PressState::PRESSING)
  //{
  //}
  //else if (state & GameFramework::PressState::RELEASED)
  //{
  //}
}


} // namespace GameFramework
