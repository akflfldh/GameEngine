#include "AnimationStateNode.h"
#include"Map/Map.h"

#include"Core/Event.h"

#include"Core/MouseEvent.h"
#include"AnimationStateLinkRectNode.h"
#include"AnimationStateTransitionStartNode.h"
#include"AnimationStateTransitionEndNode.h"
#include"Collision/CollisionHelper.h"
#include"Component/UiColliderComponent.h"
#include"TextBoxPanel.h"


#include"Parser/JsonParser.h"
#include"Object/Line/Spline.h"
#include"BezierCurve.h"

Quad::AnimationStateNode::AnimationStateNode(const std::string& name)
	:AnimationStateTransitionBaseNode(EAnimationNodeType::eStateNode,name), mLinkNodeVector(2,nullptr), mLinkStartNode(nullptr),mLinkEndNode(nullptr)
{
}

Quad::AnimationStateNode::~AnimationStateNode()
{


}

void Quad::AnimationStateNode::Initialize()
{
	AnimationStateTransitionBaseNode::Initialize();
}

Quad::AnimationStateNode* Quad::AnimationStateNode::Create(Map* map, int mapLayerIndex)
{
	AnimationStateNode* ob = static_cast<AnimationStateNode*>(map->CreateObject("AnimationStateNode", mapLayerIndex));

	ob->InitCreating();

	return ob;

}

void Quad::AnimationStateNode::Update(float deltaTime)
{
	AnimationStateTransitionBaseNode::Update(deltaTime);

}

void Quad::AnimationStateNode::OnEvent(Event* event)
{
	AnimationStateTransitionBaseNode::OnEvent(event);


	std::string eventName = event->GetName();


	//if (eventName == "LButtonDown")
	//{


	//}
	//else if (eventName == "LButtonUp")
	//{



	//}
	//else if (eventName == "Hover")
	//{


	//}
	//else if (eventName == "MouseMove")
	//{



	//}
	//






}

void Quad::AnimationStateNode::SetSize(float width, float height)
{

	AnimationStateTransitionBaseNode::SetSize(width, height);

	float linkRectNodeWidth = mLinkStartNode->GetWidth();
	//mLinkNodeVector[0]->SetPosition({ -width/2 +linkRectNodeWidth,0,-1 });
	//mLinkNodeVector[1]->SetPosition({  width / 2 -linkRectNodeWidth,0,-1 });

	mLinkStartNode->SetPosition({ width / 2 - linkRectNodeWidth,0,-1 });
	mLinkEndNode->SetPosition({ -width / 2 + linkRectNodeWidth,0,-1 });
	mStateNameTextBoxPanel->SetSize(width, mStateNameTextBoxPanel->GetHeight());
	mStateNameTextBoxPanel->SetPosition({ 0,height / 2 - mStateNameTextBoxPanel->GetHeight() / 2 , - 1 });
}

void Quad::AnimationStateNode::SetStateName(const std::string& name)
{

	mStateName = name;
	mStateNameTextBoxPanel->SetText(mStateName);
}

const std::string& Quad::AnimationStateNode::GetStateName() const
{
	return mStateName;

	// TODO: 여기에 return 문을 삽입합니다.
}

bool Quad::AnimationStateNode::RayCastingLinkEndRectNode(Ray& ray)
{
	float t = FLT_MAX;
	return 	CollisionHelper::Intersect(mLinkEndNode->GetModel()->GetUiColliderComponent()->GetCollider(), ray, t);

}

void Quad::AnimationStateNode::SetLinkEndRectNodeHighLight(const DirectX::XMFLOAT3& colorIntensity)
{
	mLinkEndNode->SetColorItensity(colorIntensity);

}

void Quad::AnimationStateNode::LinkSpline(Spline* spline, bool flagStartEnd)
{
	BezierCurve * curve =static_cast<BezierCurve*>(spline->GetCurve(0));
	if (flagStartEnd)
	{
		//end
		//curve->SetControlPoint(mLinkEndNode->GetTransform().GetPositionWorld(),3);
		mLinkEndNode->AddLinkedSpline(spline);
		

	}else
	{
	//start
	
		//curve->SetControlPoint(mLinkEndNode->GetTransform().GetPositionWorld(),0);
		mLinkStartNode->AddLinkedSpline(spline);

	}






}

void Quad::AnimationStateNode::UpdateSpline()
{
	//mLinkEndNode

	



}

void Quad::AnimationStateNode::SetUpdateLinkedSpline()
{

	mLinkEndNode->SetUpdateSplineFlag();
	mLinkStartNode->SetUpdateSplineFlag();

}


void Quad::AnimationStateNode::Serialize()
{
	AnimationStateTransitionBaseNode::Serialize();

	JsonParser::Write("AnimationStateNode_StateName", mStateName);

	mLinkStartNode->Serialize();
	mLinkEndNode->Serialize();
}

void Quad::AnimationStateNode::DeSerialize()
{
	AnimationStateTransitionBaseNode::DeSerialize();

	JsonParser::Read("AnimationStateNode_StateName", mStateName);
	mStateNameTextBoxPanel->SetText(mStateName);

	mLinkStartNode->DeSerialize();
	mLinkEndNode->DeSerialize();


}

