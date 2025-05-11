#include "Component/AnimationComponent.h"
#include"Asset/Animation/AnimationClip.h"
#include"Asset/Skeleton/Skeleton.h"
#include"Utility/Utility.h"


#include"Object/Object.h"
#include"Map/Map.h"
#include"AnimationUpdateSystem.h"


#include"Parser/JsonParser.h"
#include"ResourceManager/AnimationClipManager.h"
#include"ResourceManager/SkeletonManager.h"
#include"RegisterAnimStateTransitionCallbackClassManager.h"

Quad::AnimationComponent::AnimationComponent(SceneElement * sceneElement)
	:ModelBaseComponent(sceneElement),mSkeleton(nullptr), mStateTransitionFlag(false), mDestAnimationState(nullptr),
	mCurrAnimStateTransitionTime(0.0f)
{


}

Quad::AnimationComponent::~AnimationComponent()
{
	/*Object * object= static_cast<Object*>(GetDestSceneElement());

	
	RegisterAnimStateTransitionCallbackClassManager * registerAnimStateTransitionCallbackClassManager =	map->GetRegisterAnimStateTransitionCallbackClassManager();


	registerAnimStateTransitionCallbackClassManager->Release(registerAnimStateTransitionCallbackClassManager->GetRegisterAnimStateTransitionCallbackClass(this));*/
}



Quad::AnimationComponent& Quad::AnimationComponent::operator=(const AnimationComponent& component)
{
	mSkeleton = component.mSkeleton;
	mUpdateFlag = component.mUpdateFlag;

	mFinalMatrixVector.resize(component.mFinalMatrixVector.size(), Utility::GetIdentityMatrixF());
	mFinalInverseTransposeMatrixVector.resize(component.mFinalInverseTransposeMatrixVector.size(), Utility::GetIdentityMatrixF());

	for (std::unordered_map<std::string, AnimationState*>::const_iterator it = component.mAnimationStateUnMap.cbegin();
		it != component.mAnimationStateUnMap.cend();   ++it)
	{
		const std::string& stateName = it->first;
		AddState(stateName, it->second->GetAnimationClip());
	}

	SetCurrentAnimationState(component.mCurrentAnimationState->GetName());


	return *this;
	// TODO: 여기에 return 문을 삽입합니다.
}

void Quad::AnimationComponent::Start()
{
	if (mCurrentAnimationState != nullptr)
		mCurrentAnimationState->Start();



	//상태전이조건등록 



}

void Quad::AnimationComponent::Update(float deltaTime)
{

	std::vector<DirectX::XMFLOAT4X4> boneAnimationMatrixVector;
	if (mCurrentAnimationState == nullptr || GetUpdateFlag()== false)
	{
		return;
		//boneAnimationMatrixVector.resize(mSkeleton->GetBoneHiererachyVector().size(), Utility::GetIdentityMatrixF());
	}
	else if (mStateTransitionFlag)
	{
		//상태전이중


		//mCurrentAnimationState

		std::vector<BoneAnimSRT> currBoneAnimSRTVector;
		mCurrentAnimationState->Update(deltaTime, currBoneAnimSRTVector);


		//mDestAnimationState
		std::vector<BoneAnimSRT> destBoneAnimSRTVector;
		mDestAnimationState->Update(deltaTime, destBoneAnimSRTVector);


		mCurrAnimStateTransitionTime += deltaTime;
		std::vector<BoneAnimSRT> lerpBoneAnimSRTVector;			
		CrossFading(currBoneAnimSRTVector,destBoneAnimSRTVector,lerpBoneAnimSRTVector);


		if (mCurrAnimStateTransitionTime >= mCurrAnimStateTransition->mTransitionDuration)
		{
			EndAnimStateTransition();		
		}


		CacluateBoneLocalTransformMatrix(lerpBoneAnimSRTVector, boneAnimationMatrixVector);

	}
	else
	{
		//전이중이 아닌 유일상태
		std::vector<BoneAnimSRT> boneAnimSRTVector;
		mCurrentAnimationState->Update(deltaTime, boneAnimSRTVector);

		CacluateBoneLocalTransformMatrix(boneAnimSRTVector, boneAnimationMatrixVector);

	}





	


//
//	//둘중하나라도없다면 작동하지않아.
//	if (mAnimationClip == nullptr || mSkeleton == nullptr)
//		return;
//
//
//	mCurrentAnimationClipLocalTime += deltaTime;
//
//	if (mCurrentAnimationClipLocalTime >= mAnimationClip->GetEndTimeSecond())
//		mCurrentAnimationClipLocalTime = 0.0f;
//	
//
//	//여기서 본들의 키프레임들을  보간하고 변환행렬을 계산한다.
//
//
//	//이 에니메이션 컴포넌트의 설정에따라 deltaTime을 조정해서 넘기면 느리거나 빨라지는 효과를 낼수있을거같다.
//	//뼈대들의 변환행렬을 리턴받는다.또는 출력매개변수로받는다.	=
////	std::vector<DirectX::XMFLOAT4X4> boneAnimationMatrixVector;
//	mAnimationClip->Update(mCurrentAnimationClipLocalTime,mSkeleton->GetBoneHiererachyVector(), boneAnimationMatrixVector);


	//최종변환행렬을 계산한다.
	CacluateFinalTransformMatrix(boneAnimationMatrixVector);



}

