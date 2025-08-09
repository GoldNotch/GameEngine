#pragma once
#include <memory>
#include <unordered_map>

#include "Resource.hpp"

namespace GameFramework
{

struct ResourceLoader final
{
  explicit ResourceLoader() = default;
  ~ResourceLoader() = default;

  template<typename ResourceT>
  IResource * GetResource(const std::filesystem::path & path)
  {
    auto [it, inserted] = m_resources.insert({path, nullptr});
    if (inserted)
      it->second = std::make_unique<ResourceT>(path);
    return it->second.get();
  }

private:
  //DataBase of resources
  std::unordered_map<std::filesystem::path, std::unique_ptr<IResource>> m_resources;
};

} // namespace GameFramework
