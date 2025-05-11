#include "AnimationStateTransitionNodeEditDetailMainPanel.h"
#include"Map/Map.h"
#include"Event.h"
#include"WindowResizeEvent.h"
#include"Camera/Camera.h"
#include"TextBoxPanel.h"
#include"AnimationStateTransitionBaseNode.h"
#include"AnimationStateTransitionBlendNode.h"
#include"AnimationStateTransitionBlendNodeEditPanel.h"
#include"EditorCommonHelperClass.h"

Quad::AnimationStateTransitionNodeEditDetailMainPanel::AnimationStateTransitionNodeEditDetailMainPanel(const std::string& name)
	:mSelectedNode(nullptr), mSelectedNodeTypeTextBoxPanel(nullptr), mBlendNodeEditPanel(nullptr)
{



}

Quad::AnimationStateTransitionNodeEditDetailMainPanel::~AnimationStateTransitionNodeEditDetailMainPanel()
{


}

void Quad::AnimationStateTransitionNodeEditDetailMainPanel::Initialize()
{
	PanelUiEntity::Initialize();

	RegisterAcceptEvent("WindowResizeEvent");

}

Quad::AnimationStateTransitionNodeEditDetailMainPanel* Quad::AnimationStateTransitionNodeEditDetailMainPanel::Create(Map* map, int mapLayerIndex)
{
	AnimationStateTransitionNodeEditDetailMainPanel* ob = static_cast<AnimationStateTransitionNodeEditDetailMainPanel*>(map->CreateObject("AnimationStateTransitionNodeEditDetailMainPanel", mapLayerIndex));

	ob->InitCreating();

	return ob;
}

void Quad::AnimationStateTransitionNodeEditDetailMainPanel::NotifySelectNode(AnimationStateTransitionBaseNode* node)
{
	auto instance = GetInstance();

	if (instance->mSelectedNode == node)
		return;

	if (instance->mSelectedNode)
	{
		EAnimationNodeType preSelectedNodeType = instance->mSelectedNode->GetAnimationNodeType();
		instance->OffPreSelectedNodeEditPanel(preSelectedNodeType);
	}


	if (node == nullptr)
	{

	}
	else
	{

		switch (node->GetAnimationNodeType())
		{
		case EAnimationNodeType::eStateNode:
			instance->mSelectedNodeTypeTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"상태노드"));
				break;
		case EAnimationNodeType::eBlendNode: {
			AnimationStateTransitionBlendNode* blendNode = static_cast<AnimationStateTransitionBlendNode*>(node);
			instance->mBlendNodeEditPanel->SetDestBlendNode(blendNode);
			instance->mBlendNodeEditPanel->SetDrawFlag(true);

			
			instance->mSelectedNodeTypeTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"블랜드노드"));
		}
			break;

		}

	}


	instance->mSelectedNode = node;

}

void Quad::AnimationStateTransitionNodeEditDetailMainPanel::Update(float deltaTime)
{

	PanelUiEntity::Update(deltaTime);
}

void Quad::AnimationStateTransitionNodeEditDetailMainPanel::OnEvent(Event* event)
{
	PanelUiEntity::OnEvent(event);
	const std::string eventName = event->GetName();

	if (eventName == "WindowResizeEvent")
		HandleWindowResize(event);

}

void Quad::AnimationStateTransitionNodeEditDetailMainPanel::InitCreating()
{
	PanelUiEntity::InitCreating();
	float mainPanelWidth = 400;
	float mainPanelHeight = 1000;
	SetSize(mainPanelWidth, mainPanelHeight);

	Map* map = GetMap();
	int mapLayerIndex = GetMapLayerID();
	float nodeTypeTextBoxPanelHeight = 100;
	mSelectedNodeTypeTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, mainPanelWidth, nodeTypeTextBoxPanelHeight,1);
	//Transform & nodeTypeTextBoxPanelTransform =	mSelectedNodeTypeTextBoxPanel->GetTransform();
	//nodeTypeTextBoxPanelTransform.SetIndependentRotationFlag(true);
	//nodeTypeTextBoxPanelTransform.SetIndependentScaleFlag(true);
	//mSelectedNodeTypeTextBoxPanel->SetSelectAvailableFlag(false);
	EditorCommonHelperClass::SetObjectDefaultInit(mSelectedNodeTypeTextBoxPanel);


	mSelectedNodeTypeTextBoxPanel->SetText("");
	mSelectedNodeTypeTextBoxPanel->SetTexture("324a4f.png");
	mSelectedNodeTypeTextBoxPanel->SetDrawFlag(false);
	mSelectedNodeTypeTextBoxPanel->SetTextColor({ 1.0f,1.0f,1.0f });
	mSelectedNodeTypeTextBoxPanel->SetTextAlignmentDirection(ETextAlignmentDirection::eCenter);
	

	AddChildPanelUiEntity(mSelectedNodeTypeTextBoxPanel);

	mSelectedNodeTypeTextBoxPanel->SetPosition({ 0,mainPanelHeight / 2 - nodeTypeTextBoxPanelHeight / 2 ,-1 });


	mBlendNodeEditPanel = AnimationStateTransitionBlendNodeEditPanel::Create(map, mapLayerIndex);
	EditorCommonHelperClass::SetObjectDefaultInit(mBlendNodeEditPanel);
	mBlendNodeEditPanel->SetDrawFlag(false);
	AddChildPanelUiEntity(mBlendNodeEditPanel);
	mBlendNodeEditPanel->SetPosition({ 0,mainPanelHeight /2 -nodeTypeTextBoxPanelHeight - mBlendNodeEditPanel->GetHeight()/2 ,-1});

	mBlendNodeEditPanel->SetTexture("Green.png");


	//CreateBlendNodeEditPanel();

}

void Quad::AnimationStateTransitionNodeEditDetailMainPanel::CreateBlendNodeEditPanel()
{











}

void Quad::AnimationStateTransitionNodeEditDetailMainPanel::OffPreSelectedNodeEditPanel(EAnimationNodeType nodeType)
{
	switch (nodeType)
	{
	case EAnimationNodeType::eStateNode:



		break;
	case EAnimationNodeType::eBlendNode:
		mBlendNodeEditPanel->SetDrawFlag(false);
		break;



	}






}

void Quad::AnimationStateTransitionNodeEditDetailMainPanel::HandleWindowResize(Event* event)
{
	//WindowResizeEvent* windowResizeEvent = static_cast<WindowResizeEvent*>(event);

	
	Map * map =	GetMap();
	D3D12_VIEWPORT viewport = map->GetViewPort(GetMapLayerID());

	DirectX::XMFLOAT3 posW =GetTransform().GetPositionWorld();
	posW.x = viewport.Width - GetWidth() / 2;
	GetTransform().SetPositionWorld(posW);
	
}
