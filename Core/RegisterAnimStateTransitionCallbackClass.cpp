#include "RegisterAnimStateTransitionCallbackClass.h"
#include"InputSystem.h"

#include"Component/AnimationComponent.h"
#include"Parser/JsonParser.h"
#include"Object/Object.h"
#include"ObjectManager/RuntimeGameObjectManager.h"
#include"RegisterAnimStateTransitionCallbackClassManager.h"
#include"Map/Map.h"


Quad::RegisterAnimStateTransitionCallbackClass::RegisterAnimStateTransitionCallbackClass()
{
}

Quad::RegisterAnimStateTransitionCallbackClass::~RegisterAnimStateTransitionCallbackClass()
{


	InputSystem* inputSystem = InputSystem::GetInstance();
	for (const auto& element : mStateTransitionInputContextVector)
	{
		//inputSystem->RegisterKeyBehavior(element.key, element.mInputState, "AnimInputContextCallback");
		inputSystem->ReleaseBehaviorCallbackActionInputContext< RegisterAnimStateTransitionCallbackClass, &RegisterAnimStateTransitionCallbackClass::Callback>("AnimInputContextCallback", element.mInputState, this);
	}


}

void Quad::RegisterAnimStateTransitionCallbackClass::Start()
{

	//input System에 콜백등록
	InputSystem* inputSystem = InputSystem::GetInstance();
	for (const auto& element : mStateTransitionInputContextVector)
	{
		inputSystem->RegisterKeyBehavior(element.key, element.mInputState, "AnimInputContextCallback");
		inputSystem->RegisterBehaviorCallbackActionInputContext< RegisterAnimStateTransitionCallbackClass, &RegisterAnimStateTransitionCallbackClass::Callback>("AnimInputContextCallback", element.mInputState, this);
	}




}

void Quad::RegisterAnimStateTransitionCallbackClass::SetDestRegisterAnimStateTransitionCallbackClassMangaer(RegisterAnimStateTransitionCallbackClassManager* destManager)
{
	mDestManager = destManager;
}

void Quad::RegisterAnimStateTransitionCallbackClass::RegisterAnimStateTransitionInputCallback(int key, EInputState InputState, AnimationState* sourceState, AnimationState* destState)
{

	mStateTransitionInputContextVector.push_back({ key,InputState,sourceState,destState });	
}

void Quad::RegisterAnimStateTransitionCallbackClass::SetAnimaitonComponent(AnimationComponent* animComponent)
{
	mAnimationComponent = animComponent;
}

Quad::AnimationComponent* Quad::RegisterAnimStateTransitionCallbackClass::GetAnimationComponent() const
{
	return mAnimationComponent;
}

void Quad::RegisterAnimStateTransitionCallbackClass::Callback(const InputContext& inpputContext)
{


	AnimationComponent * animComponent = GetAnimationComponent();
	AnimationState * currAnimState= animComponent->GetCurrentAnimationState();
	for (const auto& element : mStateTransitionInputContextVector)
	{
		if (element.key == inpputContext.key && element.mInputState == inpputContext.mState)
		{
			if (currAnimState == element.mSourceState)
			{
				animComponent->RequestStateTransition(element.mDestState->GetName());
				break;
			}
		}

	}

}

void Quad::RegisterAnimStateTransitionCallbackClass::Serialize()
{
	Object* destObject =	static_cast<Object*>(mAnimationComponent->GetDestSceneElement());
	unsigned long long id =	 destObject->GetUniqueID();
	JsonParser::Write("RegsiterAnimStateTransitionCallbackClass_DestObjectUniqueID", id);

	int key = 0;
	EInputState inputState;
	AnimationState* sourceState = nullptr;
	AnimationState* destState = nullptr;

	JsonParser::Write("RegsiterAnimStateTransitionCallbackClass_StateTransitionInputContextVector", mStateTransitionInputContextVector.size());

	for(int i=0; i<mStateTransitionInputContextVector.size(); ++i)
	{ 
		key = mStateTransitionInputContextVector[i].key;
		inputState = mStateTransitionInputContextVector[i].mInputState;
		sourceState = mStateTransitionInputContextVector[i].mSourceState;
		destState = mStateTransitionInputContextVector[i].mDestState;


		std::string tagNum = std::to_string(i);

		JsonParser::Write("RegsiterAnimStateTransitionCallbackClass_key"+ tagNum, key);
		JsonParser::Write("RegsiterAnimStateTransitionCallbackClass_inputState"+ tagNum, (int)inputState);
		JsonParser::Write("RegsiterAnimStateTransitionCallbackClass_sourceStateName"+ tagNum, sourceState->GetName());
		JsonParser::Write("RegsiterAnimStateTransitionCallbackClass_destStateName"+ tagNum, destState->GetName());
	}

}

void Quad::RegisterAnimStateTransitionCallbackClass::DeSerialize()
{


	unsigned long long id = 0;
	JsonParser::Read("RegsiterAnimStateTransitionCallbackClass_DestObjectUniqueID", id);

	Map * map =	mDestManager->GetDestMap();
	

	BaseObjectManager* objectManager = map->GetObjectManager();
	Object * destObject = objectManager->GetObjectFromID(id);
	mAnimationComponent	= destObject->GetModel()->GetAnimationComponent();






	int key = 0;
	EInputState inputState;
	AnimationState* sourceState = nullptr;
	AnimationState* destState = nullptr;

	size_t stateTransitionInputContextVectorSize = 0;
	JsonParser::Read("RegsiterAnimStateTransitionCallbackClass_StateTransitionInputContextVector", stateTransitionInputContextVectorSize);

	mStateTransitionInputContextVector.resize(stateTransitionInputContextVectorSize);

	int inputStateNum = 0;
	std::string stateName;

	for (int i = 0; i < mStateTransitionInputContextVector.size(); ++i)
	{
	//	key = mStateTransitionInputContextVector[i].key;
		//inputState = mStateTransitionInputContextVector[i].mInputState;
	//	sourceState = mStateTransitionInputContextVector[i].mSourceState;
	//	destState = mStateTransitionInputContextVector[i].mDestState;


		std::string tagNum = std::to_string(i);

		JsonParser::Read("RegsiterAnimStateTransitionCallbackClass_key" + tagNum, mStateTransitionInputContextVector[i].key);
		int a;
		JsonParser::Read("RegsiterAnimStateTransitionCallbackClass_inputState" + tagNum, inputStateNum);
		mStateTransitionInputContextVector[i].mInputState = (EInputState)inputStateNum;

		JsonParser::Read("RegsiterAnimStateTransitionCallbackClass_sourceStateName" + tagNum, stateName);
		mStateTransitionInputContextVector[i].mSourceState = mAnimationComponent->GetAnimationState(stateName);

		JsonParser::Read("RegsiterAnimStateTransitionCallbackClass_destStateName" + tagNum, stateName);
		mStateTransitionInputContextVector[i].mDestState = mAnimationComponent->GetAnimationState(stateName);
	}




}
