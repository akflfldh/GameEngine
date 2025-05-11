#include "ComboBoxPanel.h"
#include"Map/Map.h"
#include"EditorCommonHelperClass.h"
#include"TextBoxPanel.h"
#include"ButtonUiEntitiy.h"
#include"ListPanel.h"

#include"MouseEvent.h"
Quad::ComboBoxPanel::ComboBoxPanel(const std::string& name)
	:PanelUiEntity(name), mHeader(nullptr), mHeaderTextBoxPanel(nullptr),mListPanel(nullptr), mDownButtonState(false), mCurrentListItemIndex(0)
{
}

Quad::ComboBoxPanel::~ComboBoxPanel()
{
}

void Quad::ComboBoxPanel::Initialize()
{
	PanelUiEntity::Initialize();

}

Quad::ComboBoxPanel* Quad::ComboBoxPanel::Create(Map* map, int mapLayerIndex, float width , int listNum, float listItemHeight)
{
	ComboBoxPanel* ob = static_cast<ComboBoxPanel*>(map->CreateObject("ComboBoxPanel", mapLayerIndex));

	ob->InitCreating(width ,listNum,listItemHeight);

	return ob;
}

void Quad::ComboBoxPanel::Update(float deltaTime)
{
	PanelUiEntity::Update(deltaTime);

}

void Quad::ComboBoxPanel::OnEvent(Event* event)
{
	PanelUiEntity::OnEvent(event);

}

void Quad::ComboBoxPanel::SetDrawFlag(bool flag)
{
	PanelUiEntity::SetDrawFlag(flag);


	if (flag == true)
	{
		//버튼이 눌렸는지 확인하고 testListPanel 의 draw를결정

		if (!mDownButtonState)
		{
			mListPanel->SetDrawFlag(false);
		}
	}

}

void Quad::ComboBoxPanel::SetText(const std::string& text, int listItemIndex)
{
	std::vector<UiEntity*>& childUiEntityVector = mListPanel->GetChildPanelUiEntityVector();
	TextBoxPanel * textBoxPanel =static_cast<TextBoxPanel*>(childUiEntityVector[listItemIndex]);

	textBoxPanel->SetText(text);

}

void Quad::ComboBoxPanel::RegisterLButtonDownCallback(int listItemIndex, const std::function<void(Event*)>& callback)
{
	mTextBoxPanelListItemVector[listItemIndex]->RegisterEventCallback("LButtonDown", callback);
}

void Quad::ComboBoxPanel::ClickDonwButton()
{
	if (mDownButtonState)
	{
		//리스트 판넬을 접는다.
		CollapseListPanel();
		
	}
	else
	{
		//펼친다.
		ExpandListPanel();

	}



}

void Quad::ComboBoxPanel::ClickListItem(int index)
{
	const std::string text = mTextBoxPanelListItemVector[index]->GetText();

	mHeaderTextBoxPanel->SetText(text);
	mCurrentListItemIndex = index;


}

int Quad::ComboBoxPanel::GetCurrentSelectedListItemIndex() const
{
	return mCurrentListItemIndex;
}

const char* Quad::ComboBoxPanel::GetCurrentSelectedListItemText() const
{
	return mHeaderTextBoxPanel->GetText().c_str();
}



