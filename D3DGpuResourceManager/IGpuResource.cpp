#include "IGpuResource.h"
#include"IGpuResourceManager.h"
GRM::IGpuResource::IGpuResource()
{
}

GRM::IGpuResource::~IGpuResource()
{



}


GRM::EGpuResourceType GRM::IGpuResource::GetGpuResourceType() const
{
	return mGpuResourceType;
}


void GRM::IGpuResource::Release()
{
	auto manager =	IGpuResourceManager::GetInstance();
	
	manager->Release(this);
}