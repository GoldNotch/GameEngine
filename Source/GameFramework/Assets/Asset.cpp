#include "Asset.hpp"

#include <Utility/Uuid.hpp>
namespace GameFramework
{

Asset::Asset()
  : m_uuid(Utils::GenerateUUID())
{
}

} // namespace GameFramework
