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

struct AxisProcessor final : public InputProcessor
{
  explicit AxisProcessor(ActionType actionType, int actionCode, InputDevice device,
                         AxesCondition && condition, CheckAxisStateFunc && checkPos);
  /// update state of action
  virtual void TickAction(double currentTime) override;
  /// get generated action if it's active
  virtual std::optional<GameInputEvent> GetAction() const noexcept override;
  /// @brief get device to be listened
  /// @return id of device
  virtual InputDevice GetDevice() const noexcept override { return m_device; }

private:
  ActionType m_actionType = ActionType::Axis;
  int m_actionCode;
  InputDevice m_device;
  AxesCondition m_condition;
  AxesValue m_oldAxisValue{AxisNoValue, AxisNoValue, AxisNoValue};
  AxesValue m_curAxisValue{AxisNoValue, AxisNoValue, AxisNoValue};
  CheckAxisStateFunc m_checkStateFunc;
};

AxisProcessor::AxisProcessor(ActionType actionType, int actionCode, InputDevice device,
                             AxesCondition && condition, CheckAxisStateFunc && checkPos)
  : m_actionType(actionType)
  , m_actionCode(actionCode)
  , m_device(device)
  , m_condition(std::move(condition))
  , m_checkStateFunc(std::move(checkPos))
{
  assert(m_actionType == ActionType::Axis);
}

void AxisProcessor::TickAction(double currentTime)
{
  for (auto superposition : m_condition)
  {
    constexpr size_t s = superposition.size();
    for (size_t i = 0; i < s; ++i)
    {
      if (superposition[i] == InputAxis::UNKNOWN)
        break;
      float value = AxisNoValue;
      value = m_checkStateFunc(m_device, superposition[i]);
      m_oldAxisValue[i] = std::exchange(m_curAxisValue[i], value);
    }
  }
}

std::optional<GameInputEvent> AxisProcessor::GetAction() const noexcept
{
  auto arrayHasValues = [](auto && arr)
  {
    return !std::ranges::all_of(arr, IsAxisValueValid);
  };

  if (!arrayHasValues(m_curAxisValue))
  {
    return std::nullopt;
  }

  AxesValue value = m_curAxisValue;
  AxesValue delta = {0.0f};
  if (arrayHasValues(m_oldAxisValue))
  {
    for (size_t i = 0; i < AxesSuperpositionLimit; ++i)
      delta[i] = m_curAxisValue[i] - m_oldAxisValue[i];
  }
  return AxisAction{m_actionCode, m_device, value, delta};
}

InputProcessorUPtr CreateAxisProcessor(ActionType actionType, int actionCode, InputDevice device,
                                       AxesCondition condition, CheckAxisStateFunc checkAxisState)
{
  if (condition.empty() || actionType != ActionType::Axis)
    return nullptr;
  return std::make_unique<AxisProcessor>(actionType, actionCode, device, std::move(condition),
                                         std::move(checkAxisState));
}

} // namespace GameFramework::details
