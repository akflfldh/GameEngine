#pragma once

#include <Core/CoreType.h>
#include <CoreBase/CallbackSystem.h>

namespace Core
{
using MultinputCallbackSystem = Core::MultiCallbackSystem<const Core::InputData &>;

}