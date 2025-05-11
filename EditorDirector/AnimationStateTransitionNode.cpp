#include "AnimationStateTransitionNode.h"
#include"Map/Map.h"
#include"MouseEvent.h"
#include"Camera/CameraHelper.h"
#include<sstream>
#include"Mouse.h"
Quad::AnimationStateTransitionNode::AnimationStateTransitionNode(const std::string& name)
	:PanelUiEntity(name), mIsCreating(true)
{
}

Quad::AnimationStateTransitionNode::~AnimationStateTransitionNode()
{
}

void Quad::AnimationStateTransitionNode::Initialize()
{
	PanelUiEntity::Initialize();


}

Quad::AnimationStateTransitionNode* Quad::AnimationStateTransitionNode::Create(Map* map, int mapLayerIndex)
{
	AnimationStateTransitionNode* ob = static_cast<AnimationStateTransitionNode*>(map->CreateObject("AnimationStateTransitionNode", mapLayerIndex));

	ob->InitCreating();

	return ob;
}

void Quad::AnimationStateTransitionNode::Update(float deltaTime)
{
	PanelUiEntity::Update(deltaTime);




}

void Quad::AnimationStateTransitionNode::OnEvent(Event* event)
{
	PanelUiEntity::OnEvent(event);




}

void Quad::AnimationStateTransitionNode::SetIsCreatingFlag(bool flag)
{
	mIsCreating = flag;
}

void Quad::AnimationStateTransitionNode::SetStartLinkNodePosWorld(const DirectX::XMFLOAT3& startPosW)
{
	mStartPosW = startPosW;
}

void Quad::AnimationStateTransitionNode::InitCreating()
{
	PanelUiEntity::InitCreating();
	




}

void Quad::AnimationStateTransitionNode::HandleMouseMove(Event* pEvent)
{

	
	//if (mIsCreating)
	//{

	//	MouseEvent* mouseEvent = static_cast<MouseEvent*>(pEvent);
	//	float clientPosX = mouseEvent->GetClientPositionX();
	//	float clientPosY = mouseEvent->GetClientPositionY();
	//	DirectX::XMFLOAT3  oMousePosW;
	//	/*CameraHelper::CalculateScreenToViewPos({ clientPosX,clientPosY }, GetMap()->GetViewPort(), *GetMap()->GetMainCamera(), true, oMousePosW);*/




	//	Ray ray;
	//	CameraHelper::CalculateRay({ clientPosX,clientPosY }, GetMap()->GetViewPort(), *GetMap()->GetMainCamera(), true, ray);

	//	oMousePosW.x = ray.GetOrigin().x;
	//	oMousePosW.y = ray.GetOrigin().y;
	//	oMousePosW.z = 0;


	//	DirectX::XMFLOAT3 linkNodePosW = mStartPosW;
	//	linkNodePosW.z = 0;
	//	DirectX::XMVECTOR dir = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&oMousePosW), DirectX::XMLoadFloat3(&linkNodePosW));

	//	DirectX::XMFLOAT3 dirF;
	//	DirectX::XMStoreFloat3(&dirF, dir);


	//	DirectX::XMFLOAT3  lengthF;
	//	DirectX::XMStoreFloat3(&lengthF, DirectX::XMVector3Length(dir));

	//	DirectX::XMVECTOR dirNormal = DirectX::XMVector3Normalize(dir);

	//	DirectX::XMFLOAT3 dirNormalF;
	//	DirectX::XMStoreFloat3(&dirNormalF, dirNormal);


	//	DirectX::XMFLOAT3 cosTheta;
	//	DirectX::XMStoreFloat3(&cosTheta, DirectX::XMVector3Dot(dirNormal, DirectX::XMVectorSet(1, 0, 0, 0)));

	//	float degree = atan2(dirNormalF.y, dirNormalF.x) * (180.0f / DirectX::XM_PI);	

	//	SetSize(lengthF.x, 20);
	//	GetTransform().SetRotationWorld({ 0,0,degree });

	//	DirectX::XMFLOAT3 posW = mStartPosW;


	//	DirectX::XMStoreFloat3(&posW, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&posW), DirectX::XMVectorScale(dir, 0.5f)));

	//	posW.z -= 1;
	//	GetTransform().SetPositionWorld(posW);

	//}
}

void Quad::AnimationStateTransitionNode::HandleLButtonUp(Event* pEvent)
{

	//if (mIsCreating)
	//{
	//	Mouse::ReleaseCaptureObject(this);
	//	SetIsCreatingFlag(false);
	//}

}

