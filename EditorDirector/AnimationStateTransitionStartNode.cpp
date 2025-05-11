#include "AnimationStateTransitionStartNode.h"
#include"Map/Map.h"

#include"AnimationStateTransitionGraphBasePanel.h"
#include"Object/Line/Spline.h"
#include"Mouse.h"
#include"BezierCurve.h"
#include"MouseEvent.h"
#include"Camera/CameraHelper.h"
#include"CurveFactory.h"
#include"Collision/CollisionHelper.h"
#include"AnimationStateNode.h"
#include"Collision/UiCollider.h"
#include"Component/UiColliderComponent.h"
#include"AnimationStateTransitionGraph.h"
#include"AnimationStateTransitionBaseNode.h"
#include"AnimationStateTransitionBlendNode.h"
#include"Core/System.h"

Quad::AnimationStateTransitionStartNode::AnimationStateTransitionStartNode(const std::string& name)
	:PanelUiEntity(name), mSendLbuttonUpEventFlag(false), mTargetNode(nullptr), mSpline(nullptr), mSplineUpdateFlag(false)
{
}

Quad::AnimationStateTransitionStartNode::~AnimationStateTransitionStartNode()
{
}

void Quad::AnimationStateTransitionStartNode::Initialize()
{
	PanelUiEntity::Initialize();
}

Quad::AnimationStateTransitionStartNode* Quad::AnimationStateTransitionStartNode::Create(Map* map, int mapLayerIndex)
{
	AnimationStateTransitionStartNode* ob = static_cast<AnimationStateTransitionStartNode*>(map->CreateObject("AnimationStateTransitionStartNode", mapLayerIndex));

	ob->InitCreating();

	return ob;
}

void Quad::AnimationStateTransitionStartNode::Update(float deltaTime)
{
	PanelUiEntity::Update(deltaTime);
}

void Quad::AnimationStateTransitionStartNode::EndUpdate(float deltaTime)
{
	if (mSplineUpdateFlag)
	{
		UpdateSpline();
		mSplineUpdateFlag = false;











	}

}

void Quad::AnimationStateTransitionStartNode::OnEvent(Event* event)
{
	PanelUiEntity::OnEvent(event);
}

void Quad::AnimationStateTransitionStartNode::Serialize()
{



}

void Quad::AnimationStateTransitionStartNode::DeSerialize()
{




}

Quad::Spline* Quad::AnimationStateTransitionStartNode::CreateSpline()
{

	DirectX::XMFLOAT3 startPos = GetTransform().GetPositionWorld();
	startPos.z = 4;
	DirectX::XMFLOAT3 endPos = startPos;
	endPos.x += 300;

	//	mLine = LineFactory::CreateLine(startPos, endPos);
	Map* map = GetMap();

	Spline * spline = static_cast<Spline*>(map->CreateObject("Spline", 0));

	BezierCurve* curve = CurveFactory::CreateBezierCurve();


	spline->SetSplineRenderStyle(Quad::Spline::ESplineRenderStyle::eThickLine);
	curve->SetControlPoint(startPos, 0);
	spline->AddCurve(curve);
	spline->SetThickness(7.0f);
	spline->GetModelSubMesh()->mModelMaterial.SetColor({ 0,0,0 });

	return spline;
}

void Quad::AnimationStateTransitionStartNode::AddLinkedSpline(Spline* spline)
{
	mLinkedSplineVector.push_back(spline);
}

void Quad::AnimationStateTransitionStartNode::SetUpdateSplineFlag()
{
	mSplineUpdateFlag = true;

}

