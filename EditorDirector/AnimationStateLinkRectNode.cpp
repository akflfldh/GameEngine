#include "AnimationStateLinkRectNode.h"
#include"Map/Map.h"

#include"AnimationStateTransitionNode.h"
#include"MouseEvent.h"

#include"Camera/CameraHelper.h"
#include"Mouse.h"
#include"Line/Line.h"
#include"ObjectFactory/LineFactory.h"
#include"Object/Line/Spline.h"
#include"Core/CurveFactory.h"
#include"Core/BezierCurve.h"


#include"AnimationStateTransitionGraphBasePanel.h"
#include"AnimationStateTransitionGraph.h"

#include"AnimationStateNode.h"
#include"Collision/CollisionHelper.h"
#include"Component/UiColliderComponent.h"

#include<sstream>

Quad::AnimationStateLinkRectNode::AnimationStateLinkRectNode(const std::string& name)
	:PanelUiEntity(name), mSpline(nullptr)
{
}

Quad::AnimationStateLinkRectNode::~AnimationStateLinkRectNode()
{
}

void Quad::AnimationStateLinkRectNode::Initialize()
{
	PanelUiEntity::Initialize();
}

Quad::AnimationStateLinkRectNode* Quad::AnimationStateLinkRectNode::Create(Map* map, int mapLayerIndex)
{
	AnimationStateLinkRectNode* ob = static_cast<AnimationStateLinkRectNode*>(map->CreateObject("AnimationStateLinkRectNode", mapLayerIndex));

	ob->InitCreating();

	return ob;
}

void Quad::AnimationStateLinkRectNode::Update(float deltaTime)
{
	PanelUiEntity::Update(deltaTime);
}

void Quad::AnimationStateLinkRectNode::OnEvent(Event* event)
{
	PanelUiEntity::OnEvent(event);
}

void Quad::AnimationStateLinkRectNode::SetDrawFlag(bool flag)
{

	//부모인 StateNode에서 벗어나있는부분이있어서 개별적으로 SelectAvailableFlag가 true로 되어있으니
	//보이지않을때는 draw flag와 selectFlag를 같이 처리하자.
	PanelUiEntity::SetDrawFlag(flag);

	SetSelectKeepingFlag(flag);



}

void Quad::AnimationStateLinkRectNode::InitCreating()
{

	PanelUiEntity::InitCreating();






}

void Quad::AnimationStateLinkRectNode::HandleLButtonDown(Event* pEvent)
{
	PanelUiEntity::HandleLButtonDown(pEvent);


}

void Quad::AnimationStateLinkRectNode::HandleLButtonUp(Event* pEvent)
{

	PanelUiEntity::HandleLButtonUp(pEvent);

	if (mTargetNode)
	{

		mSpline = nullptr;
	}
	else
	{

		Map * map =	GetMap();
		map->RequestRemoveObject(mSpline);
		mSpline = nullptr;
	}
	Mouse::ReleaseCaptureObject(this);


}

void Quad::AnimationStateLinkRectNode::HandleMouseMove(Event* pEvent)
{

	StateComponent* stateComponent = GetStateComponent();

	if (stateComponent->GetLButtonDownState())
	{
		//트랜지션노드생성
		if (mSpline == nullptr)
		{
			DirectX::XMFLOAT3 startPos = GetTransform().GetPositionWorld();
			startPos.z = 4;
			DirectX::XMFLOAT3 endPos = startPos;
			endPos.x+=300;
		
		//	mLine = LineFactory::CreateLine(startPos, endPos);
			Map * map = GetMap();
			
			mSpline =static_cast<Spline*>(map->CreateObject("Spline", 0));

			BezierCurve * curve =CurveFactory::CreateBezierCurve();


			mSpline->SetSplineRenderStyle(Quad::Spline::ESplineRenderStyle::eThickLine);
			curve->SetControlPoint(startPos,0);
			mSpline->AddCurve(curve);
			mSpline->SetThickness(7.0f);
			mSpline->GetModelSubMesh()->mModelMaterial.SetColor({ 0,0,0 });
			Mouse::CaptureObject(this);

		}

		
		MouseEvent* mouseEvent = static_cast<MouseEvent*>(pEvent);
		float clientPosX =	mouseEvent->GetClientPositionX();
		float clientPosY =	mouseEvent->GetClientPositionY();
		Map* map = GetMap();
		DirectX::XMFLOAT3 mousePosW;
		Ray ray;
		CameraHelper::CalculateRay({ clientPosX,clientPosY }, map->GetViewPort(), *map->GetMainCamera(), true, ray);

		mousePosW = ray.GetOrigin();
		mousePosW.z = 4;

	
		//mousePosW 가 curve의 endControlPoint;

		BezierCurve* curve = static_cast<BezierCurve*>(mSpline->GetCurve(0));
		DirectX::XMFLOAT3 firstControlPoint;
		curve->GetControlPoint(0, firstControlPoint);

		curve->SetControlPoint(mousePosW, 3);

		std::stringstream debug1;
		debug1 << mousePosW.x << " , " << mousePosW.y << std::endl;
		OutputDebugStringA(debug1.str().c_str());


		DirectX::XMFLOAT3 innerControlPoint[2];//가운데 중간두개의 컨트롤포인트
		DirectX::XMFLOAT3 center;
		DirectX::XMStoreFloat3(	&center,DirectX::XMVectorScale(DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&firstControlPoint), DirectX::XMLoadFloat3(&mousePosW)),0.5F));
				
		innerControlPoint[0] = center;
		innerControlPoint[1] = center;

		if (firstControlPoint.x > mousePosW.x)
		{
			innerControlPoint[0].x = firstControlPoint.x;
			

			innerControlPoint[1].x = mousePosW.x;
		}
		else
		{
			innerControlPoint[0].x = mousePosW.x;
			innerControlPoint[1].x = firstControlPoint.x;
		}
		curve->SetControlPoint(innerControlPoint[0], 1);
		curve->SetControlPoint(innerControlPoint[1],2);
		//float z=	mLine->GetStartVertexPosition().z;
		mSpline->Evaulate();
		//mLine->SetEndVertexPosition(mousePosW);

		



		//다른 stateNode들과 충돌하는지 판정한다.
		//
		auto graphBasePanel =AnimationStateTransitionGraphBasePanel::GetInstance();
		auto currGraph = graphBasePanel->GetCurrentAnimStateTransitionGraph();

		const std::unordered_map<std::string ,AnimationStateNode*> & stateNodeTable = currGraph->GetStateNodeTable();

		float minT = FLT_MAX;
		mTargetNode = nullptr;
		for (const auto & element : stateNodeTable)
		{
			AnimationStateNode* stateNode = element.second;
			
			float t = 0.0f;
			bool ret =CollisionHelper::Intersect(stateNode->GetModel()->GetUiColliderComponent()->GetCollider(), ray, t);
			if (ret  && minT > t )
			{
				minT = t;
				mTargetNode = stateNode;
			}
		}


	}



}

void Quad::AnimationStateLinkRectNode::HandleHover(Event* pEvent)
{
	PanelUiEntity::HandleHover(pEvent);


}
