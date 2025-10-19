#pragma once
#include <Game/Input.hpp>

namespace Utils
{

struct GlfwInputBinding final
{
  explicit GlfwInputBinding(const GameFramework::InputBinding & binding);
  ~GlfwInputBinding() = default;

  const std::string & GetName() const & noexcept { return m_name; }
  int GetCode() const noexcept { return m_code; }
  bool IsActive() const noexcept;

private:
  std::string m_name;
  int m_code;
  std::vector<int> m_keys;
};

} // namespace Utils
