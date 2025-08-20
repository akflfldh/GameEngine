#pragma once

#include "Core/CoreDllExport.h"

#ifdef D3DX
#include <Windows.h>
#endif
namespace Quad
{
class CORE_API_LIB GlobalAppHelper
{
  public:
    static GlobalAppHelper *GetInstance();

    void Initialize(HINSTANCE hinstance);

    static HINSTANCE GetHinstance();

  private:
    static HINSTANCE mHinstance;
};

} // namespace Quad