//void Quad::AnimationComponent::SetAnimationClip(AnimationClip* clip)
//{
//	mAnimationClip = clip;
//
//}

void Quad::AnimationComponent::SetSkeleton(Skeleton* skeleton)
{
	mSkeleton = skeleton;
	mFinalMatrixVector.resize(skeleton->GetOffsetMatrixVector().size(), Utility::GetIdentityMatrixF());
	mFinalInverseTransposeMatrixVector.resize(skeleton->GetOffsetMatrixVector().size(), Utility::GetIdentityMatrixF());
}

const std::vector<DirectX::XMFLOAT4X4>& Quad::AnimationComponent::GetFinalTransformMatrixVector() const
{
	return mFinalMatrixVector;
	// TODO: 여기에 return 문을 삽입합니다.
}

const std::vector<DirectX::XMFLOAT4X4>& Quad::AnimationComponent::GetFinalInverseTransposeTransformMatrixVector() const
{
	return mFinalInverseTransposeMatrixVector;
	// TODO: 여기에 return 문을 삽입합니다.
}

bool Quad::AnimationComponent::AddState(const std::string& stateName, AnimationClip* animationClip)
{

	

	std::unordered_map<std::string,AnimationState *> ::iterator it= mAnimationStateUnMap.find(stateName);
	if (it != mAnimationStateUnMap.end())
		return false;


	AnimationState* newState = new AnimationState(stateName);
	newState->SetAnimationClip(animationClip);
	newState->SetDestAnimationComponent(this);
	mAnimationStateUnMap.emplace(stateName, newState);

	if (mAnimationStateUnMap.size() == 1)
	{
		//기존에 상태가없었는데 최소 하나의 에니메이션상태가 존재하게되었다. 
		//이제 완전하게 에니메이션이 작동할수있다.
		//따라서 map의 에니메이션Update시스템에 이 컴포넌트의 소유자인오브젝트를 추가한다.
		Object * object= static_cast<Object*>(GetDestSceneElement());
		
		Map * map =	object->GetMap();
		map->AddObjectToAnimationUpdateSystem(object);
	}

	return true;

}