void Quad::AnimationStateTransitionStartNode::UpdateSpline()
{

	const DirectX::XMFLOAT3 posW = GetTransform().GetPositionWorld();
	BezierCurve* curve = nullptr;
	for (auto& element : mLinkedSplineVector)
	{
		curve = static_cast<BezierCurve*>(element->GetCurve(0));
		curve->SetControlPoint(posW, 0);

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

}

void Quad::AnimationStateTransitionStartNode::InitCreating()
{
	PanelUiEntity::InitCreating();

	Transform& transform = GetTransform();

	transform.SetIndependentScaleFlag(true);
	transform.SetIndependentRotationFlag(true);
	SetSelectAvailableFlag(false);
	SetSize(20, 20);

	SetTexture("Green.png");
}

void Quad::AnimationStateTransitionStartNode::HandleLButtonDown(Event* pEvent)
{
	PanelUiEntity::HandleLButtonDown(pEvent);


}

void Quad::AnimationStateTransitionStartNode::HandleLButtonUp(Event* pEvent)
{
	if (mSendLbuttonUpEventFlag == true)
		return;

	PanelUiEntity::HandleLButtonUp(pEvent);
	
	
	
	bool removeSplineFlag = true;
	if (mTargetNode)
	{
		
		//targetNode에게 정보를 알려야한다.

		AnimationStateTransitionBaseNode* sourceParentNode = static_cast<AnimationStateTransitionBaseNode*>(GetParentObject());

		EAnimationNodeType targetNodeType =mTargetNode->GetAnimationNodeType();
		EAnimationNodeType sourceParentNodeType = sourceParentNode->GetAnimationNodeType();


		if (targetNodeType == EAnimationNodeType::eStateNode)
		{
			AnimationStateNode* targetStateNode = static_cast<AnimationStateNode*>(mTargetNode);

			if (sourceParentNodeType == EAnimationNodeType::eBlendNode)
			{
				AnimationStateTransitionBlendNode* blendNode = static_cast<AnimationStateTransitionBlendNode*>(sourceParentNode);

				bool ret = blendNode->SetDestAnimationStateNode(targetStateNode);

				if (ret)
				{
					AddLinkedSpline(mSpline);
					targetStateNode->LinkSpline(mSpline,true);
					removeSplineFlag = false;
				}
			}

		}
		else if (targetNodeType == EAnimationNodeType::eBlendNode)
		{
			
			AnimationStateTransitionBlendNode* targetBlendNode = static_cast<AnimationStateTransitionBlendNode*>(mTargetNode);
			if (sourceParentNodeType == EAnimationNodeType::eStateNode)
			{
				AnimationStateNode* sourceStateNode = static_cast<AnimationStateNode*>(sourceParentNode);
				bool ret = targetBlendNode->SetSourceAnimationStateNode(sourceStateNode);
				if (ret)
				{
					AddLinkedSpline(mSpline);
					targetBlendNode->LinkSpline(mSpline,true);
					removeSplineFlag = false;
				}
			}
		}


		mTargetNode = nullptr;


	}
	
	if (mSpline)
	{
		
		if (removeSplineFlag)
		{
			Map* map = GetMap();
			map->RequestRemoveObject(mSpline);

		}
		Mouse::ReleaseCaptureObject(this);
		mSpline = nullptr;

		//lbuttonUp을 이 object만받았기에, 여전히 부모,조상들은 down상태이다. 따라서 system에 lbuttonUp Event를 전달한다.
		//단 무한루프에빠지지 않도록 flag를설정한다.
		if (mSendLbuttonUpEventFlag == false)
		{
			System* system = GetSystem();
			mSendLbuttonUpEventFlag = true;
			system->HandleEvent(pEvent);
		}

	}


}

void Quad::AnimationStateTransitionStartNode::HandleMouseMove(Event* pEvent)
{
	StateComponent* stateComponent = GetStateComponent();

	if (stateComponent->GetLButtonDownState())
	{
		//트랜지션노드생성
		if (mSpline == nullptr)
		{
			//DirectX::XMFLOAT3 startPos = GetTransform().GetPositionWorld();
			//startPos.z = 4;
			//DirectX::XMFLOAT3 endPos = startPos;
			//endPos.x += 300;

			////	mLine = LineFactory::CreateLine(startPos, endPos);
			//Map* map = GetMap();

			//mSpline = static_cast<Spline*>(map->CreateObject("Spline", 0));

			//BezierCurve* curve = CurveFactory::CreateBezierCurve();


			//mSpline->SetSplineRenderStyle(Quad::Spline::ESplineRenderStyle::eThickLine);
			//curve->SetControlPoint(startPos, 0);
			//mSpline->AddCurve(curve);
			//mSpline->SetThickness(7.0f);
			//mSpline->GetModelSubMesh()->mModelMaterial.SetColor({ 0,0,0 });

			mSpline = CreateSpline();

			Mouse::CaptureObject(this);

			mSendLbuttonUpEventFlag = false;

		}


		MouseEvent* mouseEvent = static_cast<MouseEvent*>(pEvent);
		float clientPosX = mouseEvent->GetClientPositionX();
		float clientPosY = mouseEvent->GetClientPositionY();
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

	//	std::stringstream debug1;
	///debug1 << mousePosW.x << " , " << mousePosW.y << std::endl;
	//	OutputDebugStringA(debug1.str().c_str());


		DirectX::XMFLOAT3 innerControlPoint[2];//가운데 중간두개의 컨트롤포인트
		DirectX::XMFLOAT3 center;
		DirectX::XMStoreFloat3(&center, DirectX::XMVectorScale(DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&firstControlPoint), DirectX::XMLoadFloat3(&mousePosW)), 0.5F));

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
		curve->SetControlPoint(innerControlPoint[1], 2);
		//float z=	mLine->GetStartVertexPosition().z;
		mSpline->Evaulate();
		//mLine->SetEndVertexPosition(mousePosW);





		//다른 stateNode들과 충돌하는지 판정한다.
		//
		auto graphBasePanel = AnimationStateTransitionGraphBasePanel::GetInstance();
		auto currGraph = graphBasePanel->GetCurrentAnimStateTransitionGraph();

		//const std::unordered_map<std::string, AnimationStateNode*>& stateNodeTable = currGraph->GetStateNodeTable();
		const std::vector<AnimationStateTransitionBaseNode*>& animStateTransitionNodeVector = currGraph->GetAnimationStateTransitionNodeVector();

		float minT = FLT_MAX;
		mTargetNode = nullptr;
		for (const auto& element : animStateTransitionNodeVector)
		{
			AnimationStateTransitionBaseNode* node = element;

			float t = 0.0f;
			bool ret = CollisionHelper::Intersect(node->GetModel()->GetUiColliderComponent()->GetCollider(), ray, t);
			if (ret && minT > t)
			{
				minT = t;
				mTargetNode = node;
			}
		}

		AnimationStateTransitionBaseNode* sourceParentNode = static_cast<AnimationStateTransitionBaseNode*>(GetParentObject());

		EAnimationNodeType sourceParentNodeType = sourceParentNode->GetAnimationNodeType();

		if (mTargetNode && mTargetNode != GetParentObject() )
		{

			EAnimationNodeType targetNodeType =mTargetNode->GetAnimationNodeType();
			if (targetNodeType == EAnimationNodeType::eStateNode)
			{
				AnimationStateNode* targetStateNode = static_cast<AnimationStateNode*>(mTargetNode);

				if (targetStateNode->RayCastingLinkEndRectNode(ray))
				{
					targetStateNode->SetLinkEndRectNodeHighLight({ 3.0F,3.0F,3.0F });
				}
				else
				{
					targetStateNode->SetLinkEndRectNodeHighLight({ 1.0F,1.0F,1.0F });
					mTargetNode = nullptr;
				}

			}
			else if (targetNodeType == EAnimationNodeType::eBlendNode)
			{
				if (sourceParentNodeType != EAnimationNodeType::eStateNode)
					mTargetNode = nullptr;
				else
				{

					AnimationStateTransitionBlendNode* targetBlendNode = static_cast<AnimationStateTransitionBlendNode*>(mTargetNode);

					if (targetBlendNode->RayCastingLinkEndRectNode(ray))
					{
						targetBlendNode->SetLinkEndRectNodeHighLight({ 3.0F,3.0F,3.0F });
					}
					else
					{
						targetBlendNode->SetLinkEndRectNodeHighLight({ 1.0F,1.0F,1.0F });
						mTargetNode = nullptr;
					}


				}
				
			}
		}
		else
			mTargetNode = nullptr;


	}




}

void Quad::AnimationStateTransitionStartNode::HandleHover(Event* pEvent)
{
	PanelUiEntity::HandleHover(pEvent);

}
