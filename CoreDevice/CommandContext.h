#pragma once
#include "CoreDevice/CoreDeviceDllMacro.h"

namespace Core
{

class CORE_DEVICE_API CommandContext
{
  public:
    CommandContext();
    virtual ~CommandContext() = 0;

    virtual void Initialize() = 0;

    // 다음프레임작업준비
    virtual void Reset(int frameIndex) = 0;

    // 명령  기록 종료
    virtual void Close() = 0;

  private:
};
} // namespace Core
