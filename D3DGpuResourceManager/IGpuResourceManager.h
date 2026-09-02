#pragma once

#include "CommonHeader/GpuTypes.h"
#include "D3DGpuResourceManager/GpuResourceDllMacro.h"
#include "D3DGpuResourceManager/IGpuResource.h"
#include <D3DGpuResourceManager/GpuResourceTypes.h>

namespace GRM
{

class GRMPtr;

class GPURESOURCE_MANAGER_API IGpuResourceManager
{
  public:
    virtual ~IGpuResourceManager() = 0;

    virtual GRMPtr CreateBuffer(const BufferDesc &bufferDesc) = 0;

    virtual GRMPtr CreateTexture(const TextureDesc &textureDesc) = 0;

#pragma region Render
    // 렌더모듈에서 사용하는 메서드

    virtual GRM::IGpuResource *CreateOrGetTextureFromPool(const GRM::TextureDesc &textureDesc) = 0;
    virtual void ReturnResourceToPool(GRM::IGpuResource *gpuResource) = 0;

#pragma endregion Render

    static IGpuResourceManager *GetInstance();
    static void SetGpuResourceManagerImpl(IGpuResourceManager *pImpl);

    virtual GRMPtr RegisterSwapChainBackBuffer(void *innerResoure, void *windowHandle) = 0;
    virtual void ResizeSwapChainBackBuffer(const GRMPtr &resource, void *InnerResoure) = 0;
    virtual void ReleaseSwapChainBackBuffer(const GRMPtr &resoure) = 0;

    virtual GRM::GRMPtr GetSwapChainBackBuffer(void *windowHandle, int index) = 0;

    // 외부사용자는 신경쓸거없이 사용만하면되고 내부 엔진시스템들은 기존내부리소스들이 모두 무효화된다는것을 인지할것
    virtual void ChangeTextureData(const GRMPtr &texture, const TextureDesc &textureDesc) = 0;

    virtual bool UploadBufferData(const GRMPtr &buffer, void *data, size_t elementSize, size_t elementNum,
                                  size_t bufferOffset = 0) = 0;

    // 여러가지설정을받을수있다.//현재는 그냥 디폴트
    virtual GRMPtr CreateSampler(const SamplerDesc &samplerDesc) = 0;

    virtual void TransitionResourceState(GRM::IGpuResource *resource, EResourceState afterState) = 0;
    virtual void SetResourceState(GRM::IGpuResource *resource, EResourceState state) = 0;
    // void UploadDataToBuffer(buffer, data)

    // 일반적으로 IGpuResource를참조하는 외부에서 IGpuResource의 release를 호출하면된다.
    // 그러면 내부적으로 manager의 Release함수를 호출
    // 이함수는 그렇게 Release요청을받은 gpuResource들을 모은다.
    virtual void Release(IGpuResource *resource) = 0;

    virtual void FlushGarbageCollect() = 0;
    virtual GRM::IGpuResource *GetDefaultTexture() const = 0;

  protected:
    IGpuResourceManager();

  private:
    static IGpuResourceManager *mImplManager;
};

} // namespace GRM