bool Quad::AnimationComponent::RemoveState(const std::string& stateName)
{
	

	if (mCurrentAnimationState != nullptr && mCurrentAnimationState->GetName() == stateName)
		mCurrentAnimationState = nullptr;








	bool ret = mAnimationStateUnMap.erase(stateName);

	if (ret && mAnimationStateUnMap.size() == 0)
	{
		//더이상 완전하지않은 에니메이션 컴포넌트임으로 . 에니메이션 재생이 되지않는다.
		//에니메이션업데이트시스템에서 이컴포넌트를 소유하는 Object를 제거한다.

		Object* object = static_cast<Object*>(GetDestSceneElement());

		Map* map = object->GetMap();
		map->RemoveObjectInAnimationUpdateSystem(object);
	}

	return ret;
}

bool Quad::AnimationComponent::RegisterStateTransition(const std::string& sourceStateName, const std::string& destStateName)
{

	auto sourceStateIt = mAnimationStateUnMap.find(sourceStateName);
	if (sourceStateIt == mAnimationStateUnMap.end())
		return false;



	auto destStateIt = mAnimationStateUnMap.find(destStateName);
	if (destStateIt == mAnimationStateUnMap.end())
		return false;

	AnimationState* sourceState = sourceStateIt->second;
	AnimationState* destState = destStateIt->second;



	AnimationStateTransition stateTransition;

	stateTransition.mDestState = destStateName;


	sourceState->RegisterStateTransition(std::move(stateTransition));


	return true;
}

void Quad::AnimationComponent::RequestStateTransition(const std::string& destStateName)
{

	if (mCurrentAnimationState == nullptr)
		return;


	auto destStateIt = 	mAnimationStateUnMap.find(destStateName);
	if (destStateIt == mAnimationStateUnMap.end())
		return;


	//이미 상태전이중이라면? 어떻게 할것인지도 생각해야한다.

	

	//mCurrentAnimationState->
	//bool ret =mCurrentAnimationState->GetAnimStateTransition(destStateName, mCurrAnimStateTransition);
	mCurrAnimStateTransition =mCurrentAnimationState->GetAnimationStateTransition(destStateName);
	if (mCurrAnimStateTransition == nullptr)
		return;



	mStateTransitionFlag = true;
	mDestAnimationState = destStateIt->second;
	mDestAnimationState->Start();




}

Quad::Skeleton* Quad::AnimationComponent::GetSkeleton() const
{
	return mSkeleton;
}

void Quad::AnimationComponent::SetCurrentAnimationState(const std::string& stateName)
{

	std::unordered_map<std::string, AnimationState*>::iterator it = mAnimationStateUnMap.find(stateName);

	if (it != mAnimationStateUnMap.end())
	{
		mCurrentAnimationState = it->second;
		SetUpdateFlag(true);
	}

}

Quad::AnimationState* Quad::AnimationComponent::GetCurrentAnimationState() const
{
	return mCurrentAnimationState;
}

bool Quad::AnimationComponent::GetAnimationComponentState() const
{
	if (mSkeleton != nullptr && mCurrentAnimationState != nullptr)
		return true;

	return false;


}

const std::unordered_map<std::string, Quad::AnimationState*>& Quad::AnimationComponent::GetAnimationStateTable() const
{

	return mAnimationStateUnMap;
	// TODO: 여기에 return 문을 삽입합니다.
}

void Quad::AnimationComponent::SetLocalTimeSecondUpdate(float localTime)
{
	if (!GetAnimationComponentState())
		return;

	std::vector<DirectX::XMFLOAT4X4> boneAnimationMatrixVector;
	std::vector<BoneAnimSRT> boneAnimSRTVector;
	mCurrentAnimationState->SetLocalTimeSecondUpdate(localTime, boneAnimSRTVector);
	//mCurrentAnimationState
	CacluateBoneLocalTransformMatrix(boneAnimSRTVector, boneAnimationMatrixVector);

	CacluateFinalTransformMatrix(boneAnimationMatrixVector);



}

