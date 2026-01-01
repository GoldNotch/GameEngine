#include "Uuid.hpp"

#include <cassert>
#include <stdexcept>

#include <Utility/Random.hpp>
#include <uuid.h> //stduuid

static_assert(sizeof(GameFramework::Uuid) == sizeof(uuids::uuid));

namespace GameFramework
{

Uuid::Uuid()
{
  new (m_bytes) uuids::uuid();
}

Uuid::Uuid(std::span<const char, 16> bytes16)
{
  for (size_t i = 0; i < 16; ++i)
    m_bytes[i] = static_cast<std::byte>(bytes16[i]);
}

Uuid::Uuid(std::span<const std::byte, 16> bytes16)
{
  for (size_t i = 0; i < 16; ++i)
    m_bytes[i] = bytes16[i];
}

Uuid::~Uuid() = default;

bool Uuid::operator==(const Uuid & rhs) const noexcept
{
  const uuids::uuid * l = reinterpret_cast<const uuids::uuid *>(m_bytes);
  const uuids::uuid * r = reinterpret_cast<const uuids::uuid *>(rhs.m_bytes);
  return *l == *r;
}

bool Uuid::operator!=(const Uuid & rhs) const noexcept
{
  const uuids::uuid * l = reinterpret_cast<const uuids::uuid *>(m_bytes);
  const uuids::uuid * r = reinterpret_cast<const uuids::uuid *>(rhs.m_bytes);
  return *l != *r;
}

bool Uuid::operator<(const Uuid & rhs) const noexcept
{
  const uuids::uuid * l = reinterpret_cast<const uuids::uuid *>(m_bytes);
  const uuids::uuid * r = reinterpret_cast<const uuids::uuid *>(rhs.m_bytes);
  return *l < *r;
}

size_t Uuid::Hash() const noexcept
{
  return std::hash<uuids::uuid>{}(*reinterpret_cast<const uuids::uuid *>(m_bytes));
}

std::string Uuid::ToString() const noexcept
{
  return uuids::to_string(*reinterpret_cast<const uuids::uuid *>(m_bytes));
}

std::optional<Uuid> Uuid::MakeFromString(const std::string_view & str)
{
  auto id = uuids::uuid::from_string(str);
  if (!id.has_value())
    return std::nullopt;
  auto span = id->as_bytes();
  return Uuid(span);
}

Uuid Uuid::MakeRandomUuid()
{
  static Random random;
  auto id = uuids::uuid_random_generator{random.GetEngine()}();
  auto span = id.as_bytes();
  return Uuid(span);
}

} // namespace GameFramework
