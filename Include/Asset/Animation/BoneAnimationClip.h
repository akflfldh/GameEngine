#pragma once
#include"Predeclare.h"
#include"AnimationKeyFrame.h"
#include<vector>


#include"CoreDllExport.h"

#include"Core/BoneAnimSRT.h"

namespace Quad
{
	//이뼈대가 어떻게 에니메이션되는가

	class CORE_API_LIB BoneAnimationClip
	{
	public:
		BoneAnimationClip() = default;
		~BoneAnimationClip() = default;

		void Update(float localTime, BoneAnimSRT& oBoneAnimSRT);


		void SetAnimationKeyFrameVector(std::vector<AnimationKeyFrame>&& keyframeVector);
		void SetBoneID(int id);
		int GetBoneID() const;

		const std::vector<AnimationKeyFrame>& GetKeyFrameVector() const;

	private:
		std::vector<AnimationKeyFrame> mKeyFrameVector;
		int mBoneID; //연관된 Bone의 넘버

	};

}