void Quad::AnimationComponent::SetLocalTimeFrameUpdate(int frameCount)
{
	if (!GetAnimationComponentState())
		return;
	std::vector<DirectX::XMFLOAT4X4> boneAnimationMatrixVector;
	std::vector<BoneAnimSRT> boneAnimSRTVector;
	mCurrentAnimationState->SetLocalTimeFrameUpdate(frameCount, boneAnimSRTVector);

	CacluateBoneLocalTransformMatrix(boneAnimSRTVector, boneAnimationMatrixVector);



	CacluateFinalTransformMatrix(boneAnimationMatrixVector);


}

void Quad::AnimationComponent::SetUpdateFlag(bool flag)
{
	mUpdateFlag = flag;
}

bool Quad::AnimationComponent::GetUpdateFlag() const
{
	return mUpdateFlag;
}




void Quad::AnimationComponent::Reset()
{
	mSkeleton = nullptr;
	mAnimationStateUnMap.clear();
	mUpdateFlag = true;
	mCurrentAnimationState = nullptr;
}

Quad::AnimationState* Quad::AnimationComponent::GetAnimationState(const std::string& stateName) const
{
	std::unordered_map<std::string, AnimationState*>::const_iterator cit=	mAnimationStateUnMap.find(stateName);
	if (cit == mAnimationStateUnMap.end())
		return nullptr;



	return cit->second;


}





void Quad::AnimationComponent::Serialize(const std::string& tag)
{

	std::string skeletonName = "";
	if (mSkeleton != nullptr)
	{
		skeletonName = mSkeleton->GetName();
	}
	JsonParser::Write("AnimationComponent_SkeletonName", skeletonName);

	size_t animationStateNum =	mAnimationStateUnMap.size();
	JsonParser::Write("AnimationComponent_AnimationStateNum", animationStateNum);

	int tagNum = 0;
	for (auto& animStateElement : mAnimationStateUnMap)
	{
		AnimationState* animState = animStateElement.second;
		animState->Serialize(std::to_string(tagNum));
		tagNum++;
	}


	std::string currAnimationStateName = "";
	if (mCurrentAnimationState != nullptr)
	{
		currAnimationStateName = mCurrentAnimationState->GetName();
		
	}
	JsonParser::Write("AnimationComponent_CurrentAnimationStateName", currAnimationStateName);


}

void Quad::AnimationComponent::DeSerialize(const std::string& tag)
{

	std::string skeletonName = "";
	JsonParser::Read("AnimationComponent_SkeletonName", skeletonName);
	mSkeleton = nullptr;
	if (skeletonName != "")
	{
		SetSkeleton(SkeletonManager::GetSkeleton(skeletonName));
	}

	size_t animationStateNum = 0;
	JsonParser::Read("AnimationComponent_AnimationStateNum", animationStateNum);

	for (int i = 0; i < animationStateNum; ++i)
	{
		AnimationState * animationState = new AnimationState;
		animationState->DeSerialize(std::to_string(i));
		animationState->SetDestAnimationComponent(this);
		

		mAnimationStateUnMap.insert({ animationState->GetName(), animationState });
	}

	std::string currAnimationStateName = "";
	JsonParser::Read("AnimationComponent_CurrentAnimationStateName", currAnimationStateName);
	mCurrentAnimationState = nullptr;
	if (currAnimationStateName != "")
	{
		mCurrentAnimationState = mAnimationStateUnMap[currAnimationStateName];
	}

//	Object* object = static_cast<Object*>(GetDestSceneElement());
	//Map* map = object->GetMap();

	//map->AddObjectToAnimationUpdateSystem()	AnimationUpdateSystem::AddObject(object);
	
}

