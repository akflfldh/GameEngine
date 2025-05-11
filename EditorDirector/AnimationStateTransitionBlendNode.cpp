#include "AnimationStateTransitionBlendNode.h"
#include"Map/Map.h"


#include"AnimationStateTransitionStartNode.h"
#include"AnimationStateTransitionEndNode.h"
#include"Collision/CollisionHelper.h"

#include"Component/UiColliderComponent.h"

#include"AnimationStateTransitionGraph.h"
#include"AnimationStateTransitionGraphBasePanel.h"

#include"AnimationStateNode.h"
#include"AnimationStateTransitionBlendNodeEditPanel.h"


#include"RegisterAnimStateTransitionCallbackClass.h"
#include"RegisterAnimStateTransitionCallbackClassManager.h"

#include"Parser/JsonParser.h"

#include"Object/Line/Spline.h"
#include"BezierCurve.h"
Quad::AnimationStateTransitionBlendNode::AnimationStateTransitionBlendNode(const std::string& name)
	:AnimationStateTransitionBaseNode(EAnimationNodeType::eBlendNode,name), mLinkStartNode(nullptr),mLinkEndNode(nullptr)
	, mSourceStateNode(nullptr), mDestStateNode(nullptr), mTransitionDuration(0.0f), mInputDevice(0), mInputState(0),mInputKeyValue(0)
{


}

Quad::AnimationStateTransitionBlendNode::~AnimationStateTransitionBlendNode()
{


}

void Quad::AnimationStateTransitionBlendNode::Initialize()
{
	AnimationStateTransitionBaseNode::Initialize();
}

Quad::AnimationStateTransitionBlendNode* Quad::AnimationStateTransitionBlendNode::Create(Map* map, int mapLayerIndex)
{
	AnimationStateTransitionBlendNode* ob = static_cast<AnimationStateTransitionBlendNode*>(map->CreateObject("AnimationStateTransitionBlendNode", mapLayerIndex));

	ob->InitCreating();


	return ob;
}

void Quad::AnimationStateTransitionBlendNode::Update(float deltaTime)
{
	AnimationStateTransitionBaseNode::Update(deltaTime);

}

void Quad::AnimationStateTransitionBlendNode::OnEvent(Event* event)
{
	AnimationStateTransitionBaseNode::OnEvent(event);
}

void Quad::AnimationStateTransitionBlendNode::SetSize(float width, float height)
{
	AnimationStateTransitionBaseNode::SetSize(width, height);

	float linkNodeWidth = mLinkStartNode->GetWidth();

	mLinkStartNode->SetPosition({ width / 2 - linkNodeWidth ,0, -1 });
	mLinkEndNode->SetPosition({ -width / 2 + linkNodeWidth ,0, -1 });


}

bool Quad::AnimationStateTransitionBlendNode::RayCastingLinkEndRectNode(Ray& ray)
{

	float t = FLT_MAX;
	return 	CollisionHelper::Intersect(mLinkEndNode->GetModel()->GetUiColliderComponent()->GetCollider(), ray, t);

}

void Quad::AnimationStateTransitionBlendNode::SetLinkEndRectNodeHighLight(const DirectX::XMFLOAT3& colorIntensity)
{
	mLinkEndNode->SetColorItensity(colorIntensity);

}

bool Quad::AnimationStateTransitionBlendNode::SetSourceAnimationStateNode(AnimationStateNode* animStateNode)
{

	if (mSourceStateNode || mDestStateNode == animStateNode)
		return false;

	mSourceStateNode = animStateNode;

	if (mDestStateNode != nullptr || mSourceStateNode != animStateNode)
	{
		CreateAnimaitonBlend();
	}

	AnimationStateTransitionBlendNodeEditPanel::NotifyStateNodeConnect(this);

	return true;

}

bool Quad::AnimationStateTransitionBlendNode::SetDestAnimationStateNode(AnimationStateNode* animStateNode)
{
	if (mDestStateNode || mSourceStateNode  ==  animStateNode )
		return false;
	
	
	mDestStateNode = animStateNode;



	if(mSourceStateNode!= nullptr || mDestStateNode != nullptr )
		CreateAnimaitonBlend();


	AnimationStateTransitionBlendNodeEditPanel::NotifyStateNodeConnect(this);

	return true;
}

Quad::AnimationStateNode* Quad::AnimationStateTransitionBlendNode::GetSourceAnimationStateNode() const
{
	return mSourceStateNode;
}

Quad::AnimationStateNode* Quad::AnimationStateTransitionBlendNode::GetDestAnimationStateNode() const
{
	return mDestStateNode;
}

void Quad::AnimationStateTransitionBlendNode::SetTransitionDuration(float t)
{

	mTransitionDuration = t;

	if (mSourceStateNode && mDestStateNode)
	{
		AnimationStateTransitionGraphBasePanel* animStateTransitionGraphBasePanel = AnimationStateTransitionGraphBasePanel::GetInstance();

		Object* destObject = animStateTransitionGraphBasePanel->GetDestObject();
		AnimationComponent* animComponent = destObject->GetModel()->GetAnimationComponent();



		AnimationState * sourceState = animComponent->GetAnimationState(mSourceStateNode->GetStateName());

		
		AnimationStateTransition* animStateTransition = sourceState->GetAnimationStateTransition(mDestStateNode->GetStateName());

		animStateTransition->mTransitionDuration = mTransitionDuration;

	}

}

