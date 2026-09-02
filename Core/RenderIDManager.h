#pragma once
#include <stdint.h>

#include <Core/CoreDllExport.h>

namespace Core
{
class CORE_API_LIB RenderIDManager
{
  public:
    static RenderIDManager *GetInstance();
    RenderIDManager();
    ~RenderIDManager();

    uint32_t AllocID();

  private:
    uint32_t mNextID;
};
} // namespace Core
