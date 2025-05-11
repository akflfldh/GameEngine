#include "EditorDirector/ExpandablePanel.h"

#include"TextBoxPanel.h"
#include"ButtonUiEntitiy.h"
#include"MathHelper.h"
#include"Utility.h"
#include"Map/Map.h"
Quad::ExpandablePanel::ExpandablePanel(const std::string& name)
	:PanelUiEntity(name), mNextPanelLocalPosY(0)
{
}

Quad::ExpandablePanel::~ExpandablePanel()
{
}

void Quad::ExpandablePanel::Initialize()
{
	PanelUiEntity::Initialize();


}

Quad::ExpandablePanel* Quad::ExpandablePanel::Create(Map* map, int mapLayerIndex, float width, float height)
{
	ExpandablePanel* ob = static_cast<ExpandablePanel*>(map->CreateObject("ExpandablePanel", mapLayerIndex));
	//ob->Initialize();
	ob->InitCreating(width, height);

	return ob;
}

void Quad::ExpandablePanel::InitCreating(float width, float height)
{
	PanelUiEntity::InitCreating();

	SetSize(width, height);

	//정확히 다음에추가되는  child panel의 tag panel 상단 모서리 위치 
	//child panel은 그 tag panel의 바로밑에위치한다.
	mNextPanelLocalPosY = height / 2;
}

void Quad::ExpandablePanel::AddChildPanelUiEntity(UiEntity* childPanel)
{
	PanelUiEntity::AddChildPanelUiEntity(childPanel);
}


void Quad::ExpandablePanel::Update(float deltaTime)
{
}

void Quad::ExpandablePanel::OnEvent(Event* event)
{
	PanelUiEntity::OnEvent(event);
}

