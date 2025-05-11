#include "AnimationStateTransitionBaseNode.h"
#include"MouseEvent.h"

#include"AnimationStateTransitionNodeEditDetailMainPanel.h"


#include"Parser/JsonParser.h"

#include"BezierCurve.h"

Quad::AnimationStateTransitionBaseNode::AnimationStateTransitionBaseNode(EAnimationNodeType nodeType,const std::string& name)
	:PanelUiEntity(name), mNodeID(0),mAnimationNodeType(nodeType)
{
}

Quad::AnimationStateTransitionBaseNode::~AnimationStateTransitionBaseNode()
{
}

void Quad::AnimationStateTransitionBaseNode::Initialize()
{
	PanelUiEntity::Initialize();

}

void Quad::AnimationStateTransitionBaseNode::Update(float deltaTime)
{
	PanelUiEntity::Update(deltaTime);
}

void Quad::AnimationStateTransitionBaseNode::OnEvent(Event* event)
{
	PanelUiEntity::OnEvent(event);
}

void Quad::AnimationStateTransitionBaseNode::SetCurveControlPoint(Curve* curve)
{
	BezierCurve* bezierCurve = static_cast<BezierCurve*>(curve);

	DirectX::XMFLOAT3 controlPointF[4];
	bezierCurve->GetControlPoint(0, controlPointF[0]);
	bezierCurve->GetControlPoint(0, controlPointF[3]);

	DirectX::XMFLOAT3 center;
	DirectX::XMStoreFloat3(&center, DirectX::XMVectorScale(DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&controlPointF[0]), DirectX::XMLoadFloat3(&controlPointF[3])), 0.5f));

	center.x = controlPointF[0].x;
	bezierCurve->SetControlPoint(center, 1);
	center.x = controlPointF[3].x;
	bezierCurve->SetControlPoint(center, 2);

}

Quad::EAnimationNodeType Quad::AnimationStateTransitionBaseNode::GetAnimationNodeType() const
{
	return mAnimationNodeType;
}



void Quad::AnimationStateTransitionBaseNode::SetNodeID(unsigned long long id)
{
	mNodeID = id;
}

const unsigned long long Quad::AnimationStateTransitionBaseNode::GetNodeID() const
{
	return mNodeID;
}

void Quad::AnimationStateTransitionBaseNode::Serialize()
{
	//위치만저장
	const DirectX::XMFLOAT3 posW = GetTransform().GetPositionWorld();
	JsonParser::Write("AnimationStateTransitionBaseNode_PositionWorld", posW);


}

void Quad::AnimationStateTransitionBaseNode::DeSerialize()
{

	DirectX::XMFLOAT3 posW ;
	JsonParser::Read("AnimationStateTransitionBaseNode_PositionWorld", posW);
	
	GetTransform().SetPositionWorld(posW);


}

void Quad::AnimationStateTransitionBaseNode::HandleMouseMove(Event* pEvent)
{

	StateComponent* stateComponent = GetStateComponent();
	bool moveFlag = true;
	for (auto uiEntity : GetChildPanelUiEntityVector())
	{
		if (uiEntity->GetStateComponent()->GetLButtonDownState())
		{
			//자식이 클릭되었다면 움직이는게 아니다.
			moveFlag = false;
			break;
		}
	}

	if (moveFlag && stateComponent->GetLButtonDownState())
	{
		MouseEvent* mouseEvent = static_cast<MouseEvent*>(pEvent);

		float shiftX = mouseEvent->GetClientPositionX() - mouseEvent->GetPreClientPositionX();
		float shiftY = mouseEvent->GetClientPositionY() - mouseEvent->GetPreClientPositionY();

		DirectX::XMFLOAT3 posL = GetTransform().GetPositionLocal();
		posL.x += shiftX;
		posL.y -= shiftY;

		SetPosition(posL);
	}
	else
	{
		PanelUiEntity::HandleMouseMove(pEvent);
	}




}

void Quad::AnimationStateTransitionBaseNode::HandleLButtonDown(Event* pEvent)
{
	PanelUiEntity::HandleLButtonDown(pEvent);

	StateComponent* stateComponent = GetStateComponent();

	if (stateComponent->GetHoverState())
	{
		//nodeEditDetailMainPanel에게 알린다.
		AnimationStateTransitionNodeEditDetailMainPanel::NotifySelectNode(this);

	}


}
