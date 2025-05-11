#pragma once


#include"Asset/Animation/Bone.h"
#include"Utility/Singleton.h"
#include"Allocator/DestructingPoolAllocator.h"

#include"Core/CoreDllExport.h"

namespace Quad
{
	class CORE_API_LIB BoneFactory:public Singleton<BoneFactory>
	{
	public:
		BoneFactory();
		~BoneFactory();
		

		static Bone* CreateBone();
		static void ReleaseBone(Bone* bone);

	private:
		DestructingPoolAllocator<Bone> mPoolAllocator;


	};
}

