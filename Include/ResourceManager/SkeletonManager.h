#pragma once

#include"Predeclare.h"
#include"Utility/Singleton.h"
#include<unordered_map>
#include<string>
#include"Core/CoreDllExport.h"



#include"Allocator/DestructingPoolAllocator.h"
#include"Utility/NameTable.h"
#include"Utility/UniqueIDTable.h"
#include"Asset/Skeleton/Skeleton.h"

namespace Quad
{

	class CORE_API_LIB SkeletonManager:public Singleton<SkeletonManager>
	{

		friend class ResourceController;

	public:
		SkeletonManager();
		~SkeletonManager() = default;


		static Skeleton* CreateSkeleton(const std::string& name);
		static Skeleton* CreateSkeletonFromFile(const std::string& name, unsigned long long id);
	//	static bool AddSkeleton(Skeleton* skeleton);
		static Skeleton* GetSkeleton(const std::string& name) ;
		static Skeleton* GetSkeleton(unsigned long long id) ;


	private:
		//std::unordered_map<std::string, Skeleton*> mSkeletonUnMap;

		UniqueIDTable<Skeleton>mIDTable;
		NameTable mNameTable;
		DestructingPoolAllocator<Skeleton> mSkeletonAllocator;


	};

}
