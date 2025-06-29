#include "IGpuResourceManager.h"


GRM::IGpuResourceManager* GRM::IGpuResourceManager::mImplManager = nullptr;


GRM::IGpuResourceManager::IGpuResourceManager()
{
}

GRM::IGpuResourceManager::~IGpuResourceManager()
{
}

GRM::IGpuResourceManager* GRM::IGpuResourceManager::GetInstance()
{
	if (mImplManager == nullptr)
	{
		//에러출력

		return nullptr;
	}
	else
		return mImplManager;

}

void GRM::IGpuResourceManager::SetGpuResourceManagerImpl(IGpuResourceManager* pImpl)
{
	mImplManager = pImpl;
}