void Quad::ExpandablePanel::AddChildPanelUiEntity(UiEntity* childPanel, const std::string& tagName)
{

	//tag이름을 나타내고 열고접을수있는 tag title엔티티 생성
	//위치는 마지막 판넬 밑
	//그이후 추가된 자식판넬을 그밑에 붙인다.


	Map* map = GetMap();
	//tag panel 
	int mapLayerIndex = GetMapLayerID();
	PanelUiEntity * tagPanel = PanelUiEntity::Create(map, mapLayerIndex);

	tagPanel->SetSize(GetWidth(), 30);
	tagPanel->SetTexture("Green.png");
	tagPanel->GetTransform().SetIndependentScaleFlag(true);
	tagPanel->GetTransform().SetIndependentRotationFlag(true);
	tagPanel->SetSelectAvailableFlag(false);
	PanelUiEntity::AddChildPanelUiEntity(tagPanel);

	const std::vector<UiEntity*>& childPanelVector = GetChildPanelUiEntityVector();
	//DirectX::XMFLOAT3 pos = childPanelVector[childPanelVector.size() - 1]->GetTransform().GetPositionLocal();
	DirectX::XMFLOAT3 pos = { 0,mNextPanelLocalPosY - tagPanel->GetHeight() / 2 , -1 };

	//tag panel의 위치 ,마지막판넬 바로 밑





	//expandable icon 
	//ButtonUiEntitiy* expandableButton = new ButtonUiEntitiy(childPanel->GetName() + "_TagExpandableButton");
//	ButtonUiEntity* expandableButton = static_cast<ButtonUiEntity*>(map->CreateObject("ButtonUiEntity"));
	ButtonUiEntity* expandableButton = ButtonUiEntity::Create(map, mapLayerIndex);
//	expandableButton->SetSystem(GetSystem());
	//expandableButton->Initialize();
	float buttonSize = tagPanel->GetHeight() - 4;
	expandableButton->SetSize(buttonSize, buttonSize);
	expandableButton->SetTexture("ExpandButton.png");
	//expandableButton->SetTexture(L"Red.png", EUiEntityMouseState::eHover);
	//expandableButton->SetTexture(L"Red.png", EUiEntityMouseState::eHoverSelect);
	expandableButton->SetHoverCallback([button = expandableButton]() {
		button->SetColorItensity({ 2.0f,2.0f,2.0f });
		});
	expandableButton->SetReleaseHoverCallback([button = expandableButton]()
		{
			button->SetColorItensity({ 1.0f,1.0f,1.0f });
		});


	//childPanelIndex는 아직 childPanel을 추가하지않았으니 index는 childPanelVector의 size값, 맨처음에는 스크롤바이들어가니 추가로 +1을 한다 
	expandableButton->SetLButtonDownCallback([button = expandableButton, expandablePanel = this,  expandState = true , childPanelIndex  = childPanelVector.size()  ]()mutable {
		DirectX::XMFLOAT4X4 buttonTextureTransform;
		if (expandState)
		{
			buttonTextureTransform = Utility::GetIdentityMatrixF();
			expandState = false;

			expandablePanel->CollapsePanel(childPanelIndex,true);

		}
		else
		{
			DirectX::XMStoreFloat4x4(&buttonTextureTransform, DirectX::XMMatrixRotationZ(-90.0f * DirectX::XM_PI / 180.0f));
			expandState = true;
			expandablePanel->CollapsePanel(childPanelIndex, false);
		}
		button->SetTextureTransform(buttonTextureTransform);

	});





	expandableButton->GetTransform().SetIndependentScaleFlag(true);
	expandableButton->GetTransform().SetIndependentRotationFlag(true);
	expandableButton->SetSelectAvailableFlag(false);
	DirectX::XMFLOAT4X4 buttonTextureTransform;
	DirectX::XMStoreFloat4x4(&buttonTextureTransform, DirectX::XMMatrixRotationZ(-90.0f * DirectX::XM_PI / 180.0f));
	expandableButton->SetTextureTransform(buttonTextureTransform);

	tagPanel->AddChildPanelUiEntity(expandableButton);
	expandableButton->GetTransform().SetPositionLocal({ -tagPanel->GetWidth() / 2 + expandableButton->GetWidth() / 2, 0, -1 });




	tagPanel->GetTransform().SetPositionLocal(pos);
	//TextBoxPanel* textBoxPanel = new TextBoxPanel(childPanel->GetName() + "_TagTextBoxPanel");
	//TextBoxPanel* textBoxPanel = static_cast<TextBoxPanel*>(map->CreateObject("TextBoxPanel"));
	TextBoxPanel* textBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, tagName.size() * 15, 20);
	//textBoxPanel->SetSystem(GetSystem());
	//textBoxPanel->Initialize(tagName.size() * 15, 20);
	textBoxPanel->GetTransform().SetIndependentScaleFlag(true);
	textBoxPanel->GetTransform().SetIndependentRotationFlag(true);
	textBoxPanel->SetSelectAvailableFlag(false);
	textBoxPanel->SetTextColor({ 0.0f,0.0f,0.0f });

	tagPanel->AddChildPanelUiEntity(textBoxPanel);
	textBoxPanel->GetTransform().SetPositionLocal({ -tagPanel->GetWidth() / 2 + 30 + textBoxPanel->GetWidth()/2,0,-1 });
	textBoxPanel->SetText(tagName);
	textBoxPanel->SetTexture("Transparent.png");














	//child panel 위치 조정 






	childPanel->GetTransform().SetIndependentScaleFlag(true);
	childPanel->GetTransform().SetIndependentRotationFlag(true);
	childPanel->SetSelectAvailableFlag(false);


	PanelUiEntity::AddChildPanelUiEntity(childPanel);
	float childPanelHeight = childPanel->GetHeight();
	childPanel->SetSize(GetWidth(), childPanelHeight);
	pos.y -= (tagPanel->GetHeight() / 2 + childPanelHeight / 2);
	pos.z = -1.0f;
	childPanel->GetTransform().SetPositionLocal(pos);










	mNextPanelLocalPosY = pos.y - childPanelHeight / 2;

















}
														//true : 접기 ,false: 펼치기
void Quad::ExpandablePanel::CollapsePanel(int panelIndex, bool collapseState)
{


	const std::vector<UiEntity*>& childPanelVector = GetChildPanelUiEntityVector();
	float shiftY = childPanelVector[panelIndex]->GetHeight();


	if (collapseState)
	{
		childPanelVector[panelIndex]->SetDrawFlag(false);
	//	childPanelVector[panelIndex]->SetActiveFlag(false);

	}
	else
	{
		childPanelVector[panelIndex]->SetDrawFlag(true);
	//	childPanelVector[panelIndex]->SetActiveFlag(true);
		shiftY *= -1.0f;
	}


	DirectX::XMFLOAT3 pos;
	for (int i = panelIndex + 1; i<childPanelVector.size(); ++i)
	{
		pos = childPanelVector[i]->GetTransform().GetPositionLocal();
		pos.y += shiftY;
		childPanelVector[i]->GetTransform().SetPositionLocal(pos);
	}



}

