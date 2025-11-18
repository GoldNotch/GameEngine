#include "InputController.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <memory>
#include <ranges>
#include <unordered_set>

#include <Game/Time.hpp>
#include <GameFramework.hpp>
#include <Input/BindingParser.hpp>
#include <Input/InputProcessor.hpp>
#include <Input/InputQueue.hpp>
#include <Utility/Utility.hpp>

namespace
{
struct InputBindingHasher final
{
  size_t operator()(const GameFramework::InputBinding & binding) const noexcept
  {
    size_t res = 0;
    GameFramework::Utils::hash_combine(res, binding.name, binding.code, binding.bindings);
    return res;
  }
};

struct InputBindingComparator final
{
  bool operator()(const GameFramework::InputBinding & b1,
                  const GameFramework::InputBinding & b2) const noexcept
  {
    return b1.name == b2.name && b1.code == b2.code && b1.bindings == b2.bindings;
  }
};
} // namespace

namespace GameFramework
{

class InputControllerImpl : public InputController
{
public:
  InputControllerImpl(InputBackend & backend);
  virtual ~InputControllerImpl();

  virtual void GenerateInputEvents() override;
  virtual void SetInputBindings(const std::span<InputBinding> & bindings) override;
  virtual void OnNewInputDeviceConnected(InputDevice device, bool connected) override;

private:
  InputBackend * m_backend = nullptr;
  std::unordered_set<InputDevice> m_connectedDevices;
  using ParsedBindingsMap = std::unordered_map<InputBinding, details::BindingParser,
                                               InputBindingHasher, InputBindingComparator>;
  ParsedBindingsMap m_bindings;
  std::vector<details::InputProcessorUPtr> m_processors;
};

InputControllerImpl::InputControllerImpl(InputBackend & backend)
  : m_backend(&backend)
{
  m_backend->BindController(this);
}

InputControllerImpl::~InputControllerImpl()
{
  m_backend->BindController(nullptr);
}

void InputControllerImpl::GenerateInputEvents()
{
  double time = GetTimeManager().Now();
  for (auto && processor : m_processors)
  {
    if (processor)
    {
      processor->TickAction(time);
      auto event = processor->GetAction();
      if (event.has_value())
        PushInputEvent(*event);
    }
  }
}

void InputControllerImpl::SetInputBindings(const std::span<InputBinding> & bindings)
{
  m_bindings.clear();
  for (auto && binding : bindings)
  {
    auto [it, inserted] = m_bindings.insert({binding, details::BindingParser()});
    if (inserted)
    {
      it->second = details::BindingParser(binding.bindings);
    }
  }
  m_processors.clear();
  for (auto device : m_connectedDevices)
    OnNewInputDeviceConnected(device, true);
}

void InputControllerImpl::OnNewInputDeviceConnected(InputDevice device, bool connected)
{
  if (connected)
  {
    auto [_, inserted] = m_connectedDevices.insert(device);

    auto btnChecker = std::bind(&InputBackend::CheckButtonState, m_backend, std::placeholders::_1,
                                std::placeholders::_2);

    auto axisChecker = std::bind(&InputBackend::CheckAxisState, m_backend, std::placeholders::_1,
                                 std::placeholders::_2);

    for (auto && [binding, parser] : m_bindings)
    {
      m_processors.emplace_back(details::CreateAxisProcessor(binding.type, binding.code, device,
                                                             parser.GetAxesResult(device),
                                                             axisChecker));
      m_processors.emplace_back(details::CreateButtonProcessor(binding.type, binding.code, device,
                                                               parser.GetButtonsResult(device),
                                                               btnChecker));
    }

    // remove nullptrs
    auto it = std::remove(m_processors.begin(), m_processors.end(), nullptr);
    m_processors.erase(it, m_processors.end());
  }
  else //!connected
  {
    m_connectedDevices.erase(device);
    auto range = std::ranges::remove_if(m_processors, [device](auto && proc)
                                        { return !proc || proc->GetDevice() == device; });
    m_processors.erase(range.begin(), range.end());
  }
}


GAME_FRAMEWORK_API InputControllerUPtr CreateInputController(InputBackend & backend)
{
  return std::make_unique<InputControllerImpl>(backend);
}

} // namespace GameFramework