float Quad::AnimationStateTransitionBlendNode::GetTransitionDuration() const
{
	return mTransitionDuration;
}

void Quad::AnimationStateTransitionBlendNode::SetInputDevice(int index)
{
	mInputDevice = index;
}

int Quad::AnimationStateTransitionBlendNode::GetInputDevice() const
{
	return mInputDevice;
}

void Quad::AnimationStateTransitionBlendNode::SetInputState(int index)
{
	mInputState = index;
}

int Quad::AnimationStateTransitionBlendNode::GetInputState() const
{
	return mInputState;
}

void Quad::AnimationStateTransitionBlendNode::SetInputKeyValue(int value)
{
	mInputKeyValue = value;
}

int Quad::AnimationStateTransitionBlendNode::GetInputKeyValue() const
{
	return mInputKeyValue;
}

void Quad::AnimationStateTransitionBlendNode::SetTransitionCondition(int inputDevice,	unsigned char key, EInputState inputState)
{

	//전이 조건을 생성한다. 또는 수정한다.
	mInputDevice = inputDevice;
	mInputKeyValue = key;
	mInputState = (int)inputState;


	AnimationStateTransitionGraphBasePanel * animationStateTransitionGraphBasePanel =	AnimationStateTransitionGraphBasePanel::GetInstance();


	Object* destObject = animationStateTransitionGraphBasePanel->GetDestObject();
	AnimationComponent * animationComponent =	destObject->GetModel()->GetAnimationComponent();

	AnimationState * sourceState = animationComponent->GetAnimationState(mSourceStateNode->GetStateName());
	AnimationState * destState = animationComponent->GetAnimationState(mDestStateNode->GetStateName());



	AnimationStateTransition animStateTransition;
	sourceState->GetAnimStateTransition(mDestStateNode->GetStateName(), animStateTransition);



	RegisterAnimStateTransitionCallbackClassManager * registerAnimStateTransitionCallbackClassManager =		destObject->GetMap()->GetRegisterAnimStateTransitionCallbackClassManager();


	RegisterAnimStateTransitionCallbackClass * registerAnimStateTransitionCallbackClass= registerAnimStateTransitionCallbackClassManager->GetRegisterAnimStateTransitionCallbackClass(animationComponent);



	if (registerAnimStateTransitionCallbackClass == nullptr)
		registerAnimStateTransitionCallbackClass = registerAnimStateTransitionCallbackClassManager->CreateRegisterAnimStateTransitionCallbackClass(animationComponent);





	registerAnimStateTransitionCallbackClass->RegisterAnimStateTransitionInputCallback(mInputKeyValue, (EInputState)mInputState, sourceState, destState);




}

void Quad::AnimationStateTransitionBlendNode::LinkSpline(Spline* spline, bool flagStartEnd)
{


	BezierCurve* curve = static_cast<BezierCurve*>(spline->GetCurve(0));
	if (flagStartEnd)
	{
		//end
	//	curve->SetControlPoint(mLinkEndNode->GetTransform().GetPositionWorld(), 3);
	//	SetCurveControlPoint(curve);
		mLinkEndNode->AddLinkedSpline(spline);

	}
	else
	{
		//start

		//curve->SetControlPoint(mLinkEndNode->GetTransform().GetPositionWorld(), 0);
		//SetCurveControlPoint(curve);
		mLinkStartNode->AddLinkedSpline(spline);

	}












}

void Quad::AnimationStateTransitionBlendNode::SetUpdateLinkedSpline()
{
	mLinkEndNode->SetUpdateSplineFlag();
	mLinkStartNode->SetUpdateSplineFlag();

}

void Quad::AnimationStateTransitionBlendNode::Serialize()
{
	AnimationStateTransitionBaseNode::Serialize();

	
	unsigned long long sourceStateNodeID = 0;
	if (mSourceStateNode)
		sourceStateNodeID = mSourceStateNode->GetNodeID();

	JsonParser::Write("AnimationStateTransitionBlendNode_SourceStateNodeID", sourceStateNodeID);


	unsigned long long destStateNodeID = 0;
	if (mDestStateNode)
		destStateNodeID = mDestStateNode->GetNodeID();

	JsonParser::Write("AnimationStateTransitionBlendNode_DestStateNodeID", destStateNodeID);

	
	JsonParser::Write("AnimationStateTransitionBlendNode_TransitionDuration", mTransitionDuration);


	JsonParser::Write("AnimationStateTransitionBlendNode_InputDevice", mInputDevice);

	JsonParser::Write("AnimationStateTransitionBlendNode_InputState", mInputState);


	JsonParser::Write("AnimationStateTransitionBlendNode_InputKeyValue", mInputKeyValue);



}

