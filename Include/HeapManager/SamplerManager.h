#pragma once


#include"Predeclare.h"
#include<Windows.h>
#include<unordered_map>
#include"DescriptorHeapType.h"

#include"Utility/Singleton.h"


#include"HeapManager/HeapManagerDllExport.h"

namespace Quad
{
	class HEAPMANAGER_API_LIB SamplerManager:public Singleton<SamplerManager>
	{
	public:
		SamplerManager() = default;
		~SamplerManager() = default;

		static void RegisterSampler(const std::wstring& name, ViewIndex viewIndex);
		static ViewIndex GetSamplerViewIndex(const std::wstring& name) ;

	private:
		std::unordered_map<std::wstring, ViewIndex> mSamplerUnMapTable;


	};

}
