#pragma once
#include <array>
#include <numeric>
#include <random>
#include <type_traits>

namespace GameFramework
{

struct Random final
{
  Random()
  {
    std::random_device rd;
    auto seed_data = std::array<int, std::mt19937::state_size>{};
    std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
    std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
    m_engine = std::mt19937(seq);
  }

  explicit Random(uint32_t seed)
    : m_engine(seed)
  {
  }

  template<typename T>
    requires(std::is_arithmetic_v<T>)
  T Generate(T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max()) const
  {
    if constexpr (std::is_integral_v<T>)
    {
      std::uniform_int_distribution<T> dist(min, max);
      return dist(m_engine);
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
      std::uniform_real_distribution<T> dist(min, max);
      return dist(m_engine);
    }
  }

  const std::mt19937 & GetEngine() const & noexcept { return m_engine; }
  std::mt19937 & GetEngine() & noexcept { return m_engine; }

private:
  std::mt19937 m_engine;
};

} // namespace GameFramework
