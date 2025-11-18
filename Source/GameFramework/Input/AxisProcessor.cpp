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

int GetDimensionOfAxis(InputAxis axis) noexcept
{
  switch (axis)
  {
    case InputAxis::GAMEPAD_LEFT_TRIGGER:
    case InputAxis::GAMEPAD_RIGHT_TRIGGER:
      return 1;
    case InputAxis::MOUSE_CURSOR:
    case InputAxis::GAMEPAD_LEFT_STICK:
    case InputAxis::GAMEPAD_RIGHT_STICK:
      return 2;
    case InputAxis::UNKNOWN:
    default:
      return 0;
  }
}

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
  std::optional<Vec3f> m_oldAxisValue;
  std::optional<Vec3f> m_curAxisValue;
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
  for (auto axis : m_condition)
  {
    auto state = m_checkStateFunc(m_device, axis);
    m_oldAxisValue = std::exchange(m_curAxisValue, state);
  }
}

std::optional<GameInputEvent> AxisProcessor::GetAction() const noexcept
{
  if (!m_curAxisValue.has_value())
  {
    return std::nullopt;
  }

  Vec3f delta = {0.0f, 0.0f, 0.0f};
  if (m_oldAxisValue.has_value())
  {
    delta = {m_curAxisValue->x - m_oldAxisValue->x, m_curAxisValue->y - m_oldAxisValue->y,
             m_curAxisValue->z - m_oldAxisValue->z};
  }
  return AxisAction{m_actionCode, m_device, *m_curAxisValue, delta};
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
