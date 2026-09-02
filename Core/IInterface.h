#pragma once

#include <stdint.h>

#include <Core/CoreDllExport.h>

namespace Core
{
class CORE_API_LIB IRenderableComponent
{
  public:
    IRenderableComponent();
    virtual ~IRenderableComponent() = 0;

    virtual void SetRenderID(uint32_t id) = 0;

  private:
};
} // namespace Core
