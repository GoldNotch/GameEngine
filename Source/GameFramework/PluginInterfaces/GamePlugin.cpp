#include "GamePlugin.hpp"

#include <Utility/Utility.hpp>
namespace GameFramework
{

void GamePlugin::ProcessInput()
{
  auto evt = ConsumeInputEvent();
  while (evt.has_value())
  {
    std::visit(Utils::overloaded{[this](const EventAction & evt) { OnAction(evt); },
                                 [this](const ContinousAction & action) { OnAction(action); },
                                 [this](const AxisAction & axis)
                                 {
                                   OnAction(axis);
                                 }},
               *evt);
    evt = ConsumeInputEvent();
  }
}

} // namespace GameFramework
