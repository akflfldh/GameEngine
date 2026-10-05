#pragma once
#include <stdint.h>
#include <atomic>

#include <Core/CoreDllExport.h>

namespace Core
{
// World와 Map이 공유하는 프로세스 단위 런타임 ID 발급기다. 0은 미할당 값으로 남긴다.
// 발급한 ID는 재사용하지 않으며, 객체나 프록시의 수명은 관리하지 않는다.
class CORE_API_LIB RenderIDManager
{
  public:
    static RenderIDManager *GetInstance();
    RenderIDManager();
    ~RenderIDManager();

    uint32_t AllocID();

  private:
    std::atomic<uint32_t> mNextID;
};
} // namespace Core