void Quad::AnimationComponent::CacluateFinalTransformMatrix(const std::vector<DirectX::XMFLOAT4X4>& boneAnimationMatrixVector)
{

	const std::vector<DirectX::XMFLOAT4X4> & offsetMatrixVector = mSkeleton->GetOffsetMatrixVector();

	//그냥곱해주면끝
	for (int i = 0; i < offsetMatrixVector.size(); ++i)
	{
		const DirectX::XMFLOAT4X4& currentBoneOffsetMatrix = offsetMatrixVector[i];
		const DirectX::XMFLOAT4X4& currentBoneTransformMatrix = boneAnimationMatrixVector[i];
		
		DirectX::XMFLOAT4X4 & currentBoneFinalMatrix = mFinalMatrixVector[i];
		DirectX::XMStoreFloat4x4(&currentBoneFinalMatrix, DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&currentBoneOffsetMatrix), DirectX::XMLoadFloat4x4(&currentBoneTransformMatrix))));


		DirectX::XMStoreFloat4x4(&mFinalInverseTransposeMatrixVector[i], DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&currentBoneFinalMatrix))));
	}




}

void Quad::AnimationComponent::CacluateBoneLocalTransformMatrix(std::vector<BoneAnimSRT>& boneSRTVector,std::vector<DirectX::XMFLOAT4X4> & oBoneAnimationMatrixVector)
{

	oBoneAnimationMatrixVector.resize(boneSRTVector.size());



	DirectX::XMVECTOR scale;
	DirectX::XMVECTOR rotation;
	DirectX::XMVECTOR translation;

	DirectX::XMVECTOR zero = DirectX::XMVectorSet(0, 0, 0, 1.0);
	for (int i = 0; i < boneSRTVector.size(); ++i)
	{

		scale = DirectX::XMLoadFloat3(&boneSRTVector[i].mScale);
		rotation = DirectX::XMLoadFloat4(&boneSRTVector[i].mRotation);
		translation = DirectX::XMLoadFloat3(&boneSRTVector[i].mTranslation);
		DirectX::XMMATRIX matrix = DirectX::XMMatrixAffineTransformation(scale, zero, rotation, translation);
		DirectX::XMStoreFloat4x4(&oBoneAnimationMatrixVector[i], matrix);
	}





}

void Quad::AnimationComponent::CrossFading(const std::vector<BoneAnimSRT>& sourceBoneSRTVector, const std::vector<BoneAnimSRT>& destBoneSRTVector, std::vector<BoneAnimSRT>& oBoneSRTVector)
{

	oBoneSRTVector.resize(sourceBoneSRTVector.size());

	

	float durationTime =	mCurrentAnimationState->GetAnimationStateTransition(mDestAnimationState->GetName())->mTransitionDuration;
		


	//float blendFactor = mCurrAnimStateTransitionTime / mCurrAnimStateTransition->mTransitionDuration;
	float blendFactor = mCurrAnimStateTransitionTime / durationTime;

	DirectX::XMVECTOR sourceScale, destScale,sourceRotation,destRotation,sourceTranslation,destTranslation;
	DirectX::XMVECTOR oScale, oRotation, oTranslation;

	for (int i = 0; i < sourceBoneSRTVector.size(); ++i)
	{

		//S
		sourceScale = DirectX::XMLoadFloat3(&sourceBoneSRTVector[i].mScale);
		destScale = DirectX::XMLoadFloat3(&destBoneSRTVector[i].mScale);

		oScale = DirectX::XMVectorLerp(sourceScale, destScale, blendFactor);


		//R
		sourceRotation = DirectX::XMLoadFloat4(&sourceBoneSRTVector[i].mRotation);
		destRotation = DirectX::XMLoadFloat4(&destBoneSRTVector[i].mRotation);

		oRotation = DirectX::XMQuaternionSlerp(sourceRotation, destRotation, blendFactor);
		


		//T
		sourceTranslation = DirectX::XMLoadFloat3(&sourceBoneSRTVector[i].mTranslation);
		destTranslation = DirectX::XMLoadFloat3(&destBoneSRTVector[i].mTranslation);

		oTranslation = DirectX::XMVectorLerp(sourceTranslation, destTranslation, blendFactor);



		DirectX::XMStoreFloat3(&oBoneSRTVector[i].mScale, oScale);
		DirectX::XMStoreFloat4(&oBoneSRTVector[i].mRotation, oRotation);
		DirectX::XMStoreFloat3(&oBoneSRTVector[i].mTranslation, oTranslation);

	}













}

