#include "InputController.hpp"

#include <algorithm>
#include <array>
#include <ranges>

#include <Game/Time.hpp>
#include <GameFramework.hpp>
#include <Input/InputQueue.hpp>

namespace GameFramework
{

class InputControllerImpl : public InputController
{
public:
  InputControllerImpl();
  virtual ~InputControllerImpl() = default;

  virtual void GenerateInputEvents() override;
  virtual void SetInputBindings(const std::span<InputBinding> & bindings) override;
  virtual void OnButtonAction(InputButton code, PressState state) override;

private:
  std::array<PressState, static_cast<size_t>(InputButton::TOTAL)> m_pressedButtons;
  std::vector<ActionGenerator> m_generators;
};

InputControllerImpl::InputControllerImpl()
  : m_pressedButtons{PressState::RELEASED}
{
}

void InputControllerImpl::GenerateInputEvents()
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

void InputControllerImpl::SetInputBindings(const std::span<InputBinding> & bindings)
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

void InputControllerImpl::OnButtonAction(InputButton code, PressState state)
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


GAME_FRAMEWORK_API InputControllerUPtr CreateInputController()
{
  return std::make_unique<InputControllerImpl>();
  ;
}

} // namespace GameFramework
