#pragma once

#include"Predeclare.h"
#include"Utility/Singleton.h"
#include<unordered_map>
#include<string>
#include"Core/CoreDllExport.h"

#include"Utility/NameTable.h"
#include"Utility/UniqueIDTable.h"

#include"Allocator/DestructingPoolAllocator.h"
#include"Asset/Animation/AnimationClip.h"

namespace Quad
{

	class CORE_API_LIB AnimationClipManager:public Singleton<AnimationClipManager>
	{
		friend class ResourceController;
	public:
		AnimationClipManager();
		~AnimationClipManager();

		//static bool AddAnimationClip(AnimationClip* animationClip);

		static AnimationClip* CreateAnimationClip(const std::string& name);
		static AnimationClip* CreateAnimationClipFromFile(const std::string& name,unsigned long long id);

		static AnimationClip* GetAnimationClip(const std::string& name);
		static AnimationClip* GetAnimationClip(unsigned long long id);


	private:
		//std::unordered_map<std::string, AnimationClip*> mAnimationClipUnMap;


		NameTable mNameTable;
		UniqueIDTable<AnimationClip> mIDTable;
		DestructingPoolAllocator<AnimationClip> mPoolAllocator;



	};



}