void Quad::AnimationComponent::EndAnimStateTransition()
{

	mStateTransitionFlag = false;
	mCurrAnimStateTransitionTime = 0.0f;
	mCurrentAnimationState = mDestAnimationState;
	mDestAnimationState = nullptr;

}

Quad::AnimationState::AnimationState(const std::string& name)
	:mName(name)
{
}

void Quad::AnimationState::Start()
{
	mLocalCurrentTime = mLocalStartTime;


}

void Quad::AnimationState::Update(float deltaTime , std::vector<BoneAnimSRT>& oBoneAnimSRTVector)
{




	mLocalCurrentTime += deltaTime;
	if (mLocalCurrentTime > mLocalEndTime)
		mLocalCurrentTime = mLocalStartTime;




	Skeleton * skeleton =  GetDestAnimationComponent()->GetSkeleton();
	mAnimationClip->Update(mLocalCurrentTime, skeleton->GetBoneHiererachyVector(), oBoneAnimSRTVector);



}

void Quad::AnimationState::SetAnimationClip(AnimationClip* animationClip)
{
	
	mAnimationClip = animationClip;

	mLocalStartTime = 0.0f;
	mLocalEndTime = mAnimationClip->GetEndTimeSecond();

}

void Quad::AnimationState::SetName(const std::string& name)
{
	mName = name;
}

const std::string& Quad::AnimationState::GetName() const
{
	return mName;
	// TODO: 여기에 return 문을 삽입합니다.
}

void Quad::AnimationState::SetDestAnimationComponent(AnimationComponent* component)
{
	mDestAnimationComponent = component;
}

Quad::AnimationComponent* Quad::AnimationState::GetDestAnimationComponent() const
{
	return mDestAnimationComponent;
}

Quad::AnimationClip* Quad::AnimationState::GetAnimationClip() const
{
	return mAnimationClip;
}

void Quad::AnimationState::SetLocalTimeSecondUpdate(float localTime, std::vector<BoneAnimSRT>& oBoneAnimSRTVector)
{
	

	mLocalCurrentTime = localTime;
	if (mLocalCurrentTime > mLocalEndTime)
		mLocalCurrentTime = mLocalEndTime;


	Skeleton* skeleton = GetDestAnimationComponent()->GetSkeleton();
	mAnimationClip->Update(mLocalCurrentTime, skeleton->GetBoneHiererachyVector(), oBoneAnimSRTVector);



}

void Quad::AnimationState::SetLocalTimeFrameUpdate(int frameCount, std::vector<BoneAnimSRT>& oBoneAnimSRTVector)
{

	float localTimeSecond = 0.0f;
	switch (mAnimationClip->GetTimeMode())
	{
	case EAnimationTimeMode::eFPS30:


		localTimeSecond = (float)frameCount / 30;

		break;
	case EAnimationTimeMode::eFPS60:
		localTimeSecond = (float)frameCount / 60;
		break;
	}

	SetLocalTimeSecondUpdate(localTimeSecond, oBoneAnimSRTVector);



}

void Quad::AnimationState::SetLocalStartTime(float startTime)
{
	if (startTime < 0)
		startTime = 0;

	if (startTime > GetAnimationClip()->GetEndTimeSecond())
		startTime = 0;


	mLocalStartTime = startTime;



}

void Quad::AnimationState::SetLocalEndTime(float endTime)
{
	float clipLocalEndTime = GetAnimationClip()->GetEndTimeSecond();
	if (endTime < 0)
		endTime = clipLocalEndTime;

	if (endTime > clipLocalEndTime  || endTime< mLocalStartTime)
		endTime = clipLocalEndTime;


	mLocalEndTime = endTime;



}

float Quad::AnimationState::GetLocalStartTime() const
{
	return mLocalStartTime;
}

float Quad::AnimationState::GetLocalEndTime() const
{
	return mLocalEndTime;
}

