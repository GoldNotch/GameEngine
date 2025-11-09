#include "Rect2d.hpp"

#include <Utility/Utility.hpp>

namespace GameFramework
{
Rect2d::Rect2d(float left, float top, float width, float height)
  : m_left(left)
  , m_top(top)
  , m_width(width)
  , m_height(height)
{
}

size_t Rect2d::Hash() const noexcept
{
  size_t hash = 0;
  Utils::hash_combine(hash, m_left, m_top, m_width, m_height);
  return hash;
}
} // namespace GameFramework
