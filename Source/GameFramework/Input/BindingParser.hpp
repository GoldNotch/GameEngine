#pragma once
#include <string_view>
#include <vector>

#include <Input/InputDevice.hpp>
#include <Input/InputProcessor.hpp>

namespace GameFramework::details
{
struct BindingParser final
{
  BindingParser() = default;
  explicit BindingParser(std::string_view str);
  AxesCondition GetAxesResult(InputDevice device) const;
  ButtonsCondition GetButtonsResult(InputDevice device) const;

private:
  std::unordered_map<InputDevice, std::pair<AxesCondition, ButtonsCondition>> m_parsedBinding;
};
} // namespace GameFramework::details