void Quad::ComboBoxPanel::InitCreating(float width ,int listNum, float listItemHeight)
{

	PanelUiEntity::InitCreating();
	mWidth = width;
	mListNum = listNum;
	mListItemHeight = listItemHeight;
	//mTextBoxPanelListItemVector;

					//combox header
	Map* map = GetMap();
	int mapLayerIndex = GetMapLayerID();
	SetSize(width, listItemHeight);


	PanelUiEntity* header = PanelUiEntity::Create(map, mapLayerIndex);
	mHeader = header;
	EditorCommonHelperClass::SetObjectDefaultInit(header);
	header->SetSize(width, listItemHeight);
	
	TextBoxPanel* headerTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex,width,listItemHeight);
	mHeaderTextBoxPanel = headerTextBoxPanel;
	EditorCommonHelperClass::SetObjectDefaultInit(headerTextBoxPanel);
	headerTextBoxPanel->SetText("123");
	headerTextBoxPanel->SetTextColor({ 0,0,0 });

	header->AddChildPanelUiEntity(headerTextBoxPanel);
	headerTextBoxPanel->SetPosition(0, 0, -1);





	ButtonUiEntity* downButton = ButtonUiEntity::Create(map, mapLayerIndex);
	EditorCommonHelperClass::SetObjectDefaultInit(downButton);
	downButton->SetSize(listItemHeight, listItemHeight);
	header->AddChildPanelUiEntity(downButton);
	downButton->SetPosition(width/2 - listItemHeight/2 ,0,-2);
	downButton->SetTexture("Red.png");
	

	mListPanel = ListPanel::Create(map, mapLayerIndex,width,listItemHeight*listNum,listItemHeight);
	EditorCommonHelperClass::SetObjectDefaultInit(mListPanel);
	mListPanel->SetSize(width, listItemHeight * listNum);
	AddChildPanelUiEntity(mListPanel);



	float totalHeight = listItemHeight * (listNum + 1);//펼쳤을때의 main panel의높이 (header + listPanel)
	mListPanel->SetPosition(0, -totalHeight / 2 + mListPanel->GetHeight() / 2, -2);
	mListPanel->SetDrawFlag(false);
	mListPanel->SetTexture("Blue.png");



	downButton->SetLButtonDownCallback([comboBoxPanel = this ]() {
		
		comboBoxPanel->ClickDonwButton();

		});
	

	std::vector<TextBoxPanel*> textBoxPanelItemVector(listNum,nullptr);

	auto textBoxPanelHoverCallback = [](Event* pEvent, TextBoxPanel* textBoxPanel) {
		MouseEvent* mouseEvent = static_cast<MouseEvent*>(pEvent);

		StateComponent * stateComponent =textBoxPanel->GetStateComponent();
	

		if (!stateComponent->GetSelectState())
		{
			if (mouseEvent->GetHoverState())
			{
				textBoxPanel->SetColorItensity({ 3.0f,3.0f,3.0f });
			}
			else 
			{
				textBoxPanel->SetColorItensity({ 1.0f,1.0f,1.0f });

			}
		}


		};

	auto textBoxPanelLbuttonDownCallback = [comboBoxPanel = this ](Event* pEvent, TextBoxPanel* textBoxPanel,int listItemIndex) {
		
		StateComponent * stateComponent =	textBoxPanel->GetStateComponent();
		if (stateComponent->GetHoverState())
		{
			textBoxPanel->SetColorItensity({ 0,0,1.0F });
			stateComponent->SetSelectState(true);

			comboBoxPanel->ClickListItem(listItemIndex);


		}
		else
		{
			textBoxPanel->SetColorItensity({ 1.0f,1.0f,1.0F });
			stateComponent->SetSelectState(false);
		}



		};

	auto textBoxPanelLbuttonUpCallback = [](Event* pEvent, TextBoxPanel* textBoxPanel) {

		StateComponent* stateComponent = textBoxPanel->GetStateComponent();
		if (stateComponent->GetLButtonDownState() && !stateComponent->GetHoverState())
		{
			textBoxPanel->SetColorItensity({ 1.0f,1.0f,1.0F });

		}
		};



	for (int i = 0; i < textBoxPanelItemVector.size(); ++i)
	{
			
			TextBoxPanel* textBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, width, listItemHeight);
			EditorCommonHelperClass::SetObjectDefaultInit(textBoxPanel);

			mListPanel->AddChildPanelEntity(textBoxPanel);
			textBoxPanel->SetTextColor({ 0,0,0 });

			textBoxPanel->RegisterEventCallback("Hover", std::bind(textBoxPanelHoverCallback, std::placeholders::_1, textBoxPanel));

			textBoxPanel->RegisterEventCallback("LButtonDown", std::bind(textBoxPanelLbuttonDownCallback, std::placeholders::_1, textBoxPanel,i));


			textBoxPanelItemVector[i] = textBoxPanel;
		
	}
//	textBoxPanelItemVector[0]->SetText("ASD");
	//textBoxPanelItemVector[1]->SetText("12FD");
//	textBoxPanelItemVector[2]->SetText("LDM");

	mTextBoxPanelListItemVector = std::move(textBoxPanelItemVector);



	AddChildPanelUiEntity(header);


}

void Quad::ComboBoxPanel::HandleLButtonDown(Event* pEvent)
{
	PanelUiEntity::HandleLButtonDown(pEvent);

	StateComponent* stateComponent = GetStateComponent();


	if (!stateComponent->GetHoverState())
	{
		if(mDownButtonState)
			CollapseListPanel();
	}


}

void Quad::ComboBoxPanel::ExpandListPanel()
{

	mListPanel->SetDrawFlag(true);
	float totalHeight = mListItemHeight * (mListNum + 1);
	SetSize(mWidth, totalHeight);

	DirectX::XMFLOAT3 pos = GetTransform().GetPositionLocal();
	pos.y -= (totalHeight / 2 - mListItemHeight / 2);
	SetPosition(pos);



	pos = mHeader->GetTransform().GetPositionLocal();
	pos.y = totalHeight / 2 - mHeader->GetHeight() / 2;
	mHeader->SetPosition(pos);





	mDownButtonState = true;
}

void Quad::ComboBoxPanel::CollapseListPanel()
{
	mListPanel->SetDrawFlag(false);

	//사이즈조절

	SetSize(mWidth, mListItemHeight);

	//위치조절
	float totalHeight = mListItemHeight * (mListNum + 1);
	DirectX::XMFLOAT3 pos = GetTransform().GetPositionLocal();
	pos.y += (totalHeight / 2 - mListItemHeight / 2);
	SetPosition(pos);


	pos = mHeader->GetTransform().GetPositionLocal();
	pos.y = 0;
	mHeader->SetPosition(pos);




	mDownButtonState = false;
}
