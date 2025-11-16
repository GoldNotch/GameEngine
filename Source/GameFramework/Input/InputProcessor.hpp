#pragma once
#include <functional>
#include <memory>
#include <optional>

#include <Input/Input.hpp>
#include <Input/InputDevice.hpp>

namespace GameFramework::details
{

struct IInputProcessor
{
  virtual ~IInputProcessor() = default;
  virtual void TickAction(double currentTime) = 0;
  /// get generated action if it's active
  virtual std::optional<GameInputEvent> GetAction() const noexcept = 0;
};

using InputProcessorUPtr = std::unique_ptr<IInputProcessor>;

InputProcessorUPtr CreateButtonProcessor(const InputBinding & binding,
                                         CheckButtonStateFunc checkButtonState);
InputProcessorUPtr CreateAxisProcessor(const InputBinding & binding,
                                       CheckAxisStateFunc checkAxisState);
} // namespace GameFramework::details
