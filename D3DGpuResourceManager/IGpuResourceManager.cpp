#include "D3DGpuResourceManager/IGpuResourceManager.h"
#include <Logger/Logger.h>
#include <assert.h>

GRM::IGpuResourceManager *GRM::IGpuResourceManager::mImplManager = nullptr;

GRM::IGpuResourceManager::IGpuResourceManager() {}

GRM::IGpuResourceManager::~IGpuResourceManager() {}

GRM::IGpuResourceManager *GRM::IGpuResourceManager::GetInstance()
{
    if (mImplManager == nullptr)
    {
        // 에러출력
        LOG_MESSAGE_CRITICAL(
            "GpuResourceManager",
            "IGpuResourceManagerImpl이 설정되지않았습니다. SetGpuResourceManagerImpl호출을 확인하세요.");
        assert(0);
        return nullptr;
    }
    else
        return mImplManager;
}

void GRM::IGpuResourceManager::SetGpuResourceManagerImpl(IGpuResourceManager *pImpl)
{
    mImplManager = pImpl;
}
