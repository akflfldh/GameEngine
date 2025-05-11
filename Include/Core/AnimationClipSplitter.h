#pragma once



#include"Predeclare.h"
#include"Utility/Singleton.h"

#include<string>
#include"Core/CoreDllExport.h"
namespace Quad
{
	class CORE_API_LIB AnimationClipSplitter:public Singleton<AnimationClipSplitter>
	{
	public:
		AnimationClipSplitter() = default;
		~AnimationClipSplitter() = default;


		static AnimationClip* SplitAnimationClip(AnimationClip * sourceAnimationClip,const std::string& newAnimationClipName, int startFrame, int endFrame);


	private:







	};

}