void Quad::AnimationStateNode::InitCreating()
{
	AnimationStateTransitionBaseNode::InitCreating();

	//std::vector< AnimationStateLinkRectNode*> mLinkNodeVector
	float linkRectNodeWidth = 20;
	float linkRectNodeHeight = 20;
	//for (int i = 0; i < 2; ++i)
	//{
	//	AnimationStateLinkRectNode* linkNode = AnimationStateLinkRectNode::Create(GetMap(), GetMapLayerID());
	//	Transform & linkNodeTransform =	linkNode->GetTransform();
	//	linkNodeTransform.SetIndependentRotationFlag(true);
	//	linkNodeTransform.SetIndependentScaleFlag(true);
	//	//linkNode->SetSelectAvailableFlag(false);
	//	linkNode->SetSize(linkRectNodeWidth, linkRectNodeHeight);
	//	linkNode->SetTexture("DarkGray.png");
	//	AddChildPanelUiEntity(linkNode);

	//	mLinkNodeVector[i] = linkNode;

	//}
	Map * map =GetMap();
	int mapLayerIndex = GetMapLayerID();

	mLinkStartNode = AnimationStateTransitionStartNode::Create(map, mapLayerIndex);
	Transform& startNodeTransform = mLinkStartNode->GetTransform();
	startNodeTransform.SetIndependentRotationFlag(true);
	startNodeTransform.SetIndependentScaleFlag(true);
	//linkNode->SetSelectAvailableFlag(false);
	mLinkStartNode->SetSize(linkRectNodeWidth, linkRectNodeHeight);
//	mLinkStartNode->SetTexture("Green.png");
	AddChildPanelUiEntity(mLinkStartNode);




	mLinkEndNode = AnimationStateTransitionEndNode::Create(map, mapLayerIndex);
	Transform& endNodeTransform = mLinkEndNode->GetTransform();
	endNodeTransform.SetIndependentRotationFlag(true);
	endNodeTransform.SetIndependentScaleFlag(true);
	//linkNode->SetSelectAvailableFlag(false);
	mLinkEndNode->SetSize(linkRectNodeWidth, linkRectNodeHeight);
	//mLinkEndNode->SetTexture("Blue.png");
	AddChildPanelUiEntity(mLinkEndNode);


	float stateNameTextBoxPanelWidth = 100;
	float stateNameTextBoxPanelHeight = 30;
	mStateNameTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, stateNameTextBoxPanelWidth, stateNameTextBoxPanelHeight);
	Transform & stateNameTextBoxPanel =	mStateNameTextBoxPanel->GetTransform();
	stateNameTextBoxPanel.SetIndependentScaleFlag(true);
	stateNameTextBoxPanel.SetIndependentRotationFlag(true);
	mStateNameTextBoxPanel->SetSelectAvailableFlag(false);
	mStateNameTextBoxPanel->SetTextColor({ 0,0,0 });
	mStateNameTextBoxPanel->SetTexture("324a4f.png");
//	mStateNameTextBoxPanel->SetTextAlignmentDirection(ETextAlignmentDirection::eCenter);
	AddChildPanelUiEntity(mStateNameTextBoxPanel);




	float stateNodeWidth = GetWidth();
	float stateNodeHeight = GetHeight();
//	mLinkNodeVector[0]->SetPosition({ -stateNodeWidth/2 + linkRectNodeWidth ,0,-1});
	//mLinkNodeVector[1]->SetPosition({ stateNodeWidth/2 - linkRectNodeWidth,0,-1});

	mLinkEndNode->SetPosition({ -stateNodeWidth / 2 + linkRectNodeWidth ,0,-1 });
	mLinkStartNode->SetPosition({ stateNodeWidth / 2 - linkRectNodeWidth,0,-1 });
	mStateNameTextBoxPanel->SetPosition({ 0,stateNodeHeight / 2 - stateNameTextBoxPanelHeight / 2 ,-1 });




}

void Quad::AnimationStateNode::HandleLButtonDown(Event* pEvent)
{
	AnimationStateTransitionBaseNode::HandleLButtonDown(pEvent);





}

void Quad::AnimationStateNode::HandleLButtonUp(Event* pEvent)
{
	AnimationStateTransitionBaseNode::HandleLButtonUp(pEvent);



}

void Quad::AnimationStateNode::SetPosition(const DirectX::XMFLOAT3& posL)
{
	AnimationStateTransitionBaseNode::SetPosition(posL);


	mLinkStartNode->SetUpdateSplineFlag();
	mLinkEndNode->SetUpdateSplineFlag();

}

void Quad::AnimationStateNode::SetPosition(float x, float y, float z)
{
	AnimationStateTransitionBaseNode::SetPosition(x, y, z);

	mLinkStartNode->SetUpdateSplineFlag();
	mLinkEndNode->SetUpdateSplineFlag();


}


void Quad::AnimationStateNode::HandleMouseMove(Event* pEvent)
{
	AnimationStateTransitionBaseNode::HandleMouseMove(pEvent);
	//StateComponent* stateComponent = GetStateComponent();
	//bool moveFlag = true;
	//for (auto uiEntity : GetChildPanelUiEntityVector())
	//{
	//	if (uiEntity->GetStateComponent()->GetLButtonDownState())
	//	{
	//		//자식이 클릭되었다면 움직이는게 아니다.
	//		moveFlag = false;
	//		break;
	//	}
	//}



	//if (moveFlag && stateComponent->GetLButtonDownState() )
	//{
	//	MouseEvent* mouseEvent = static_cast<MouseEvent*>(pEvent);

	//	float shiftX = mouseEvent->GetClientPositionX() - mouseEvent->GetPreClientPositionX();
	//	float shiftY = mouseEvent->GetClientPositionY() - mouseEvent->GetPreClientPositionY();

	//	DirectX::XMFLOAT3 posL =GetTransform().GetPositionLocal();
	//	posL.x += shiftX;
	//	posL.y -= shiftY;

	//	SetPosition(posL);
	//}
	//else 
	//{
	//	PanelUiEntity::HandleMouseMove(pEvent);
	//}


}

void Quad::AnimationStateNode::HandleHover(Event* pEvent)
{

	AnimationStateTransitionBaseNode::HandleHover(pEvent);




}

