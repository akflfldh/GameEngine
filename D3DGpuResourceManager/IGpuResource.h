#pragma once
#include "D3DGpuResourceManager/GpuResourceDllMacro.h"

namespace GRM
{
enum class EGpuResourceType
{
    eBuffer = 0,
    e2DTexture,
    eSampler,
};

class GPURESOURCE_MANAGER_API IGpuResource
{
  public:
    virtual ~IGpuResource() = 0;

    GRM::EGpuResourceType GetGpuResourceType() const;

    // 내부적으로 gpuResourceManager의 release를 호출해서 제거할것을 요청한다,(정확히는 등록한다)
    // 이것을 스마트포인터가 manager의 release를 호출해줘야할거같다.
    virtual void Release();

  protected:
    IGpuResource();

    GRM::EGpuResourceType mGpuResourceType;
};

} // namespace GRM