#pragma once

struct AssetLoader final
{
  AssetLoader();
  ~AssetLoader();

  std::shared_ptr<void> LoadTexture(const wchar_t * path);
  std::shared_ptr<void> LoadMesh(const wchar_t* path);
};