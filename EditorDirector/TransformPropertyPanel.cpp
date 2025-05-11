#include "TransformPropertyPanel.h"
#include"MouseEvent.h"
#include"System/AttributeUiSystem.h"
#include"GameObjectSelectEvent.h"

Quad::TransformPropertyPanel::TransformPropertyPanel(const std::string& name)
	:PanelUiEntity(name), mTagTextBox(nullptr)
{


}

Quad::TransformPropertyPanel::~TransformPropertyPanel()
{
}

void Quad::TransformPropertyPanel::Initialize( )
{
	PanelUiEntity::Initialize();

	;
	RegisterAcceptEvent("MouseMove");
	



}

Quad::TransformPropertyPanel* Quad::TransformPropertyPanel::Create(Map* map, int mapLayerIndex, float width, float height, const std::string& tagName, int textBoxNum)
{
	TransformPropertyPanel* ob = static_cast<TransformPropertyPanel*>(map->CreateObject("TransformPropertyPanel", mapLayerIndex));
	//ob->Initialize();
	ob->InitCreating(width, height, tagName, textBoxNum);

	return ob;
}

void Quad::TransformPropertyPanel::Update(float deltaTime)
{
	PanelUiEntity::Update(deltaTime);

	auto updateCallback = GetUpdateCallback();
	if ((*updateCallback)!=nullptr)
		(*updateCallback)(this ,deltaTime);

	








}

void Quad::TransformPropertyPanel::OnEvent(Event* event)
{
	PanelUiEntity::OnEvent(event);

	
	const std::string& eventName = event->GetName();

	if (eventName == "GameObjectSelect")
	{
		GameObjectSelectEvent* selectEvent = (GameObjectSelectEvent*)event;

		SetSelectedObject(selectEvent->GetSelectObject());
	}




}

void Quad::TransformPropertyPanel::SetSelectedObject(Object* object)
{
	mSelectedObject = object;
}

Quad::Object* Quad::TransformPropertyPanel::GetSelectedObject() const
{
	return mSelectedObject;
}

void Quad::TransformPropertyPanel::SetXYZ(const DirectX::XMFLOAT3& xyz)
{


	std::vector<std::string> xyzText(3);
	xyzText[0]= std::to_string(xyz.x);
	xyzText[1] = std::to_string(xyz.y);
	xyzText[2] = std::to_string(xyz.z);



	
	for (int i = 0; i < mTextBoxVector.size(); ++i)
	{
	
		mTextBoxVector[i]->SetText(xyzText[i]);
	}

}

void Quad::TransformPropertyPanel::SetXTextBoxEnterCallback(const std::function<void()>& callback)
{
	mTextBoxVector[0]->SetEnterCallBack(callback);

}

void Quad::TransformPropertyPanel::SetYTextBoxEnterCallback(const std::function<void()>& callback)
{
	mTextBoxVector[1]->SetEnterCallBack(callback);
}

void Quad::TransformPropertyPanel::SetZTextBoxEnterCallback(const std::function<void()>& callback)
{
	mTextBoxVector[2]->SetEnterCallBack(callback);
}

std::string Quad::TransformPropertyPanel::GetTextX() const
{

	return mTextBoxVector[0]->GetText();
	// TODO: 여기에 return 문을 삽입합니다.
}

std::string Quad::TransformPropertyPanel::GetTextY() const
{
	return mTextBoxVector[1]->GetText();
	// TODO: 여기에 return 문을 삽입합니다.
}

std::string Quad::TransformPropertyPanel::GetTextZ() const
{
	return mTextBoxVector[2]->GetText();
	// TODO: 여기에 return 문을 삽입합니다.
}

void Quad::TransformPropertyPanel::SetDrawFlag(bool flag)
{
	PanelUiEntity::SetDrawFlag(flag);

	mTagTextBox->SetTextDrawFlag(flag);

	std::for_each(mTextBoxVector.begin(), mTextBoxVector.end(), [flag](TextBox* textbox) {
		textbox->SetTextDrawFlag(flag);
		textbox->SetTextEditMode(flag);
		});




}