void Quad::AnimationState::Serialize(const std::string& tag)
{
	//animationState Name 

	//animationClip name 


	//local time
	//end time


	JsonParser::Write("AnimationState_Name"+ tag, mName);
	JsonParser::Write("AnimationState_ClipName"+ tag, mAnimationClip->GetName());
	JsonParser::Write("AnimationState_LocalStartTime"+ tag, mLocalStartTime);
	JsonParser::Write("AnimationState_LocalEndTime"+ tag, mLocalEndTime);


	JsonParser::Write("AnimationState_TransitionNum"+ tag, mStateTransitionVector.size());

	for (int i = 0; i < mStateTransitionVector.size(); ++i)
	{
		std::string tag = mName + std::to_string(i);
		mStateTransitionVector[i].Serialize(tag);
	}





}

void Quad::AnimationState::DeSerialize(const std::string& tag)
{
	JsonParser::Read("AnimationState_Name"+ tag, mName);
	std::string animationClipName = "";
	JsonParser::Read("AnimationState_ClipName"+ tag, animationClipName);

	mAnimationClip= AnimationClipManager::GetAnimationClip(animationClipName);


	JsonParser::Read("AnimationState_LocalStartTime"+ tag, mLocalStartTime);
	JsonParser::Read("AnimationState_LocalEndTime"+ tag, mLocalEndTime);


	size_t transitionNum = 0;
	JsonParser::Read("AnimationState_TransitionNum"+ tag, transitionNum);
	mStateTransitionVector.resize(transitionNum);

	for (int i = 0; i < mStateTransitionVector.size(); ++i)
	{
		std::string tag = mName + std::to_string(i);
		mStateTransitionVector[i].DeSerialize(tag);
	}



}

void Quad::AnimationState::RegisterStateTransition(const AnimationStateTransition& transition)
{

	auto it =std::find(mStateTransitionVector.begin(), mStateTransitionVector.end(), transition);
	if (it != mStateTransitionVector.end())
	{
		*it = transition;
		return;
	}
	
	mStateTransitionVector.push_back(transition);




}

void Quad::AnimationState::RegisterStateTransition(AnimationStateTransition&& transition)
{

	auto it = std::find(mStateTransitionVector.begin(), mStateTransitionVector.end(), transition);
	if (it != mStateTransitionVector.end())
	{
		*it = std::move(transition);
		return;
	}

	mStateTransitionVector.push_back(std::move(transition));

}

bool Quad::AnimationState::GetAnimStateTransition(const std::string destStateName, AnimationStateTransition& oAnimStateTransition)
{

	auto it = std::find_if(mStateTransitionVector.begin(), mStateTransitionVector.end(), [destStateName](const AnimationStateTransition & stateTransition) {

		if (stateTransition.mDestState == destStateName)
			return true;

		return false;
	
		});




	if (it == mStateTransitionVector.end())
		return false;


	oAnimStateTransition = *it;

	return true;



}

Quad::AnimationStateTransition* Quad::AnimationState::GetAnimationStateTransition(const std::string & destStateName)
{

	auto it = std::find_if(mStateTransitionVector.begin(), mStateTransitionVector.end(), [destStateName](const AnimationStateTransition& stateTransition) {

		if (stateTransition.mDestState == destStateName)
			return true;

		return false;

		});

	AnimationStateTransition* stateTransition = &(*it);

	return stateTransition;


}



void Quad::AnimationStateTransition::Serialize(const std::string& tag)
{

	JsonParser::Write("AnimationStateTransition_DestStateName" + tag, mDestState);
	JsonParser::Write("AnimationStateTransition_TransitionDurationTime" + tag, mTransitionDuration);
}

void Quad::AnimationStateTransition::DeSerialize(const std::string& tag)
{

	JsonParser::Read("AnimationStateTransition_DestStateName" + tag, mDestState);
	JsonParser::Read("AnimationStateTransition_TransitionDurationTime" + tag, mTransitionDuration);

}
