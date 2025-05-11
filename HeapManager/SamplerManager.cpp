#include "HeapManager/SamplerManager.h"

void Quad::SamplerManager::RegisterSampler(const std::wstring& name, ViewIndex viewIndex)
{
	auto instance = GetInstance();
	instance->mSamplerUnMapTable.try_emplace(name, viewIndex);
}

ViewIndex Quad::SamplerManager::GetSamplerViewIndex(const std::wstring& name) 
{
	auto instance = GetInstance();
	std::unordered_map<std::wstring, ViewIndex> ::const_iterator it = instance->mSamplerUnMapTable.find(name);
	return it != instance->mSamplerUnMapTable.end() ? it->second: InvalidViewIndex;

}
