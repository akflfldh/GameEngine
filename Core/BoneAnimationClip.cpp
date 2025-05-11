#include "Asset/Animation/BoneAnimationClip.h"

#include"Utility.h"

void Quad::BoneAnimationClip::Update(float localTime, BoneAnimSRT & oBoneAnimSRT)
{
	if (mKeyFrameVector.size() == 0)
	{
	
		DirectX::XMVECTOR identityScale = DirectX::XMVectorSet(1, 1, 1, 0);
		DirectX::XMVECTOR identityRotation = DirectX::XMVectorSet(0, 0, 0, 1);
		DirectX::XMVECTOR identityTranslation = DirectX::XMVectorSet(0, 0, 0, 0);

		DirectX::XMStoreFloat3(&oBoneAnimSRT.mScale, identityScale);
		DirectX::XMStoreFloat4(&oBoneAnimSRT.mRotation, identityRotation);
		DirectX::XMStoreFloat3(&oBoneAnimSRT.mTranslation, identityTranslation);


		//oTransformMatrix = Utility::GetIdentityMatrixF();
		return;
	}


	//키프레임을 보간한다.
	//쿼터니언은 구면선형보간, 나머지는 선형보간

	std::vector<AnimationKeyFrame>::iterator beforeKeyFrameIterator = std::find_if(mKeyFrameVector.begin(), mKeyFrameVector.end(), [&localTime](const AnimationKeyFrame& keyFrame) { return keyFrame.mTime >= localTime ? true : false; });
	

	/*oTransformMatrix = Utility::GetIdentityMatrixF();
	return;*/
	std::vector<AnimationKeyFrame>::iterator afterKeyFrameIterator;
	if (beforeKeyFrameIterator == mKeyFrameVector.end())
	{
		beforeKeyFrameIterator -= 2;
		afterKeyFrameIterator = (beforeKeyFrameIterator - 1);
		localTime = afterKeyFrameIterator->mTime;
		int a = 2;
	}
	else
	{
		afterKeyFrameIterator = (beforeKeyFrameIterator + 1);
	}

	if (afterKeyFrameIterator == mKeyFrameVector.end())
	{
		localTime = beforeKeyFrameIterator->mTime;
		afterKeyFrameIterator = beforeKeyFrameIterator;
		beforeKeyFrameIterator -= 1;
	}



	//보간수행
	float keyFrameTimeInterval = afterKeyFrameIterator->mTime - beforeKeyFrameIterator->mTime;


	float KeyFrameIterpolationTimeFactor  = (localTime - beforeKeyFrameIterator->mTime)/ keyFrameTimeInterval;

	DirectX::XMVECTOR scale = DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&beforeKeyFrameIterator->mScale),
		DirectX::XMLoadFloat3(&afterKeyFrameIterator->mScale), KeyFrameIterpolationTimeFactor);


	DirectX::XMVECTOR translation = DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&beforeKeyFrameIterator->mPosition),
		DirectX::XMLoadFloat3(&afterKeyFrameIterator->mPosition), KeyFrameIterpolationTimeFactor);


	DirectX::XMVECTOR quaternion = DirectX::XMQuaternionSlerp(DirectX::XMLoadFloat4(&beforeKeyFrameIterator->mQuaternion),
		DirectX::XMLoadFloat4(&afterKeyFrameIterator->mQuaternion), KeyFrameIterpolationTimeFactor);





	DirectX::XMStoreFloat3(&oBoneAnimSRT.mScale, scale);
	DirectX::XMStoreFloat4(&oBoneAnimSRT.mRotation, quaternion);
	DirectX::XMStoreFloat3(&oBoneAnimSRT.mTranslation, translation);
	







	DirectX::XMVECTOR zero = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	//DirectX::XMStoreFloat4x4(&oTransformMatrix ,DirectX::XMMatrixAffineTransformation(scale, zero, quaternion, translation));



}

void Quad::BoneAnimationClip::SetAnimationKeyFrameVector(std::vector<AnimationKeyFrame>&& keyframeVector)
{
	mKeyFrameVector = std::move(keyframeVector);
}

void Quad::BoneAnimationClip::SetBoneID(int id)
{
	mBoneID = id;
}

int Quad::BoneAnimationClip::GetBoneID() const
{
	return mBoneID;
}

const std::vector<Quad::AnimationKeyFrame>& Quad::BoneAnimationClip::GetKeyFrameVector() const
{
	return mKeyFrameVector;
	// TODO: 여기에 return 문을 삽입합니다.
}
