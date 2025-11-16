#include "InputController.hpp"

#include <algorithm>
#include <array>
#include <memory>
#include <ranges>

#include <Game/Time.hpp>
#include <GameFramework.hpp>
#include <Input/InputProcessor.hpp>
#include <Input/InputQueue.hpp>

namespace GameFramework
{

class InputControllerImpl : public InputController
{
public:
  InputControllerImpl(CheckButtonStateFunc && checkButtonState,
                      CheckAxisStateFunc && checkAxisState);
  virtual ~InputControllerImpl() = default;

  virtual void GenerateInputEvents() override;
  virtual void SetInputBindings(const std::span<InputBinding> & bindings) override;

private:
  CheckButtonStateFunc m_checkButtonStateFunc;
  CheckAxisStateFunc m_checkAxisStateFunc;
  std::vector<details::InputProcessorUPtr> m_processors;
};

InputControllerImpl::InputControllerImpl(CheckButtonStateFunc && checkButtonState,
                                         CheckAxisStateFunc && checkAxisState)
  : m_checkButtonStateFunc(checkButtonState)
  , m_checkAxisStateFunc(checkAxisState)
{
}

void InputControllerImpl::GenerateInputEvents()
{
  double time = GetTimeManager().Now();
  for (auto && processor : m_processors)
  {
    processor->TickAction(time);
    auto event = processor->GetAction();
    if (event.has_value())
      PushInputEvent(*event);
  }
}

void InputControllerImpl::SetInputBindings(const std::span<InputBinding> & bindings)
{
  std::vector<details::InputProcessorUPtr> newProcessors;
  newProcessors.reserve(bindings.size());
  for (auto && srcBinding : bindings)
  {
    if (srcBinding.type == ActionType::Axis)
      newProcessors.emplace_back(details::CreateAxisProcessor(srcBinding, m_checkAxisStateFunc));
    else if (srcBinding.type == ActionType::Event || srcBinding.type == ActionType::Continous)
      newProcessors.emplace_back(
        details::CreateButtonProcessor(srcBinding, m_checkButtonStateFunc));
  }
  m_processors = std::move(newProcessors);
}


GAME_FRAMEWORK_API InputControllerUPtr CreateInputController(
  CheckButtonStateFunc && checkButtonState, CheckAxisStateFunc && checkAxisState)
{
  return std::make_unique<InputControllerImpl>(std::move(checkButtonState),
                                               std::move(checkAxisState));
}

} // namespace GameFramework
