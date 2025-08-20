#pragma once

#include "D3DGpuResourceManager/GpuResourceDllMacro.h"

namespace GRM
{

class IGpuResourceManager;
class IGpuResource;
class GRMPtr;
struct GRMPtrControlBlcok
{
    IGpuResource *mGpuResource;
    size_t mRefCount;
    // IGpuResourceManager* mGpuResourceManager;	//이건 필요없을수도, 전역적으로 얻을수있기에
};

GPURESOURCE_MANAGER_API GRMPtr MakeGRMPtr(IGpuResource *gpuResource);

class GPURESOURCE_MANAGER_API GRMPtr
{
  public:
    GRMPtr();
    GRMPtr(IGpuResource *gpuResource);
    ~GRMPtr();

    GRMPtr(const GRMPtr &ptr);
    GRMPtr &operator=(const GRMPtr &ptr);

    GRMPtr(GRMPtr &&rhs);
    GRMPtr &operator=(GRMPtr &&rhs);

    IGpuResource *getResource() const;

  private:
    // 내부리소스해제
    //  refCount가 0에 도달하면 호출
    void Release();

  private:
    GRMPtrControlBlcok *mControlBlock;
};
} // namespace GRM