void Quad::TransformPropertyPanel::HandleLButtonDown(Event* pEvent)
{


	StateComponent* stateComponent = GetStateComponent();

	auto callback = GetEventCallback("LButtonDown");
	if (callback != nullptr)
		(*callback)(pEvent);

	if (stateComponent->GetHoverState())
	{
		stateComponent->SetLButtonDownState(true);
	}


	for (int i = 0; i < mTextBoxVector.size(); ++i)
		mTextBoxVector[i]->OnEvent(pEvent);







}

void Quad::TransformPropertyPanel::HandleLButtonUp(Event* pEvent)
{
	StateComponent* stateComponent = GetStateComponent();


	auto callback = GetEventCallback("LButtonUp");
	if (callback != nullptr)
		(*callback)(pEvent);




	if (stateComponent->GetLButtonDownState())
	{
		stateComponent->SetLButtonDownState(false);


		for (int i = 0; i < mTextBoxVector.size(); ++i)
		{
			mTextBoxVector[i]->OnEvent(pEvent);
		}

	}


}

void Quad::TransformPropertyPanel::InitCreating(float width, float height, const std::string& tagName, int textBoxNum)
{

	PanelUiEntity::InitCreating();



	SetTexture("Black.png");



	SetSize(width, height);


	if (textBoxNum < 1)
		textBoxNum = 1;
	else if (textBoxNum > 3)
		textBoxNum = 3;

	mTextBoxVector.resize(textBoxNum, nullptr);


	Map* map = GetMap();
	int mapLayerIndex = GetMapLayerID();
	//mTagTextBox = static_cast<TextBox*>(map->CreateObject("TextBox"));
	mTagTextBox = TextBox::Create(map, mapLayerIndex, 15);

	float textBox = -width / 2;
	//tag textbox 
	//mTagTextBox->SetSystem(GetSystem());
//	mTagTextBox->Initialize(15);
	mTagTextBox->SetAlignmentDirection(ETextAlignmentDirection::eCenter);
	mTagTextBox->SetSize(120, 15);
	mTagTextBox->GetTransform().SetIndependentScaleFlag(true);
	AddChildPanelUiEntity(mTagTextBox);
	textBox += mTagTextBox->GetWidth() / 2;

	mTagTextBox->SetPosition(textBox, 0, -1);
	mTagTextBox->SetText(tagName);
	mTagTextBox->SetSelectAvailableFlag(false);
	mTagTextBox->SetTexture("Transparent.png");
	mTagTextBox->SetTextEditMode(false);
	//mTagTextBox.SetDrawFlag(true);
	mTagTextBox->SetTextColor({ 0.0F,0.0F,0.0F });

	textBox += mTagTextBox->GetWidth() / 2 + 20;
	for (int i = 0; i < mTextBoxVector.size(); ++i)
	{
		//mTextBoxVector[i] = new TextBox(GetName() + "_textBox" + std::to_string(i));
		//mTextBoxVector[i] = static_cast<TextBox*>(map->CreateObject("TextBox"));
		mTextBoxVector[i] = TextBox::Create(map, mapLayerIndex, 255, ETextLineMode::eOneLine);

		//mTextBoxVector[i]->SetSystem(GetSystem());
		//mTextBoxVector[i]->Initialize(255, ETextLineMode::eOneLine);
		mTextBoxVector[i]->SetSize(130, 20);
		mTextBoxVector[i]->GetTransform().SetIndependentScaleFlag(true);
		AddChildPanelUiEntity(mTextBoxVector[i]);
		textBox += mTextBoxVector[i]->GetWidth() / 2;

		mTextBoxVector[i]->SetPosition(textBox, 0, -1);
		mTextBoxVector[i]->SetSelectAvailableFlag(false);
		mTextBoxVector[i]->SetDrawFlag(true);
		mTextBoxVector[i]->SetText("10");
		mTextBoxVector[i]->SetTextDrawFlag(true);
		mTextBoxVector[i]->SetTexture("TextBox.png");
		mTextBoxVector[i]->SetTextColor({ 0.0F,0.0F,0.0F });
		mTextBoxVector[i]->SetEnterEndFlag(true);

		textBox += mTextBoxVector[i]->GetWidth() / 2 + 25;

		mTextBoxVector[i]->SetTextFilterMode(ETextFilterMode::eAlphabet);
	}




}


