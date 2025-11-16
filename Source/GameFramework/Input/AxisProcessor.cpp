#include <algorithm>
#include <cassert>
#include <chrono>
#include <ranges>
#include <stdexcept>
#include <unordered_map>

#include <GameFramework.hpp>
#include <Input/InputProcessor.hpp>
#include <Utility/StringUtils.hpp>

namespace GameFramework::details
{
using AxesSet = std::vector<InputAxis>;
/// all binding are delimited with ;
static constexpr char s_delimiter = ';';

InputAxis ParseAxisName(std::string_view str) noexcept
{
  using AxisMap = std::unordered_map<std::string_view, InputAxis>;
  static const AxisMap s_axisMap = {
    // Mouse buttons
    {"MouseCursor", InputAxis::MOUSE_CURSOR},
    {"GamepadLeftStick", InputAxis::GAMEPAD_LEFT_STICK},
    {"GamepadRightStick", InputAxis::GAMEPAD_RIGHT_STICK},
    {"GamepadL2", InputAxis::GAMEPAD_LEFT_TRIGGER},
    {"GamepadR2", InputAxis::GAMEPAD_RIGHT_TRIGGER},
  };

  auto it = s_axisMap.find(Utils::Trim(str));
  return it == s_axisMap.end() ? InputAxis::UNKNOWN : it->second;
}

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

struct AxisProcessor final : public IInputProcessor
{
  explicit AxisProcessor(const InputBinding & binding, CheckAxisStateFunc && checkPos);
  /// update state of action
  virtual void TickAction(double currentTime) override;
  /// get generated action if it's active
  virtual std::optional<GameInputEvent> GetAction() const noexcept override;

private:
  InputBinding m_binding;
  std::optional<Vec3f> m_oldAxisValue;
  CheckAxisStateFunc m_checkStateFunc;
  AxesSet m_boundAxes;
};

details::AxesSet ParseBindingString(std::string_view str)
{
  if (str.empty())
    throw std::runtime_error("Binding expression was empty");
  details::AxesSet set;
  auto axesStr = Utils::Split(str, s_delimiter);
  set.reserve(axesStr.size());
  for (auto && axisStr : axesStr)
    set.push_back(ParseAxisName(axisStr));

  return set;
}

AxisProcessor::AxisProcessor(const InputBinding & binding, CheckAxisStateFunc && checkPos)
  : m_binding(binding)
  , m_checkStateFunc(std::move(checkPos))
{
  details::AxesSet builtResult;
  try
  {
    builtResult = ParseBindingString(binding.bindings);
  }
  catch (const std::exception & e)
  {
    Log(LogMessageType::Error, "Failed to built input binding string - ", e.what());
    return;
  }
  m_boundAxes = std::move(builtResult);
}

void AxisProcessor::TickAction(double currentTime)
{
  for (auto axis : m_boundAxes)
  {
    //auto state = m_checkStateFunc(axis);
    //if (state.has_value())
    //{
    //}
  }
}

std::optional<GameInputEvent> AxisProcessor::GetAction() const noexcept
{
  return std::optional<GameInputEvent>();
}

InputProcessorUPtr CreateAxisProcessor(const InputBinding & binding,
                                       CheckAxisStateFunc checkAxisState)
{
  assert(binding.type == ActionType::Axis);
  return std::make_unique<AxisProcessor>(binding, std::move(checkAxisState));
}

} // namespace GameFramework::details
