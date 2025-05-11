#include "AnimationStateTransitionEndNode.h"

#include"Map/Map.h"
#include"Object/Line/Spline.h"
#include"BezierCurve.h"

Quad::AnimationStateTransitionEndNode::AnimationStateTransitionEndNode(const std::string& name)
	:PanelUiEntity(name), mSplineUpdateFlag(false)
{
}

Quad::AnimationStateTransitionEndNode::~AnimationStateTransitionEndNode()
{
}

void Quad::AnimationStateTransitionEndNode::Initialize()
{
	PanelUiEntity::Initialize();
}

Quad::AnimationStateTransitionEndNode* Quad::AnimationStateTransitionEndNode::Create(Map* map, int mapLayerIndex)
{
	AnimationStateTransitionEndNode* ob = static_cast<AnimationStateTransitionEndNode*>(map->CreateObject("AnimationStateTransitionEndNode", mapLayerIndex));

	ob->InitCreating();

	return ob;
}

void Quad::AnimationStateTransitionEndNode::Update(float deltaTime)
{

	PanelUiEntity::Update(deltaTime);

}

void Quad::AnimationStateTransitionEndNode::EndUpdate(float deltaTime)
{

	PanelUiEntity::EndUpdate(deltaTime);

	if (mSplineUpdateFlag)
	{
		const DirectX::XMFLOAT3 posW = GetTransform().GetPositionWorld();
		BezierCurve* curve = nullptr;
	//	mLinkedSplineVector[0]->Evaulate();

		for (auto& element : mLinkedSplineVector)
		{
			curve = static_cast<BezierCurve*>(element->GetCurve(0));
			curve->SetControlPoint(posW, 3);


			DirectX::XMFLOAT3 controlPointF[4];
			curve->GetControlPoint(0, controlPointF[0]);
			curve->GetControlPoint(3, controlPointF[3]);

			DirectX::XMFLOAT3 center;
			DirectX::XMStoreFloat3(&center, DirectX::XMVectorScale(DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&controlPointF[0]), DirectX::XMLoadFloat3(&controlPointF[3])), 0.5f));

			center.x = controlPointF[0].x;
			curve->SetControlPoint(center, 1);
			center.x = controlPointF[3].x;
			curve->SetControlPoint(center, 2);

			element->Evaulate();
		}

		mSplineUpdateFlag = false;
	}


}

void Quad::AnimationStateTransitionEndNode::OnEvent(Event* event)
{
	PanelUiEntity::OnEvent(event);

}

void Quad::AnimationStateTransitionEndNode::Serialize()
{


}


void Quad::AnimationStateTransitionEndNode::DeSerialize()
{


}

void Quad::AnimationStateTransitionEndNode::AddLinkedSpline(Spline* spline)
{
	mLinkedSplineVector.push_back(spline);

}

void Quad::AnimationStateTransitionEndNode::SetUpdateSplineFlag()
{
	mSplineUpdateFlag = true;

}

void Quad::AnimationStateTransitionEndNode::InitCreating()
{
	PanelUiEntity::InitCreating();

	Transform & transform =	GetTransform();

	transform.SetIndependentScaleFlag(true);
	transform.SetIndependentRotationFlag(true);
	SetSelectAvailableFlag(false);
	SetSize(20, 20);

	SetTexture("Red.png");
}
