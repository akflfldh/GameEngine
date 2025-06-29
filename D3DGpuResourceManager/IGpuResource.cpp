#include "IGpuResource.h"

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
