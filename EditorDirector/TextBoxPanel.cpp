#include "TextBoxPanel.h"
#include"Map/Map.h"
Quad::TextBoxPanel::TextBoxPanel(const std::string& name)
	:PanelUiEntity(name)
{



}

Quad::TextBoxPanel::~TextBoxPanel()
{
}

void Quad::TextBoxPanel::Initialize()
{
	PanelUiEntity::Initialize();

}




Quad::TextBoxPanel* Quad::TextBoxPanel::Create(Map* map, int mapLayerIndex, float width, float height, int fontSize )
{
	TextBoxPanel* ob = static_cast<TextBoxPanel*>(map->CreateObject("TextBoxPanel", mapLayerIndex));
	//ob->Initialize();
	ob->InitCreating(width, height, fontSize);
	return ob;
}

void Quad::TextBoxPanel::SetText(const std::string& text)
{

	mTextBox->SetText(text);

}

std::string Quad::TextBoxPanel::GetText() const
{
	return mTextBox->GetText();
	// TODO: 여기에 return 문을 삽입합니다.
}

void Quad::TextBoxPanel::Update(float deltaTime)
{
}

void Quad::TextBoxPanel::OnEvent(Event* event)
{
	PanelUiEntity::OnEvent(event);

}

void Quad::TextBoxPanel::SetDrawFlag(bool flag)
{
	Object::SetDrawFlag(flag);
	
	//mTextBox->SetDrawFlag(flag);
	mTextBox->SetTextDrawFlag(flag);

	if (flag == false)
		mTextBox->SetTextEditMode(flag);

}

void Quad::TextBoxPanel::SetActiveFlag(bool flag)
{

	Object::SetActiveFlag(flag);

	//mTextBox->SetDrawFlag(flag);
	mTextBox->SetActiveFlag(flag);
	//mTextBox->SetTextEditMode(flag);
	



}

void Quad::TextBoxPanel::SetStencilRefValue(unsigned char value)
{
	Object::SetStencilRefValue(value);

	const std::vector<UiEntity*>& childPanelVector = GetChildPanelUiEntityVector();

	for (int i = 0; i < childPanelVector.size(); ++i)
	{
		childPanelVector[i]->SetStencilRefValue(value);

	}
}

void Quad::TextBoxPanel::SetTextColor(const DirectX::XMFLOAT3& textColor)
{
	mTextBox->SetTextColor(textColor);
}

void Quad::TextBoxPanel::SetTextAlignmentDirection(ETextAlignmentDirection dir)
{
	mTextBox->SetAlignmentDirection(dir);

}

void Quad::TextBoxPanel::InitCreating(float width, float height,int fontSizeIndex)
{

	PanelUiEntity::InitCreating();

	SetSize(width, height);
	Map* map = GetMap();
	int mapLayerIndex = GetMapLayerID();
	//mTextBox = new TextBox(GetName() + "_TextBox");
	//mTextBox = static_cast<TextBox*>(map->CreateObject("TextBox"));
	mTextBox = TextBox::Create(map, mapLayerIndex,50,Quad::ETextLineMode::eMultiLine,fontSizeIndex);
	//mTextBox->SetSystem(GetSystem());
//	mTextBox->Initialize(50);
	mTextBox->SetSelectAvailableFlag(false);
	mTextBox->GetTransform().SetIndependentScaleFlag(true);
	mTextBox->GetTransform().SetIndependentRotationFlag(true);
	mTextBox->SetSize(width, height);
	mTextBox->SetDrawFlag(false);
	mTextBox->SetTextEditMode(false);
	mTextBox->SetTextDrawFlag(true);
	AddChildPanelUiEntity(mTextBox);
	mTextBox->SetPosition(0, 0, -1);

}