void Quad::AnimationStateTransitionBlendNode::DeSerialize()
{
	AnimationStateTransitionBaseNode::DeSerialize();


	unsigned long long sourceStateNodeID = 0;
	JsonParser::Read("AnimationStateTransitionBlendNode_SourceStateNodeID", sourceStateNodeID);
	if (sourceStateNodeID != 0)
	{
		//획득
		AnimationStateTransitionGraphBasePanel* animStateTransitionGraphBasePanel =	AnimationStateTransitionGraphBasePanel::GetInstance();

		AnimationStateTransitionGraph * graph = animStateTransitionGraphBasePanel->GetCurrentAnimStateTransitionGraph();


		mSourceStateNode = static_cast<AnimationStateNode*>(graph->GetNode(sourceStateNodeID));

		//스플라인 그리기
		Spline* spline = mLinkStartNode->CreateSpline();
		BezierCurve* curve = static_cast<BezierCurve*>(spline->GetCurve(0));


		mSourceStateNode->LinkSpline(spline,false);
		LinkSpline(spline, true);


		mSourceStateNode->SetUpdateLinkedSpline();
		SetUpdateLinkedSpline();
		//mLinkStartNode->SetUpdateSplineFlag();

	}


	unsigned long long destStateNodeID = 0;
	JsonParser::Read("AnimationStateTransitionBlendNode_DestStateNodeID", destStateNodeID);
	if (destStateNodeID != 0)
	{
		//획득
		AnimationStateTransitionGraphBasePanel* animStateTransitionGraphBasePanel = AnimationStateTransitionGraphBasePanel::GetInstance();

		AnimationStateTransitionGraph* graph = animStateTransitionGraphBasePanel->GetCurrentAnimStateTransitionGraph();


		mDestStateNode = static_cast<AnimationStateNode*>(graph->GetNode(destStateNodeID));

		
		//스플라인 그리기
		
		Spline * spline = mLinkStartNode->CreateSpline();
		BezierCurve* curve =static_cast<BezierCurve*>(spline->GetCurve(0));
	

		mDestStateNode->LinkSpline(spline,true);
		LinkSpline(spline, false);

		mDestStateNode->SetUpdateLinkedSpline();
		SetUpdateLinkedSpline();

	}

	JsonParser::Read("AnimationStateTransitionBlendNode_TransitionDuration", mTransitionDuration);
	
	JsonParser::Read("AnimationStateTransitionBlendNode_InputDevice", mInputDevice);

	JsonParser::Read("AnimationStateTransitionBlendNode_InputState", mInputState);


	JsonParser::Read("AnimationStateTransitionBlendNode_InputKeyValue", mInputKeyValue);
	





}

void Quad::AnimationStateTransitionBlendNode::InitCreating()
{
	AnimationStateTransitionBaseNode::InitCreating();


	Map* map = GetMap();
	int mapLayerIndex = GetMapLayerID();
	
	float blendNodeWidth = GetWidth();
	float blendNodeHeight = GetHeight();

	Transform & blendNodeTransform =	GetTransform();

	blendNodeTransform.SetIndependentScaleFlag(true);
	blendNodeTransform.SetIndependentRotationFlag(true);
	SetSelectAvailableFlag(false);

	SetTexture("Green.png");






	mLinkStartNode = AnimationStateTransitionStartNode::Create(map, mapLayerIndex);
	mLinkEndNode = AnimationStateTransitionEndNode::Create(map, mapLayerIndex);


	float linkNodeWidth = mLinkStartNode->GetWidth();
	float linkNodeHeight = mLinkStartNode->GetHeight();


	AddChildPanelUiEntity(mLinkStartNode);
	AddChildPanelUiEntity(mLinkEndNode);

	mLinkStartNode->SetPosition({ blendNodeWidth / 2 - linkNodeWidth ,0, -1 });
	mLinkEndNode->SetPosition({ -blendNodeWidth / 2 + linkNodeWidth ,0, -1 });


}

void Quad::AnimationStateTransitionBlendNode::CreateAnimaitonBlend()
{


	AnimationStateTransitionGraphBasePanel * animStateTransitionGraphBasePanel =  AnimationStateTransitionGraphBasePanel::GetInstance();

	Object * destObject =animStateTransitionGraphBasePanel->GetDestObject();



	AnimationComponent * animComponent = destObject->GetModel()->GetAnimationComponent();
	animComponent->RegisterStateTransition(mSourceStateNode->GetStateName(), mDestStateNode->GetStateName());








}


void Quad::AnimationStateTransitionBlendNode::SetPosition(const DirectX::XMFLOAT3& posL)
{
	AnimationStateTransitionBaseNode::SetPosition(posL);


	mLinkStartNode->SetUpdateSplineFlag();
	mLinkEndNode->SetUpdateSplineFlag();

}

void Quad::AnimationStateTransitionBlendNode::SetPosition(float x, float y, float z)
{
	AnimationStateTransitionBaseNode::SetPosition(x, y, z);

	mLinkStartNode->SetUpdateSplineFlag();
	mLinkEndNode->SetUpdateSplineFlag();


}
