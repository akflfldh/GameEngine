#pragma once

#include "CoreDevice/CoreDeviceDllMacro.h"
#include <stdint.h>

namespace Core
{

class CORE_DEVICE_API CoreDevice
{
  public:
    static CoreDevice *GetInstance();
    CoreDevice();
    virtual ~CoreDevice() = 0;

    // virtual void Initialize() = 0;

    virtual void FlushCommandQueue() = 0;
    virtual uint64_t GetNextFenceValue() = 0;
    virtual void IncreaseNextFenceValue() = 0;

    virtual void WaitFenceValue(uint64_t value) = 0;

    // 설정된 value값 리턴
    virtual void SetFence(uint64_t value) = 0;

    virtual void FlushGpu() = 0;

  private:
};

} // namespace Core