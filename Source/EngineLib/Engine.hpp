#pragma once
//C++ interface with App (header only)
namespace App
{

struct IAppObject
{
  virtual ~IAppObject() = default;
  virtual void Tick() = 0; ///< tick for object
};


/// @brief Objects consists of components, but components doesn't stored in objects, objects have only references
///			Systems are iterate over related components and update each. It's App-implementable object
struct IComponentSystem
{
  virtual ~IComponentSystem() = default;
  virtual void Update() = 0; ///< updates related components
};

} // namespace App
