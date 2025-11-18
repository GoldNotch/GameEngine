#include <algorithm>
#include <cassert>
#include <chrono>
#include <ranges>
#include <stdexcept>
#include <unordered_map>

#include <GameFramework.hpp>
#include <Input/InputProcessor.hpp>

namespace GameFramework::details
{

/// @brief parses a set of keys or combination keys and generate one action if it occures
struct ButtonProcessor final : public InputProcessor
{
  explicit ButtonProcessor(ActionType actionType, int actionCode, InputDevice device,
                           ButtonsCondition && condition, CheckButtonStateFunc && checkState);

  /// update state of action
  virtual void TickAction(double currentTime) override;
  /// get generated action if it's active
  virtual std::optional<GameInputEvent> GetAction() const noexcept override;
  /// @brief get device to be listened
  /// @return id of device
  virtual InputDevice GetDevice() const noexcept override { return m_device; }

private:
  ActionType m_actionType;
  int m_actionCode;
  InputDevice m_device;
  ButtonsCondition m_condition;

  bool m_isActive = false;
  double m_activeTimestamp = 0.0;
  double m_activeDuration = 0.0;

  CheckButtonStateFunc m_checkState;

private:
  bool IsActionActive() const;
};

ButtonProcessor::ButtonProcessor(ActionType actionType, int actionCode, InputDevice device,
                                 ButtonsCondition && condition, CheckButtonStateFunc && checkState)
  : m_actionType(actionType)
  , m_actionCode(actionCode)
  , m_device(device)
  , m_condition(std::move(condition))
  , m_checkState(checkState)
{
  assert(m_actionType == ActionType::Event || m_actionType == ActionType::Continous);
}

void ButtonProcessor::TickAction(double currentTime)
{
  bool isStillActive = IsActionActive();
  if (m_isActive != isStillActive)
  {
    // if just activated when set time to currentTime
    m_activeTimestamp = !m_isActive && isStillActive ? currentTime : 0.0;
    m_activeDuration = 0.0;
  }
  else
  {
    m_activeDuration = currentTime - m_activeTimestamp;
  }
  m_isActive = isStillActive;
}

std::optional<GameInputEvent> ButtonProcessor::GetAction() const noexcept
{
  if (m_isActive)
  {
    switch (m_actionType)
    {
      case ActionType::Event:
        return m_activeDuration > 0.0 ? std::nullopt
                                      : std::make_optional(EventAction{m_actionCode, m_device});
      case ActionType::Continous:
        return ContinousAction{m_actionCode, m_device, m_activeTimestamp, m_activeDuration};
      default:
        Log(LogMessageType::Warning, "Unknown action type is fired - ", m_actionCode, L"(",
            static_cast<int>(m_actionType), ")");
        return std::nullopt;
    }
  }
  return std::nullopt;
}

bool ButtonProcessor::IsActionActive() const
{
  if (m_condition.empty())
    return false;

  auto checkButtonPressState = [this](const details::ButtonPressState & ps)
  {
    return m_checkState(m_device, ps.first) == ps.second;
  };
  auto checkChord = [&checkButtonPressState](const details::ButtonsChord & chord) -> bool
  {
    return std::ranges::all_of(chord, checkButtonPressState);
  };

  return std::ranges::any_of(m_condition, checkChord);
}

InputProcessorUPtr CreateButtonProcessor(ActionType actionType, int actionCode, InputDevice device,
                                         ButtonsCondition condition,
                                         CheckButtonStateFunc checkButtonState)
{
  if (condition.empty() || actionType != ActionType::Event && actionType != ActionType::Continous)
    return nullptr;
  return std::make_unique<ButtonProcessor>(actionType, actionCode, device, std::move(condition),
                                           std::move(checkButtonState));
}

} // namespace GameFramework::details
