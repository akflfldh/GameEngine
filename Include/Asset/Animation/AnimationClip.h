#pragma once

#include"Predeclare.h"

#include<string>
#include<vector>
#include"Asset/Asset.h"
#include"Asset/Animation/BoneAnimationClip.h"

#include"Core/CoreDllExport.h"

#include"BoneAnimSRT.h"

namespace Quad
{

	class CORE_API_LIB AnimationClip:public Asset
	{
		friend class AnimationClipManager;
		

	public:
		AnimationClip();
		~AnimationClip() = default;


		void Copy(AnimationClip&& animClip);

		void Update(float localTime, const std::vector<int>& boneHierarchyVector,
			std::vector<BoneAnimSRT>& oBoneAnimSRTVector);


		void SetBoneAnimationClipVector(std::vector<BoneAnimationClip>&& boneAnimationClipVector);
		const std::vector<BoneAnimationClip>& GetBoneAnimationClipVector() const;
		

		void SetEndTimeSecond(float time);
		float GetEndTimeSecond()const;

		void SetFrameEndCount(int frameCount);
		int GetEndFrameCount() const;


		void SetTimeMode(EAnimationTimeMode timeMode);
		EAnimationTimeMode GetTimeMode()const;
		

		//리소스들을 핸들테이블로 관리하게 바꿀것이니 여기도 바뀔것
		void SetDefaultMesh(Mesh * defaultMesh);
		Mesh* GetDefaultMesh() const;

		void SetDefaultSkeleton(Skeleton* defaultSkeleton);
		Skeleton* GetDefaultSkeleton()const;

	private:
		std::vector<BoneAnimationClip> mBoneAnimationClipVector;


		//int mFrameStartCount = 0;
		int mFrameEndCount = 0;

		float mEndTimeSecond = 0.0f;


	//	int mKeyFrameCount;
		//FPS
		EAnimationTimeMode mTimeMode;


		Mesh* mDefaultMesh;
		Skeleton* mDefaultSkeleton;

	};

}
