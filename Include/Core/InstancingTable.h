#pragma once

#include"Predeclare.h"
#include<vector>
#include<unordered_map>
#include"InstancingTableType.h"
#include"Core/CoreDllExport.h"
namespace Quad
{

	

	class CORE_API_LIB InstancingTable
	{
	public:
		InstancingTable() = default;
		~InstancingTable()=default;
		
			
		static void RegisterEntityInstance(const std::wstring & groupName, Entity * entity);
		static bool GetInstanceDataAndStructuredBuffer(const std::wstring & groupName,
			std::pair<InstancingData*, StructuredBuffer*>& oData);

	private:
		static std::unordered_map<std::wstring, std::pair<InstancingData*,StructuredBuffer *>> mInstancingTable;
		



	};


}

//구조적버퍼도 디폴트힙에 존재하는 버퍼가있을수있고 
//업로드 힙에존재하는 버퍼가있을수있겠지
//인스턴싱에서는 당연히 업로드힙에존재하는 구조적버퍼를 사용하겠죠
