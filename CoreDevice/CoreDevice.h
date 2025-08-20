#pragma once

#include "CoreDevice/CoreDeviceDllMacro.h"

namespace Core
{

class CORE_DEVICE_API CoreDevice
{
  public:
    static CoreDevice *GetInstance();
    CoreDevice();
    virtual ~CoreDevice() = 0;

    // virtual void Initialize() = 0;

  private:
};

} // namespace Core