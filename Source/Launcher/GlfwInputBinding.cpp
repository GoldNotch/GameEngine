#include "GlfwInputBinding.hpp"

namespace Utils
{

GlfwInputBinding::GlfwInputBinding(const GameFramework::InputBinding & binding)
  : m_name(binding.name)
  , m_code(binding.code)
{
  std::string_view bindingStr(binding.bindings);
}

} // namespace Utils
