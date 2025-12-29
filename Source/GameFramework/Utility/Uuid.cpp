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
  new (x) uuids::uuid();
}

Uuid::Uuid(const std::string & str)
{
  auto id = uuids::uuid::from_string(str);
  if (!id.has_value())
    throw std::runtime_error("Incorrect UUID format");
  new (x) uuids::uuid(*id);
}

Uuid::Uuid(const std::span<char, 16> & bytes16)
{
  new (x) uuids::uuid(bytes16.begin(), bytes16.end());
}

Uuid::Uuid(const std::span<uint8_t, 16> & bytes16)
{
  new (x) uuids::uuid(bytes16.begin(), bytes16.end());
}

Uuid::~Uuid()
{
  delete reinterpret_cast<uuids::uuid *>(x);
}

bool Uuid::operator==(const Uuid & rhs) const noexcept
{
  const uuids::uuid * l = reinterpret_cast<const uuids::uuid *>(x);
  const uuids::uuid * r = reinterpret_cast<const uuids::uuid *>(rhs.x);
  return *l == *r;
}

bool Uuid::operator!=(const Uuid & rhs) const noexcept
{
  const uuids::uuid * l = reinterpret_cast<const uuids::uuid *>(x);
  const uuids::uuid * r = reinterpret_cast<const uuids::uuid *>(rhs.x);
  return *l != *r;
}

bool Uuid::operator<(const Uuid & rhs) const noexcept
{
  const uuids::uuid * l = reinterpret_cast<const uuids::uuid *>(x);
  const uuids::uuid * r = reinterpret_cast<const uuids::uuid *>(rhs.x);
  return *l < *r;
}

std::string Uuid::ToString() const noexcept
{
  return uuids::to_string(*reinterpret_cast<const uuids::uuid *>(x));
}

Uuid Uuid::MakeRandomUuid()
{
  static Random random;
  auto id = uuids::uuid_random_generator{random.GetEngine()}();
  auto span = id.as_bytes();
  return Uuid({span.begin(), span.end()});
}

} // namespace GameFramework
