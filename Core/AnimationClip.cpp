#include "Asset/Animation/AnimationClip.h"

Quad::AnimationClip::AnimationClip()
	:Asset("",EAssetType::eAnimationClip), mDefaultMesh(nullptr),mDefaultSkeleton(nullptr)
{
}

void Quad::AnimationClip::Copy(Quad::AnimationClip&& animClip)
{
	mFrameEndCount = animClip.mFrameEndCount;
	mEndTimeSecond = animClip.mEndTimeSecond;
	mTimeMode = animClip.mTimeMode;
	mDefaultMesh = animClip.mDefaultMesh;
	mDefaultSkeleton = animClip.mDefaultSkeleton;

	mBoneAnimationClipVector = std::move(animClip.mBoneAnimationClipVector);

}


void  Quad::AnimationClip::Update(float localTime, const std::vector<int>& boneHierarchyVector, std::vector<BoneAnimSRT>& oBoneAnimSRTVector)
{
	oBoneAnimSRTVector.resize(boneHierarchyVector.size());
//	std::vector<DirectX::XMFLOAT4X4> boneAnimationMatrixVector(boneHierarchyVector.size());


	//top - down방식으로 계산하는것.
	//본에 아무것도 키프레임이없다 그럼 단위행렬

	//본자체에서 키프레임을 보간해서 변환행렬을 구한다.
	
	//본마다가지고있으면 낭비지 ,
	//리턴하도록한다.


	for (int boneIndex = 0; boneIndex < boneHierarchyVector.size(); ++boneIndex)
	{
		BoneAnimSRT& currentBoneSRT = oBoneAnimSRTVector[boneIndex];
		mBoneAnimationClipVector[boneIndex].Update(localTime, currentBoneSRT);

		//부모의 변환행렬을 곱한다.
		int parentBoneIndex = boneHierarchyVector[boneIndex];
		//최상위 bone
		if (parentBoneIndex == -1)
			continue;

		//const DirectX::XMFLOAT4X4& parentBoneTransformMatrix = boneAnimationMatrixVector[parentBoneIndex];


		//부모 bone의 행렬과곱
		//DirectX::XMStoreFloat4x4(&currentBoneTransformMatrix, DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&currentBoneTransformMatrix), DirectX::XMLoadFloat4x4(&parentBoneTransformMatrix)));
	}

	//oBoneAnimaitonMatrixVector = std::move(boneAnimationMatrixVector);
	return;

}

void Quad::AnimationClip::SetBoneAnimationClipVector(std::vector<BoneAnimationClip>&& boneAnimationClipVector)
{
	mBoneAnimationClipVector = boneAnimationClipVector;
}

const std::vector<Quad::BoneAnimationClip>& Quad::AnimationClip::GetBoneAnimationClipVector() const
{
	return mBoneAnimationClipVector;
	// TODO: 여기에 return 문을 삽입합니다.
}

void Quad::AnimationClip::SetFrameEndCount(int frameCount)
{
	mFrameEndCount = frameCount;

	switch (mTimeMode)
	{
	case EAnimationTimeMode::eFPS30:
		mEndTimeSecond = (float)mFrameEndCount / 30;

		break;
	case EAnimationTimeMode::eFPS60:
		mEndTimeSecond = (float)mFrameEndCount/60;
		break;
	}




}

void Quad::AnimationClip::SetEndTimeSecond(float time)
{
	mEndTimeSecond = time;
}

float Quad::AnimationClip::GetEndTimeSecond() const
{
	return mEndTimeSecond;
}

int Quad::AnimationClip::GetEndFrameCount() const
{
	return mFrameEndCount;
}

void Quad::AnimationClip::SetTimeMode(EAnimationTimeMode timeMode)
{
	mTimeMode = timeMode;
	SetFrameEndCount(mFrameEndCount);



}

Quad::EAnimationTimeMode Quad::AnimationClip::GetTimeMode() const
{
	return mTimeMode;
}

void Quad::AnimationClip::SetDefaultMesh(Mesh* defaultMesh)
{
	mDefaultMesh = defaultMesh;
}

Quad::Mesh* Quad::AnimationClip::GetDefaultMesh() const
{
	return mDefaultMesh;
}

void Quad::AnimationClip::SetDefaultSkeleton(Skeleton* defaultSkeleton)
{
	mDefaultSkeleton = defaultSkeleton;


}

Quad::Skeleton* Quad::AnimationClip::GetDefaultSkeleton() const
{
	return mDefaultSkeleton;
}

