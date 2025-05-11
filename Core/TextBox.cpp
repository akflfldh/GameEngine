#include "TextBox.h"
#include"MouseEvent.h"
#include"ObjectFactory/TextFactory.h"
#include"KeyEvent.h"
#include"TextCharacterPanelEntity.h"
#include"Event.h"
#include"Font.h"
#include"System.h"
#include"Map/Map.h"
#include"Map/SceneGraph.h"

#include"KeyBoard.h"
#include"Core/BaseWindow.h"
#include"Mouse.h"


#include"Collision/CollisionHelper.h"
#include"WindowActiveEvent.h"
#include<cwctype>
#include"Component/UiColliderComponent.h"

Quad::TextBox::TextBox(const std::string& name)
	:UiEntity(name), mTextColor({ 1.0f,1.0f,1.0f }), mAlignmentDirection(ETextAlignmentDirection::eLeft),
	mLineInfoVector(1), mDragCursorStartPositionInText(-1),mTextFilterMode(ETextFilterMode::eNone), mTextLineMode(ETextLineMode::eMultiLine), mOnlyEnglishFlag(false), mOnlyUpperCaseFlag(false)
{

}

Quad::TextBox::~TextBox()
{

}

void Quad::TextBox::Initialize()
{
	
	UiEntity::Initialize();

	RegisterAcceptEvent("WindowActive");

	mHbBuffer = hb_buffer_create();
	hb_buffer_set_direction(mHbBuffer, HB_DIRECTION_LTR);
	hb_buffer_set_script(mHbBuffer, HB_SCRIPT_HANGUL);
	hb_buffer_set_language(mHbBuffer, hb_language_from_string("ko", -1));


	//mTextLineMode = textLineMode;
	//if (font == nullptr)
	//{
	//	mFont =  TextFactory::GetFont(L"DefaultFont.ttf", fontSizeIndex);
	//}
	//mTextCharacterPanelEntityVector.resize(maxCharacterNum,nullptr);

	//SetLineHeight((mFont->GetAscender() - mFont->GetDescender() )* mLineHeightScaleFactor);

	//SetBaseLinePanelCharacterPosY(GetLineHeight() / 2 - mFont->GetAscender() * mLineHeightScaleFactor);

	//Map* map = GetMap();

	//
	//mTextCursorEntity = static_cast<TextCursorEntity*>(map->CreateObject("TextCursorEntity"));
	//
	////mTextCursorEntity->SetSystem(GetSystem());
	//mTextCursorEntity->Initialize();

	//mTextCursorEntity->GetTransform().SetIndependentScaleFlag(true);
	//mTextCursorEntity->SetSelectAvailableFlag(false);
	//mTextCursorEntity->SetSize(1.0f, GetLineHeight());
	//AddChildObject(mTextCursorEntity);
	//SetCursorPosition(0);


	//mTextCursorEntity->SetDrawFlag(false);
	//mTextCursorEntity->SetActiveFlag(false);


	//mVisibleLineStartIndex = 0;


}

Quad::TextBox* Quad::TextBox::Create(Map* map, int mapLayerIndex, int maxCharacterNum, ETextLineMode textLineMode, int fontSizeIndex, Font* mFont)
{
	TextBox* ob = static_cast<TextBox*>(map->CreateObject("TextBox", mapLayerIndex));
	//ob->Initialize();
	ob->InitCreating(maxCharacterNum, textLineMode, fontSizeIndex, mFont);

	return ob;
}

void Quad::TextBox::SetStencilRefValue(unsigned char value)
{
	Object::SetStencilRefValue(value);
	for (int i = 0; i < mText.size(); ++i)
	{
		mTextCharacterPanelEntityVector[i]->SetStencilRefValue(value);
	}

}

void Quad::TextBox::SetDrawFlag(bool flag)
{
	UiEntity::SetDrawFlag(flag);

	SetTextDrawFlag(flag);


}

void Quad::TextBox::SetTextDrawFlag(bool flag)
{
	mTextDrawFlag = flag;
	ETextLineMode textLineMode = GetTextLineMode();

	if (textLineMode == ETextLineMode::eMultiLine)
	{
		if (flag == true)
		{

			std::for_each(mTextCharacterPanelEntityVector.begin(), mTextCharacterPanelEntityVector.begin() + mText.size(),
				[textBoxWidth = GetWidth(), visibleStartLineIndex = mVisibleLineStartIndex, visibleLineNum = mVisibleLineNum](TextCharacterPanelEntity* panel) {

					//mVisibleLineNum;
					// mVisibleLineStartIndex;

					int characterPanelLineNum = panel->GetLineNum();

					if ((characterPanelLineNum < visibleStartLineIndex || characterPanelLineNum >= (visibleLineNum + visibleStartLineIndex)))
					{
						panel->GetCharacterEntity()->SetDrawFlag(false);
						
					}
					else
					{
						wchar_t ch = panel->GetCharacter();
						if ((ch != L'\r' && ch != L' '))
						{
							panel->GetCharacterEntity()->SetDrawFlag(true);
							//panel->SetDrawFlag(true);
						}
					}
				});
		}
		else
		{
			std::for_each(mTextCharacterPanelEntityVector.begin(), mTextCharacterPanelEntityVector.begin() + mText.size(),
				[textBoxWidth = GetWidth()](TextCharacterPanelEntity* panel) {

					//mVisibleLineNum;
					// mVisibleLineStartIndex;


					wchar_t ch = panel->GetCharacter();
					if ((ch != L'\r' && ch != L' '))
					{
						panel->GetCharacterEntity()->SetDrawFlag(false);
						//panel->SetDrawFlag(true);
					}
				});
		}
		


		
	}
	else if (textLineMode == ETextLineMode::eOneLine)
	{
		if (flag == false)
		{
			std::for_each(mTextCharacterPanelEntityVector.begin(), mTextCharacterPanelEntityVector.begin() + mText.size(),
				[](TextCharacterPanelEntity* panel) {

					panel->GetCharacterEntity()->SetDrawFlag(false);
					panel->SetDrawFlag(false);
				});
		}
		else
		{
			SetChracterDrawInOneLine();
			//std::for_each(mTextCharacterPanelEntityVector.begin(), mTextCharacterPanelEntityVector.begin() + mText.size(),
			//	[flag, textBoxWidth = GetWidth()](TextCharacterPanelEntity* panel) {

			//		wchar_t ch = panel->GetCharacter();
			//		float chPosX = panel->GetTransform().GetPositionLocal().x;
			//		float chWidth = panel->GetWidth();
			//		if ((ch != L' ') && ((chPosX + chWidth / 2) >= -textBoxWidth / 2) && ((chPosX - chWidth / 2) <= textBoxWidth / 2))
			//		{
			//			panel->GetCharacterEntity()->SetDrawFlag(flag);
			//			//panel->SetDrawFlag(true);
			//		}
			//		else
			//		{
			//			panel->GetCharacterEntity()->SetDrawFlag(false);
			//			panel->SetDrawFlag(false);
			//		}
			//	});

		}




	}

	//SetVisibleLine();

}

bool Quad::TextBox::GetTextDrawFlag() const
{
	return mTextDrawFlag;
}

void Quad::TextBox::SetTextEditMode(bool state)
{
	HWND windowHande = GetSystem()->GetWindow()->GetWindowHandle();
	mTextEditMode = state;
	if (state)
	{
		mTextCursorEntity->SetDrawFlag(true);
		mTextCursorEntity->SetActiveFlag(true);	

		SetDrawFlag(true);
		//SetActiveFlag(true);


		KeyBoard::SetTextBoxActiveFlag(true, windowHande,this);
		KeyBoard::CaptureObject(this);
	}
	else
	{

		mTextCursorEntity->SetDrawFlag(false);
		mTextCursorEntity->SetActiveFlag(false);
		//SetDrawFlag(false);
		SetCurrentTexture(GetDefaultTexture());
		ReSetDragRegion();
		KeyBoard::SetTextBoxActiveFlag(false, windowHande,this);
		KeyBoard::ReleaseCaptureObject(this);
		//SetActiveFlag(false);



	}






}

bool Quad::TextBox::GetTextEditMode() const
{
	return mTextEditMode;
}

void Quad::TextBox::ResetVisibleLine()
{

	SetCursorPosition(0);
	SetVisibleLine();

}

void Quad::TextBox::SetEnterCallBack(std::function<void()> callback)
{
	mEnterCallback = callback;
}

void Quad::TextBox::SetEnterEndFlag(bool flag)
{
	mEnterEndFlag = flag;

}

bool Quad::TextBox::GetEnterEndFlag() const
{
	return mEnterEndFlag;
}

void Quad::TextBox::SetTextFilterMode(ETextFilterMode filterMode)
{
	mTextFilterMode = filterMode;

}

Quad::ETextFilterMode Quad::TextBox::GetTextFilterMode() const
{
	return mTextFilterMode;
}


void Quad::TextBox::SetSize(float width, float height)
{
	mVisibleLineNum = height / GetLineHeight();

	if (mVisibleLineNum == 0)
	{
		mVisibleLineNum = 1;
		height = GetLineHeight();
	}
	UiEntity::SetSize(width, height);

	
	mCharacterPanelStartLocalPosX = -width/2.0f;
	mCharacterPanelStartLocalPosY = height/2.0f;


	ReBuildCharacterMetrics(0);
	SetCursorPosition(mTextCursorEntity->GetPositionInText());
	




}

void Quad::TextBox::SetText(const std::string& text)
{
	
	std::wstring str= Utility::ConvertToWString(text, true);



	//Reset(int remainnum);   n개까지는 남겨두고 빠르게 재사용할수있게하자.(text는 초기화되고, characterEntity가 맵에서 제거되지않는것일뿐).
	Reset(str);

	ReBuildCharacterMetrics(0);


	//설정하면서 글자들이 보여지게됬으니, 다시 원래상태를 유지하도록처리한다
	if (GetTextDrawFlag() == false)
		SetTextDrawFlag(false);

}

std::string Quad::TextBox::GetText() const
{

	return	Utility::ConvertToString(mText, true);

//	return mText;
	// TODO: 여기에 return 문을 삽입합니다.
}

void Quad::TextBox::SetTextColor(const DirectX::XMFLOAT3& color)
{
	mTextColor = color;

	std::for_each(mTextCharacterPanelEntityVector.begin(), mTextCharacterPanelEntityVector.begin()+mText.size(), [&color](TextCharacterPanelEntity* panelEntity) {panelEntity->GetCharacterEntity()->SetColor(color); });

}

const DirectX::XMFLOAT3& Quad::TextBox::GetTextColor() const
{
	return mTextColor;
	// TODO: 여기에 return 문을 삽입합니다.
}

void Quad::TextBox::SetAlignmentDirection(ETextAlignmentDirection dir)
{
	mAlignmentDirection = dir;
}

Quad::ETextAlignmentDirection Quad::TextBox::GetAlignmentDirection() const
{
	return mAlignmentDirection;
}

void Quad::TextBox::Update(float deltaTime)
{
	

	////이업데이트부분 개선해야한다. 
	//for (int i = 0; i < mText.size(); ++i)
	//{
	//	bool ss= GetTransform().GetDirtyFlag();
	//	mTextCharacterPanelEntityVector[i]->Update(deltaTime);
	//	//한프레임정도 밀린다 (씬그래프에서 처리되는게아니라서) 하지만 한프레임정도는크게 상관없을거같다.

	//		if (GetTransform().GetDirtyFlag())
	//			mTextCharacterPanelEntityVector[i]->GetTransform().UpdateWorldMatrix(GetTransform().GetWorldMatrix());


	//		if (mTextCharacterPanelEntityVector[i]->GetTransform().GetDirtyFlag())
	//		{
	//			const DirectX::XMFLOAT4X4& panelWorldMatrix = mTextCharacterPanelEntityVector[i]->GetTransform().GetWorldMatrix();
	//			mTextCharacterPanelEntityVector[i]->GetCharacterEntity()->GetTransform().UpdateWorldMatrix(
	//				panelWorldMatrix
	//			);

	//			mTextCharacterPanelEntityVector[i]->GetCollider()->Update(deltaTime, panelWorldMatrix);
	//		}
	//	

	//	//mTextCharacterPanelEntityVector[i]->GetCharacterEntity()->GetCollider()->Update(deltaTime,
	//	//	mTextCharacterPanelEntityVector[i]->GetCharacterEntity()->GetTransform().GetWorldMatrix());

	//}

	//mTextCursorEntity.GetTransform().UpdateWorldMatrix(GetTransform().GetWorldMatrix());
	//mTextCursorEntity.GetCollider()->Update(deltaTime, mTextCursorEntity.GetTransform().GetWorldMatrix());
}

void Quad::TextBox::OnEvent(Event* event)
{

	const std::string& eventName = event->GetName();

	if (eventName == "LButtonDown")
	{
		int a = 2;
	}
	UiEntity::OnEvent(event);



	if (eventName == "KeyDown")
	{
		HandleKeyDown(event);
	}
	else if (eventName == "MouseMove")
	{
		HandleMouseMove(event);
	}
	else if (eventName == "WindowActive")
	{
		HandleWindowActive(event);
	}
	else if (eventName == "LButtonDown")
	{
		HandleLButtonDown(event);
	}




}

void Quad::TextBox::HandleHover(Event* pEvent)
{

	StateComponent* stateComponent = GetStateComponent();
	MouseEvent* mouseEvent = (MouseEvent*)pEvent;
	
	stateComponent->SetHoverState(mouseEvent->GetHoverState());


	if (mouseEvent->GetHoverState())
	{
		int a = 2;

	//	OutputDebugString(L"TextHover\n");
	}
	else
	{
		//OutputDebugString(L"TextHoverNot\n");

	}




}

void Quad::TextBox::HandleKeyDown(Event* pEvent)
{
	KeyEvent* keyEvent = (KeyEvent*)pEvent;

	if (keyEvent->GetVKFlag())
	{
		int key = keyEvent->GetKey();

		if (key != VK_BACK)
		{
			
			MoveCursorByArrowKey(key);		//방향키 처리
		}
		else
			DeleteCharacter();
		return;
	}


	//입력처리



	UNICODE_VALUE newCharacter = keyEvent->GetCharacter();
	if (!CheckFilterMode(newCharacter))
	{
		//필터링검사먼저 수행
		return;
	}

	//드래그한상태에서 입력이들어오면 드래그한문자들을 먼저제거.
	if (mDragCursorStartPositionInText != -1 && mDragCursorStartPositionInText != mTextCursorEntity->GetPositionInText())
	{//드래그를 하지않았다면 제거하지않는다. 
		DeleteCharacter();
	}

	if (keyEvent->GetHangulFlag())
	{
		//한글처리
		HanguleCharacterProcess(keyEvent);

		return;
	}
	


	
	if (newCharacter == '\r' && mEnterCallback != nullptr)
	{
		mEnterCallback();
		
	}

	
	if (newCharacter == '\r' && GetEnterEndFlag())
	{
		SetTextEditMode(false);
		return;
	}
	
	

	if (mText.size() == mMaxCharacterNum)
		return;

	InsertCharacter(newCharacter);
	

}

Quad::ETextLineMode Quad::TextBox::GetTextLineMode() const
{
	return mTextLineMode;
}

void Quad::TextBox::HandleLButtonDown(Event* pEvent)
{

	//어디클릭했는가?

	
	StateComponent* stateComponent = GetStateComponent();
	bool editMOde = GetTextEditMode();
	bool ho= stateComponent->GetHoverState();
	if(stateComponent->GetHoverState()  &&  GetTextEditMode() == false)
	{
		SetTextEditMode(true);
	}
	else if( !stateComponent->GetHoverState() && GetTextEditMode() == true)
	{

		SetTextEditMode(false);
	}

	if (stateComponent->GetHoverState() == false)
		return;

	stateComponent->SetLButtonDownState(true);

	MouseEvent* mouseEvent = (MouseEvent*)pEvent;
	if (mText.size() != 0)
	{
		Mouse::CaptureObject(this);
		ReSetDragRegion();
	}

	
	SetCursorPosition(mouseEvent);

	//현재 visible한 라인글자들에대해서만 판정하면되지.

	


}

void Quad::TextBox::HandleMouseMove(Event* pEvent)
{


	StateComponent* stateComponent = GetStateComponent();
	

	if (stateComponent->GetLButtonDownState())
	{

		

		int dragCursorStartPosition = mDragCursorStartPositionInText;
		int currentCursorPositionInText = mTextCursorEntity->GetPositionInText();
		if (dragCursorStartPosition == -1)//드래그 시작이라면
		{
			mDragCursorStartPositionInText = mTextCursorEntity->GetPositionInText();
			dragCursorStartPosition = mDragCursorStartPositionInText;
		}
		else
		{
			//드래그중.
			//전프레임에 드래그했던영역을 일단 초기화한다.
			ReSetDragRegion();
			//아에 리셋이니 시작지점은 다시 복구
			mDragCursorStartPositionInText = dragCursorStartPosition;
		
		}
		SetCursorPosition((MouseEvent*)pEvent);
		//HandleLButtonDown(pEvent);


		currentCursorPositionInText = mTextCursorEntity->GetPositionInText();
		OutputDebugString(std::to_wstring(dragCursorStartPosition).c_str());
		OutputDebugString(L"\n");
		OutputDebugString(std::to_wstring(currentCursorPositionInText).c_str());
		OutputDebugString(L"\n");

		if (mDragCursorStartPositionInText == currentCursorPositionInText)
			return;
		

		if (currentCursorPositionInText == 0)
			int a = 2;
		if (dragCursorStartPosition > currentCursorPositionInText)
			std::swap(currentCursorPositionInText, dragCursorStartPosition);


		for (int i = dragCursorStartPosition; i < currentCursorPositionInText; ++i)
		{
			if (mTextCharacterPanelEntityVector[i]->GetCharacter() != '\r')
			{
				mTextCharacterPanelEntityVector[i]->SetDrawFlag(true);
			
			}

		}
		SetVisibleLine();


	}

}

void Quad::TextBox::HandleLButtonUp(Event* pEvent)
{
	StateComponent* stateComponent = GetStateComponent();

	if (stateComponent->GetLButtonDownState())
	{

		//up을했을때 lBUtton상태였고 드래그를 시작했었는데 현재 커서의위치가 동일하다면 리셋해줘야한다
		//리셋해주지않으면 여전히 드래그하는상태로남아있고 나중에 추가로 키를입력하고나서 제거할떄 리셋되않아 -1이 아님으로 리셋되지않은 시작위치에서부터 그떄의 커서의위치까지 드래그가 된상태로 여겨져서 글자가 하나가 삭제되는게아니라 드래그해서제거하는듯이 여러개가 지워지는 문제가있다.

		int currentCursorPositionInText=  mTextCursorEntity->GetPositionInText();
		
		if (currentCursorPositionInText == mDragCursorStartPositionInText)
		{
			mDragCursorStartPositionInText = -1;
		}
	}




	stateComponent->SetLButtonDownState(false);



	//mDragCursorStartPositionInText = -1;
	Mouse::ReleaseCaptureObject(this);

	OutputDebugString(L"ASD");





}

void Quad::TextBox::HandleWindowActive(Event* pEvent)
{

	WindowActiveEvent* activeEvent = (WindowActiveEvent*)pEvent;

	if (activeEvent->GetWindowActiveState())
	{
		
	}
	else
	{
		SetTextEditMode(false);
		KeyBoard::ReleaseCaptureObject(this);
	}
	
	OutputDebugString(L"Active\n");

}

void Quad::TextBox::SetOneLineFlag(bool flag)
{
	mOneLineFlag = flag;
}

bool Quad::TextBox::GetOneLineFlag() const
{
	return mOneLineFlag;
}

void Quad::TextBox::MoveCursorByArrowKey(int key)
{
	ReSetDragRegion();
	int cursorPositionInText = mTextCursorEntity->GetPositionInText();
	switch (key)
	{
	case VK_LEFT:

		if (cursorPositionInText != 0)
			cursorPositionInText--;

		break;

	case VK_RIGHT:

		if (cursorPositionInText < mText.size())
			cursorPositionInText++;

		break;
	case VK_UP:

		//ray 판정을 이용해서 복잡하게 라인을 관리하지않고 처리해보자

		break;

	case VK_DOWN:


		break;
		

	}


	//mTextCursorEntity.SetPositionInText(cursorPositionInText);
	SetCursorPosition(cursorPositionInText);

	SetVisibleLine();

}

void Quad::TextBox::InsertCharacter(UNICODE_VALUE newCharacter)
{
	int cursorPositionInText = mTextCursorEntity->GetPositionInText();
	
	Map* map = GetMap();
	int mapLayerIndex = GetMapLayerID();
	//textPanelVector에 추가
	//TextCharacterPanelEntity* newTextCharacterPanelEntity = TextFactory::GetTextCharacterPanelEntity();
	//TextCharacterPanelEntity* newTextCharacterPanelEntity = static_cast<TextCharacterPanelEntity*>(map->CreateObject("TextCharacterPanelEntity"));
	TextCharacterPanelEntity* newTextCharacterPanelEntity = TextCharacterPanelEntity::Create(map, mapLayerIndex, 20, GetLineHeight());
	//적절한초기화
	//newTextCharacterPanelEntity->Initialize();
	//newTextCharacterPanelEntity->SetName(GetName() + std::to_wstring(newCharacter));
	InitNewCharacter(newTextCharacterPanelEntity, newCharacter);
//	newTextCharacterPanelEntity->SetCh(newCharacter);


	//크기가고정되어있으니 한칸씩 밀어야한다.//글자개수가 max라면 사전에 처리해서 여기까지오지않는다.
	std::copy_backward(mTextCharacterPanelEntityVector.begin() + cursorPositionInText, mTextCharacterPanelEntityVector.begin() + mText.size(),
		mTextCharacterPanelEntityVector.begin() + mText.size() + 1);
	mTextCharacterPanelEntityVector[cursorPositionInText] = newTextCharacterPanelEntity;

	//text string에도 추가
	mText.insert(mText.begin() + cursorPositionInText, newCharacter);


	//rebuild 새로들어온문자와 그 이후문자들의 위치와 크기등을 재조정해야한다.
	ReBuildCharacterMetrics(cursorPositionInText);


	//커서위치 하나 증가
//	mTextCursorEntity.SetPositionInText(cursorPositionInText + 1);
	SetCursorPosition(cursorPositionInText+1);

	SetVisibleLine();


}

void Quad::TextBox::DeleteCharacter()
{
	Map* map = GetMap();

	int cursorPositionInText = mTextCursorEntity->GetPositionInText();

	int start = mDragCursorStartPositionInText;
	int end = cursorPositionInText;

	if (mDragCursorStartPositionInText == -1  ||  mDragCursorStartPositionInText == cursorPositionInText)
	{	
		//드래그를 수행하지않았거나, 수행했는데 시작끝이 동일한경우

		if (cursorPositionInText == 0)
			return;

		start = cursorPositionInText-1;
		 end = cursorPositionInText;
	}
	ReSetDragRegion();


		if (start > end)
			std::swap(start, end);

		for (int i = start; i < end; ++i)
		{

			GetSystem()->RemoveObject(mTextCharacterPanelEntityVector[i]);
			GetSystem()->RemoveObject(mTextCharacterPanelEntityVector[i]->GetCharacterEntity());
			

			map->RequestRemoveObject(mTextCharacterPanelEntityVector[i]);

			//TextFactory::ReleaseTextCharacterPanelEntity(mTextCharacterPanelEntityVector[i]);
			mTextCharacterPanelEntityVector[i] = nullptr;
		}

		//mTextCharacterPanelEntityVector.mTextCharacterPanelEntityVector.begin() + start,mTextCharacterPanelEntityVector.begin()//+end);
		
		std::vector<TextCharacterPanelEntity*>::iterator newEndIt =	std::remove(mTextCharacterPanelEntityVector.begin() + start, mTextCharacterPanelEntityVector.begin() + mText.size() ,nullptr);

		std::for_each(newEndIt, mTextCharacterPanelEntityVector.begin() + mText.size(), [](TextCharacterPanelEntity*& textPanel)
			{
				textPanel = nullptr;
			});
			

		mText.erase(mText.begin() + start, mText.begin() + end);
		//mText.erase(mText.begin() + start ,mText.begin()+end);
		
		
		ReBuildCharacterMetrics(start);
	
		SetCursorPosition(start);
	

	SetVisibleLine();

}

void Quad::TextBox::HanguleCharacterProcess(KeyEvent * keyEvent)
{



	if (keyEvent->GetHangulCompositionFlag())
	{

		//현재 한글 조합중
		//첫시작인지아닌지 확인
		wchar_t ch = keyEvent->GetCharacter();
		if (KeyBoard::GetHangulCompositionStartFlag())
		{
			int a = 2;

			//새로운 characterpanelentity 생성 //영어와동일
			InsertCharacter(ch);

		}
		else
		{
			//현재커서 앞에있는 문자가 바로 조합을 진행중인문자, 그문자부터 다시 rebuild (entity를 새로생성할필요는없음)
			int b = 2;
			
			int cursorPositionInText = mTextCursorEntity->GetPositionInText();
			mText[cursorPositionInText - 1] = ch;
			mTextCharacterPanelEntityVector[cursorPositionInText - 1]->SetCharacter(ch);

			ReBuildCharacterMetrics(cursorPositionInText - 1);
			
		}
	}else
	{
		//한글조합완료

		
		//처리 안해도될거같고.
		//처리를해줘야한다

		//ex 낫 + ㅓ하면 나서 가는데 , 이떄 조합완료메세지로'나' 가 들어오고 조합진행중인 메세지로'서'가들어온다.
		wchar_t ch = keyEvent->GetCharacter();
		int cursorPositionInText = mTextCursorEntity->GetPositionInText();
		mText[cursorPositionInText - 1] = ch;
		mTextCharacterPanelEntityVector[cursorPositionInText - 1]->SetCharacter(ch);
		ReBuildCharacterMetrics(cursorPositionInText - 1);

	}









}

void Quad::TextBox::InitNewCharacter(TextCharacterPanelEntity* newCharacterPanelEntity, wchar_t ch)
{
	DirectX::XMFLOAT3 textBoxWorldPos  = GetTransform().GetPositionWorld();

	//newCharacterPanelEntity->SetSystem(GetSystem());


	int cursorPositionInText = mTextCursorEntity->GetPositionInText();
	
	//newCharacterPanelEntity->Initialize(20, GetLineHeight());
	newCharacterPanelEntity->SetSelectAvailableFlag(false);
	newCharacterPanelEntity->GetTransform().SetIndependentScaleFlag(true);

	newCharacterPanelEntity->GetCharacterEntity()->SetDrawFlag(true);
	newCharacterPanelEntity->GetCharacterEntity()->SetColor(GetTextColor());
	newCharacterPanelEntity->SetLineNum(0);
	newCharacterPanelEntity->SetCharacter(ch);
	newCharacterPanelEntity->SetStencilRefValue(GetStencilRefValue());

	if (ch == L'\r' || ch == L' ')
	{
		newCharacterPanelEntity->GetCharacterEntity()->SetDrawFlag(false);
	}

	AddChildObject(newCharacterPanelEntity);





}

void Quad::TextBox::ReBuildCharacterPositionLeftAlignment(const GlyphMetrics * glyphMetrics ,int  characterEntityIndex)
{



	//라인정보는 어떻게 업데이트할것인가?

	//현재 라인정보와 character vector,text들의 정보가 동기화되기전이니 복잡한계산보단
	//, 안전하게 변경이 시작된라인에서의 바로 윗라인까지포함해서 라인정보를 갱신하자.
	//그냥 자신이 라인정보를 가지도록할까 각 character panel이 ?
	//라인넘어가면 바로옆panel라인넘버에서 1증가시킨값만 저장하면되는거니깐. 초기화는 다 0으로초기화하면되고.
	//좋은데  바로바로 새롭게계산할때 바로옆에 문자의 라인넘버를 가져오면되니깐.(엔터는 다음라인으로 넘어가지않는다) 


	float currentCharacterAdvance = glyphMetrics->mHoriAdvance;

	ETextLineMode textLineMode = GetTextLineMode();
	

	//float currentCharacterAdvance = glyphSlot->advance.x / 64;
	//위치값조정 glyph 이미지의 크기값 설정
	if (characterEntityIndex != 0)
	{
		int preCharacterEntityIndex = characterEntityIndex - 1;

		wchar_t preCharacter = mTextCharacterPanelEntityVector[preCharacterEntityIndex]->GetCharacter();
		DirectX::XMFLOAT3 beforeCharacterPanelLocalPosition = mTextCharacterPanelEntityVector[preCharacterEntityIndex]->GetTransform().GetPositionLocal();

		float advance = mTextCharacterPanelEntityVector[preCharacterEntityIndex]->GetAdvanceX();
		int preCharacterPanelLineNum = mTextCharacterPanelEntityVector[preCharacterEntityIndex]->GetLineNum();

		beforeCharacterPanelLocalPosition.x += advance / 2; //시작지점
			//이전 문자판넬의 위치를 얻고 advance를 얻어서 설정. 


			//멀티라인모드인가?
			//이전글자가 엔터인가?
			//textbox경계를 넘어가는가?
		if (textLineMode == ETextLineMode::eMultiLine  && (beforeCharacterPanelLocalPosition.x + currentCharacterAdvance > (GetWidth() / 2) || preCharacter == L'\r'))
		{
			//라인이 넘어갔음으로 
			//밑줄시작으로
			beforeCharacterPanelLocalPosition.x = mCharacterPanelStartLocalPosX + currentCharacterAdvance / 2;
			beforeCharacterPanelLocalPosition.y -= GetLineHeight();
			mTextCharacterPanelEntityVector[characterEntityIndex]->SetLineNum(preCharacterPanelLineNum + 1);
		}
		else
		{
			//안넘어갔으니 옆으로
			beforeCharacterPanelLocalPosition.x += currentCharacterAdvance / 2;
			mTextCharacterPanelEntityVector[characterEntityIndex]->SetLineNum(preCharacterPanelLineNum);
		}

		mTextCharacterPanelEntityVector[characterEntityIndex]->SetPosition(beforeCharacterPanelLocalPosition.x,
			beforeCharacterPanelLocalPosition.y, beforeCharacterPanelLocalPosition.z);



	}
	else
	{
		mTextCharacterPanelEntityVector[characterEntityIndex]->SetPosition(mCharacterPanelStartLocalPosX + currentCharacterAdvance / 2, mCharacterPanelStartLocalPosY - GetLineHeight() / 2, -1.0F);

		mTextCharacterPanelEntityVector[characterEntityIndex]->SetLineNum(0);


	}





}

void Quad::TextBox::CenterAlignment(int characterEntityIndex)
{

	//delete 하는경우 마지막문자를 제거하는경우나( 범위에벗어남).
	//중간문자를 delete했지만 다음문자가 여전히 다음라인에있어서 그 이전라인의 재정렬이 일어나지못하는경우를 생각해야하니 
	//index를 한번뺸다 .

	if (mText.size() == 0)
		return;

	if(characterEntityIndex!=0)
		characterEntityIndex -= 1;
	
		


	int lineNum = mTextCharacterPanelEntityVector[characterEntityIndex]->GetLineNum();


	int firstCharacterEntityIndex = -1;
	int lastCharacterEntityIndex = -1;

	std::vector<TextCharacterPanelEntity*>::iterator firstIt = std::find_if(mTextCharacterPanelEntityVector.begin(), mTextCharacterPanelEntityVector.end(),
		[lineNum](TextCharacterPanelEntity* panel) {return panel->GetLineNum() == lineNum ? true : false; });

	float panelWidthSum = 0;


	//한라인을 정렬하는 람다.
	auto lineAlignLamda = []
	(std::vector<TextCharacterPanelEntity*>::iterator lineFirstIt, std::vector<TextCharacterPanelEntity*>::iterator lineEndIt, float panelWidthSum)
	{

		//다시 그 시작부터  그마지막 문자까지 위치 재정렬
		//첫시작문자의 위치를 설정한다.

		//중심에서 너비의 합의 절반만큼 각각 왼쪽오른쪽으로 글자들이 존재한다. 
		DirectX::XMFLOAT3 pos = (*lineFirstIt)->GetTransform().GetPositionLocal();
		pos.x = -1.0f * panelWidthSum / 2 + (*lineFirstIt)->GetWidth() / 2;
		(*lineFirstIt)->GetTransform().SetPositionLocal(pos);

		//나머지문자들은 앞의문자위치로부터 계산한다..
		for (auto lineIt = lineFirstIt + 1; lineIt != lineEndIt; ++lineIt)
		{
			//이전문자의 위치
			pos = (*(lineIt - 1))->GetTransform().GetPositionLocal();
			pos.x += (*(lineIt - 1))->GetWidth() / 2 + (*lineIt)->GetWidth() / 2;

			//현재문자위치설정
			(*lineIt)->GetTransform().SetPositionLocal(pos);
		}
	};







	for (auto it = firstIt; it != mTextCharacterPanelEntityVector.begin()+mText.size(); ++it)
	{
		if ((*it)->GetLineNum() == lineNum)
		{
			//그라인의 모든문자들의 너비합 
			panelWidthSum += (*it)->GetWidth();

		}
		

		//다음라인의 문자로 넘어가거나, 마지막라인처리일경우
		if(((*it)->GetLineNum() != lineNum ))
		{

			lineAlignLamda(firstIt, it, panelWidthSum);

			//다음라인정렬준비
			panelWidthSum = 0;
			panelWidthSum = (*it)->GetWidth();
			firstIt = it;
			lineNum++;
		}

	}

	//마지막라인은 end조건때문에 처리를 못하고빠져나왔다.
	//추가적인처리를수행한다.
	lineAlignLamda(firstIt, mTextCharacterPanelEntityVector.begin() + mText.size(), panelWidthSum);





}

void Quad::TextBox::RightAlignment(int characterEntityIndex)
{
}




void Quad::TextBox::ReBuildCharacterMetrics(int characterOffset)
{

	hb_feature_t features[] = {
	{HB_TAG('l', 'i', 'g', 'a'), 0, 0, (unsigned int)-1}, // "liga" 기능 비활성화
	{HB_TAG('c', 'l', 'i', 'g'), 0, 0, (unsigned int)-1}  // "clig" 기능 비활성화
	};
	
	hb_buffer_add_utf16(mHbBuffer,(uint16_t *) mText.c_str(), -1, characterOffset, mText.size() - characterOffset);
	//hb_buffer_add_utf8(mHbBuffer, (uint16_t*)mText.c_str(), -1, characterOffset, mText.size() - characterOffset);
	hb_shape(mFont->GetHB_Font(),mHbBuffer, features, 2);


	unsigned int glyphCount;
	hb_glyph_info_t* glyph_info = hb_buffer_get_glyph_infos(mHbBuffer, &glyphCount);
	hb_glyph_position_t* glyph_pos = hb_buffer_get_glyph_positions(mHbBuffer, &glyphCount);


	for (int i = 0; i < glyphCount; ++i)
	{
		//hb_shape이후로 얻은것이니 glyph의 index이다.
		uint32_t glyphIndex  =  glyph_info[i].codepoint;
		hb_position_t advanceX = glyph_pos[i].x_advance;

	
		if ((glyphIndex != mFont->GetSpaceGlyphIndex()) && (glyphIndex != mFont->GetEnterGlyphIndex()))
		{

			Texture* glyphTexture = mFont->GetGlyphBitmap(glyphIndex);

			if (glyphTexture == nullptr)
			{

				if (FT_Load_Glyph(mFont->GetFT_Face(), glyphIndex, FT_LOAD_DEFAULT) != 0)
				{
					OutputDebugString(L"load glyph 에러발생 \n");
					assert(0);
				}

				if (mFont->GetFT_Face()->glyph->format != FT_GLYPH_FORMAT_BITMAP)
				{
					if (FT_Render_Glyph(mFont->GetFT_Face()->glyph, FT_RENDER_MODE_NORMAL) != 0)
					{
						OutputDebugString(L"glyph를 bitmap으로 변환하는과정에러 \n");
						assert(0);
					}
				}
				glyphTexture = mFont->CreateGlyphBitmap(mFont->GetFT_Face()->glyph);
				if (glyphTexture == nullptr)
				{
					int a = 2;
				}

				GlyphMetrics glyphMetrics;
				glyphMetrics.mWidth = mFont->GetFT_Face()->glyph->metrics.width / 64;
				glyphMetrics.mHeight = mFont->GetFT_Face()->glyph->metrics.height / 64;
				glyphMetrics.mHoriBearingX = mFont->GetFT_Face()->glyph->metrics.horiBearingX / 64;
				glyphMetrics.mHoriBearingY = mFont->GetFT_Face()->glyph->metrics.horiBearingY / 64;
				glyphMetrics.mHoriAdvance = mFont->GetFT_Face()->glyph->metrics.horiAdvance / 64;
				glyphMetrics.mVertBearingX = mFont->GetFT_Face()->glyph->metrics.vertBearingX / 64;
				glyphMetrics.mVertBearingY = mFont->GetFT_Face()->glyph->metrics.vertBearingY / 64;
				glyphMetrics.mVertAdvance = mFont->GetFT_Face()->glyph->metrics.vertAdvance / 64;

				mFont->AddGlyphMetrics(glyphIndex, glyphMetrics);
			}

			mTextCharacterPanelEntityVector[characterOffset + i]->SetGlyphTexture(glyphTexture);

		}


		//위치와 글리프 크기를 결정한다.
		//FT_GlyphSlot& glyphSlot = mFont->GetFT_Face()->glyph;
		const GlyphMetrics* glyphMetrics = mFont->GetGlyphMetrics(glyphIndex);
		float currentCharacterAdvance = glyphMetrics->mHoriAdvance;

		//일단왼쪽정렬로 텍스트처리
		ReBuildCharacterPositionLeftAlignment(glyphMetrics ,characterOffset+i );
		mTextCharacterPanelEntityVector[characterOffset+i]->SetGlyphMetrics(*glyphMetrics ,currentCharacterAdvance, GetBaseLinePanelCharacterPosY());
	
	}

	//정렬설정에따라 각라인문자들 이동
	switch (GetAlignmentDirection())
	{
	case ETextAlignmentDirection::eCenter:
		CenterAlignment (characterOffset);
	break;
	case ETextAlignmentDirection::eRight:
		RightAlignment(characterOffset);
	break;
	}








	hb_buffer_reset(mHbBuffer);
	hb_buffer_set_direction(mHbBuffer, HB_DIRECTION_LTR);
	hb_buffer_set_script(mHbBuffer, HB_SCRIPT_HANGUL);
	hb_buffer_set_language(mHbBuffer, hb_language_from_string("ko", -1));
}

void Quad::TextBox::SetLineHeight(float height)
{
	mLineHeight = height;

}

float Quad::TextBox::GetLineHeight()
{
	return mLineHeight;
	
}

void Quad::TextBox::SetBaseLinePanelCharacterPosY(float y)
{
	mBaseLinePanelLocalPosY = y;

}

float Quad::TextBox::GetBaseLinePanelCharacterPosY() const
{
	return mBaseLinePanelLocalPosY;
}

void Quad::TextBox::Reset(const std::wstring & newText)
{
	int remainCharacterEntityNum = newText.size();


	//for (int i = remainCharacterEntityNum; i < mText.size(); ++i)
	//{
	//	GetSystem()->RemoveObject(mTextCharacterPanelEntityVector[i]);
	//	GetSystem()->RemoveObject(mTextCharacterPanelEntityVector[i]->GetCharacterEntity());

	//	//제거할때 object-childvector에서는 제거를 안하고있다지금 이게 지금 맵에서만 제거하는건데
	//	//parent->RemoveChildObject()
	//	RemoveChildObject(mTextCharacterPanelEntityVector[i]);

	//	if (mTextCharacterPanelEntityVector[i]->GetTransform().GetDirtyFlag())
	//	{
	//		int a = 2;
	//	}
	//	TextFactory::ReleaseTextCharacterPanelEntity(mTextCharacterPanelEntityVector[i]);
	//}

	//if (newText.size() > mText.size())
	//{
	//	for (int i = 0; i < mText.size(); ++i)
	//	{
	//		mTextCharacterPanelEntityVector[i]->SetCharacter(newText[i]);
	//	}

	//	for (int i = mText.size(); i < remainCharacterEntityNum; ++i)
	//	{
	//		TextCharacterPanelEntity* newTextCharacterPanelEntity = TextFactory::GetTextCharacterPanelEntity();
	//	

	//		InitNewCharacter(newTextCharacterPanelEntity, newText[i]);
	//		mTextCharacterPanelEntityVector[i] = newTextCharacterPanelEntity;

	//	}
	//}
	//else 
	//{
	//	for (int i = 0; i < remainCharacterEntityNum; ++i)
	//	{
	//		mTextCharacterPanelEntityVector[i]->SetCharacter(newText[i]);
	//	}

	//	for (int i = remainCharacterEntityNum; i < mText.size(); ++i)
	//	{
	//		mTextCharacterPanelEntityVector[i] = nullptr;
	//	}
	//}

	//



	Map* map = GetMap();

	int mapLayerIndex = GetMapLayerID();



	if (mText.size() > remainCharacterEntityNum)
	{
		for (int i = remainCharacterEntityNum; i < mText.size(); ++i)
		{
			GetSystem()->RemoveObject(mTextCharacterPanelEntityVector[i]);
			GetSystem()->RemoveObject(mTextCharacterPanelEntityVector[i]->GetCharacterEntity());

			//제거할때 object-childvector에서는 제거를 안하고있다지금 이게 지금 맵에서만 제거하는건데
			//parent->RemoveChildObject()
			RemoveChildObject(mTextCharacterPanelEntityVector[i]);

			map->RequestRemoveObject(mTextCharacterPanelEntityVector[i]);
			//TextFactory::ReleaseTextCharacterPanelEntity(mTextCharacterPanelEntityVector[i]);
		}



		for(int i=remainCharacterEntityNum;  i< mText.size(); ++i)
		{
			mTextCharacterPanelEntityVector[i] = nullptr;
		}


		//mTextCharacterPanelEntityVector.erase(mTextCharacterPanelEntityVector.begin() + remainCharacterEntityNum, mTextCharacterPanelEntityVector.end());

		for (int i = 0; i < newText.size(); ++i)
		{
			mTextCharacterPanelEntityVector[i]->SetCharacter(newText[i]);
			//스페이스바의 경우에는 false로 처리한다.
			if (newText[i] == L' ')
			{
				mTextCharacterPanelEntityVector[i]->GetCharacterEntity()->SetDrawFlag(false);
			}
			else
			{
				mTextCharacterPanelEntityVector[i]->GetCharacterEntity()->SetDrawFlag(true);
			}
		}

	}
	else
	{


		for (int i = 0; i < mText.size(); ++i)
		{
			mTextCharacterPanelEntityVector[i]->SetCharacter(newText[i]);
			if (newText[i] == L' ')
			{
				mTextCharacterPanelEntityVector[i]->GetCharacterEntity()->SetDrawFlag(false);
			}
			else
			{
				mTextCharacterPanelEntityVector[i]->GetCharacterEntity()->SetDrawFlag(true);
			}
		}


		for (int i = mText.size(); i < remainCharacterEntityNum; ++i)
		{
			//TextCharacterPanelEntity* newTextCharacterPanelEntity = TextFactory::GetTextCharacterPanelEntity();
		//	TextCharacterPanelEntity* newTextCharacterPanelEntity = static_cast<TextCharacterPanelEntity*>(map->CreateObject("TextCharacterPanelEntity"));
			TextCharacterPanelEntity* newTextCharacterPanelEntity = TextCharacterPanelEntity::Create(map, mapLayerIndex, 20, GetLineHeight());

			InitNewCharacter(newTextCharacterPanelEntity,newText[i]);

			mTextCharacterPanelEntityVector[i] = newTextCharacterPanelEntity;


			mTextCharacterPanelEntityVector[i]->SetCharacter(newText[i]);
			if (newText[i] == L' ')
			{
				newTextCharacterPanelEntity->GetCharacterEntity()->SetDrawFlag(false);
			}
			else
			{
				newTextCharacterPanelEntity->GetCharacterEntity()->SetDrawFlag(true);
			}

		}
	}
	
	mText = newText;





}

int Quad::TextBox::GetCurrentCursorLineNum() const
{
	int cursorPositionInText = mTextCursorEntity->GetPositionInText();
	return GetLineNum(cursorPositionInText);

}

int Quad::TextBox::GetLineNum(int positionInText) const
{
	
	int lineNum = 0;
	if (mText.size() == 0)
		return lineNum;


	if (mText.size() == positionInText)
	{
		//마지막문자 뒤


		lineNum = mTextCharacterPanelEntityVector[positionInText - 1]->GetLineNum();;
		if (mText[positionInText - 1] == L'\r')
		{		//근데 엔터라면 다음라인
			return lineNum + 1;
		}

		return lineNum;
	}
	else
	{

		return mTextCharacterPanelEntityVector[positionInText]->GetLineNum();
	}

	OutputDebugString(L"커서의 라인넘버계산이 잘못되었다.\n");
	assert(0);
	return -1;

}

void Quad::TextBox::SetCursorPosition(int positionInText)
{


	TextCharacterPanelEntity* panel = nullptr;
	DirectX::XMFLOAT3 cursorPos;
	cursorPos.z = -1.0f;

	//정렬에따라 다르다
	float startPosX = 0;
	switch (GetAlignmentDirection())
	{
	case ETextAlignmentDirection::eLeft:

		startPosX = mCharacterPanelStartLocalPosX;
		break;

	case ETextAlignmentDirection::eCenter:
		startPosX = 0;
		break;

	case ETextAlignmentDirection::eRight:
		startPosX = mCharacterPanelStartLocalPosX + GetWidth() / 2;
		break;
	}







	if (mText.size() == 0)
	{

		//정렬에따라 다르다.
		cursorPos.x = startPosX + mTextCursorEntity->GetWidth()/2;
		cursorPos.y = mCharacterPanelStartLocalPosY - GetLineHeight() / 2;
		mTextCursorEntity->GetTransform().SetPositionLocal(cursorPos);
		mTextCursorEntity->SetPositionInText(positionInText);
		return;
	}



	if (mText.size() == positionInText)
	{
		//마지막 문자의 오른쪽위치의 경우이다. 

		//근데 엔터인경우는 다음라인 시작부분으로
		if (mText[positionInText-1] == L'\r')
		{
			//엔터인경우	
			//다음라인(마지막라인) 시작위치로 이동해야한다.
			cursorPos.x = startPosX;
			cursorPos.y = mTextCharacterPanelEntityVector[positionInText-1]->GetTransform().GetPositionLocal().y - GetLineHeight();
		}
		else
		{
			//엔터가 아닌경우
			panel = mTextCharacterPanelEntityVector[positionInText - 1];
			cursorPos = panel->GetTransform().GetPositionLocal();
			cursorPos.x += panel->GetWidth() / 2;
		}


	}
	else
	{
		//일반적인경우
		//ex 커서가 1인덱스위치라면 , 1번인덱스문자의 위치에서 왼쪽으로 그문자의 절반너비만큼이동하면된다(panel기준이다 , 글리프이미지 기준이아니라) 
		panel = mTextCharacterPanelEntityVector[positionInText];
		cursorPos = panel->GetTransform().GetPositionLocal();
		cursorPos.x -= panel->GetWidth() / 2;
		cursorPos.x += mTextCursorEntity->GetWidth() / 2;//보정; //0일떄 경계안으로들어오록하기위해
	}

	mTextCursorEntity->GetTransform().SetPositionLocal(cursorPos);

	mTextCursorEntity->SetPositionInText(positionInText);




}

void Quad::TextBox::SetVisibleLine()
{


	int cursorLineNum = GetCurrentCursorLineNum();
	ETextLineMode textLineMode = GetTextLineMode();


	if (textLineMode == ETextLineMode::eMultiLine)
	{


		if (mVisibleLineStartIndex <= cursorLineNum && cursorLineNum <= (mVisibleLineStartIndex + mVisibleLineNum - 1))
		{
			//문제없음 
			return;
		}

		//cursor가 범위보다 크면  범위를 그 위치까지 증가시키고 ,
		//작으면 감소시킨다.

		//먼저 위치가 범위보다 큰값인지 확인
		int shiftVisibleLineOffset = cursorLineNum - (mVisibleLineNum + mVisibleLineStartIndex - 1);
		if (cursorLineNum < mVisibleLineStartIndex)
		{
			//커서위치가 범위보다 작다. 
			shiftVisibleLineOffset = cursorLineNum - mVisibleLineStartIndex;
		}


		mVisibleLineStartIndex += shiftVisibleLineOffset;
		DirectX::XMFLOAT3 pos;

		for (int i = 0; i < mText.size(); ++i)
		{

			int lineNum = mTextCharacterPanelEntityVector[i]->GetLineNum();
			pos = mTextCharacterPanelEntityVector[i]->GetTransform().GetPositionLocal();
			pos.y += GetLineHeight() * shiftVisibleLineOffset;

			mTextCharacterPanelEntityVector[i]->GetTransform().SetPositionLocal(pos);

			if (lineNum < mVisibleLineStartIndex || lineNum >(mVisibleLineStartIndex + mVisibleLineNum - 1))
			{

				mTextCharacterPanelEntityVector[i]->GetCharacterEntity()->SetDrawFlag(false);

			}
			else
			{

				wchar_t ch = mTextCharacterPanelEntityVector[i]->GetCharacter();
				if (ch != L'\r' && ch != L' ')
					mTextCharacterPanelEntityVector[i]->GetCharacterEntity()->SetDrawFlag(true);
			}

		}



		//라인들이 이동했으니 같이 이동해야한다.
		SetCursorPosition(mTextCursorEntity->GetPositionInText());
	}
	else if (textLineMode == ETextLineMode::eOneLine)
	{

		float textBoxWidth = GetWidth();

		int cursorPosInText = mTextCursorEntity->GetPositionInText();

		float cursorPosX = mTextCursorEntity->GetTransform().GetPositionLocal().x;
		float cursorWidth = mTextCursorEntity->GetWidth();
		DirectX::XMFLOAT3 characterShift = { 0,0,0 };

		if ((cursorPosX + cursorWidth / 2) > (textBoxWidth / 2))
		{	//커서가 텍스트박스범위를(오른쪽으로) 벗어날떄 . 
			//왼쪽으로 이동시킨다(글자들과 커서를)

			float shiftX = -1.0f * ((cursorPosX + cursorWidth / 2) - (textBoxWidth / 2));
			characterShift.x = shiftX;

	
		}
		else if ((cursorPosX - cursorWidth / 2) < (-textBoxWidth / 2))
		{



			float shiftX = (-(cursorPosX - cursorWidth / 2) - (textBoxWidth / 2));
			characterShift.x = shiftX;
	
		}
		else
		{

			if (mText.size() == 0)
				return;

			float lastCharacterPosX = mTextCharacterPanelEntityVector[mText.size() - 1]->GetTransform().GetPositionLocal().x;
			float lastCharacterWidth = mTextCharacterPanelEntityVector[mText.size() - 1]->GetWidth();
			float firstCharacterPosX = mTextCharacterPanelEntityVector[0]->GetTransform().GetPositionLocal().x;
			float firstCharacterWidth = mTextCharacterPanelEntityVector[0]->GetWidth();

			//더 짧은거리만큼 이동한다.

			float shift = min((textBoxWidth / 2 - (lastCharacterPosX+ lastCharacterWidth/2)), (-textBoxWidth / 2 - (firstCharacterPosX- firstCharacterWidth/2)));
			if (shift < 0)
				shift = 0;

			characterShift.x = shift;
		}


		
		for (int i = 0; i < mText.size(); ++i)
		{
			if(characterShift.x!=0)
				mTextCharacterPanelEntityVector[i]->GetTransform().TranslateLocal(characterShift);
		}
		SetChracterDrawInOneLine();

		SetCursorPosition(mTextCursorEntity->GetPositionInText());








	}

}
void Quad::TextBox::SetCursorPosition(MouseEvent* mouseEvent)
{

	if (mText.size() == 0)
	{
		SetCursorPosition(0);
		return;
	}


	int startLineIndex = mVisibleLineStartIndex;

	std::vector<TextCharacterPanelEntity*>::iterator firstPanelEntityIt = std::find_if(mTextCharacterPanelEntityVector.begin(), mTextCharacterPanelEntityVector.begin() + mText.size(),
		[startLineIndex](const TextCharacterPanelEntity* panelEntity) {

			if (panelEntity->GetLineNum() == startLineIndex)
				return true;
			return false;
		});



	Ray ray = mouseEvent->GetRay();
	float mouseWorldY = ray.GetOrigin().y;//월드 y좌표
	float mouseWorldX = ray.GetOrigin().x;

	//mouse line index계산 //TextLineMode가 OneLineMode인경우에는 mouseLineIndex가 0 으로 고정될것이다.
	float visibleLineY = (*firstPanelEntityIt)->GetTransform().GetPositionWorld().y;
	float lineHeight = GetLineHeight();
	int mouseLineIndex = 0;
	for (int i = 0; i < mVisibleLineNum; ++i)
	{

		if (((visibleLineY + lineHeight / 2) >= mouseWorldY) && ((visibleLineY - lineHeight / 2) < mouseWorldY))
		{
			mouseLineIndex = startLineIndex + i;
			break;
		}
		visibleLineY -= lineHeight;

	}


	//그 mouse line 에 존재하는 첫번째 엔티티를 찾는다.
	std::vector<TextCharacterPanelEntity*>::iterator mouseLineFirstPanelEntityIt = std::find_if(mTextCharacterPanelEntityVector.begin(), mTextCharacterPanelEntityVector.begin() + mText.size(),
		[mouseLineIndex](const TextCharacterPanelEntity* panelEntity) {

			if (panelEntity->GetLineNum() == mouseLineIndex)
				return true;
			return false;
		});

	if (mouseLineFirstPanelEntityIt == mTextCharacterPanelEntityVector.begin()+mText.size())
	{
		//없다 즉 텍스트박스의 가장밑빈공간을클릭한것 
		//그러면 가장마지막문자위치로 커서이동
		SetCursorPosition(mText.size());
		return;

	}





	//그 라인의 엔티티들에대해서 정확한 판정
	std::vector<TextCharacterPanelEntity*>::iterator panelEntityIt = mouseLineFirstPanelEntityIt;


	//그 첫번째 엔티티보다 x좌표가 작다.(왼쪽에있다 ==빈공간클릭 ) 
	//그러면 그 첫번쨰 엔티티앞으로 커서가이동한다.
	//즉 커서의좌표는 그 엔티티의 인덱스값.

	if (((*panelEntityIt)->GetTransform().GetPositionWorld().x - ((*panelEntityIt)->GetWidth() / 2) > mouseWorldX))
	{
		int panelIndex = panelEntityIt - mTextCharacterPanelEntityVector.begin();
		SetCursorPosition(panelIndex);

		return;
	}


	//커서가 라인에있는 문자들위에있는경우
	float t = FLT_MAX;
	ETextLineMode lineMode = GetTextLineMode();
	if (lineMode == ETextLineMode::eMultiLine)
	{
		while (1)
		{


			if (CollisionHelper::Intersect((*panelEntityIt)->GetModel()->GetUiColliderComponent()->GetCollider(), ray, t))
			{
				//이 문자를 클릭했다.

				//그문자의 중심에서 어느쪽(x축)에 치우쳐서 클릭되었는가를 판단해서 (왼쪽이냐 오른쪽이냐) , 커서위치 최종확정 
				int panelIndex = panelEntityIt - mTextCharacterPanelEntityVector.begin();
				float panelWorldX = (*panelEntityIt)->GetTransform().GetPositionWorld().x;
				if (panelWorldX >= mouseWorldX)
				{
					SetCursorPosition(panelIndex);
				}
				else
				{
					SetCursorPosition(panelIndex + 1);
				}
				return;
			}


			panelEntityIt++;
			if ((*panelEntityIt) == nullptr || (*panelEntityIt)->GetLineNum() != mouseLineIndex)
			{
				//마지막문자보다 더 오른쪽에 마우스가 위치한다.
				//마지막문자뒤에 커서가 위치해야한다.
				panelEntityIt--;
				int panelIndex = panelEntityIt - mTextCharacterPanelEntityVector.begin();
				SetCursorPosition(panelIndex + 1);
				return;
			}
		}


	}
	else if (lineMode == ETextLineMode::eOneLine)
	{
		//textLineMode가 oneLine이면 lbutton을 누른상태에서 텍스트박스 밖으로 벗어나도 x만으로 판정하여(충돌하지않아도)
		//커서가 그 문자가있는곳에 위치하도록하는것이 시각적으로 자연스럽다.
		while (1)
		{
			float characterPosX =(*panelEntityIt)->GetTransform().GetPositionWorld().x;
			float characterWidth = (*panelEntityIt)->GetWidth();

			if ((mouseWorldX >= (characterPosX - characterWidth / 2)) && (mouseWorldX < (characterPosX + characterWidth / 2)))
			{

				int panelIndex = panelEntityIt - mTextCharacterPanelEntityVector.begin();
				if (characterPosX >= mouseWorldX)
				{
					SetCursorPosition(panelIndex);
				}
				else
				{
					SetCursorPosition(panelIndex + 1);
				}
				return;
			}


			//if (CollisionHelper::Intersect((*panelEntityIt)->GetCollider(), ray, t))
			//{
			//	//이 문자를 클릭했다.

			//	//그문자의 중심에서 어느쪽(x축)에 치우쳐서 클릭되었는가를 판단해서 (왼쪽이냐 오른쪽이냐) , 커서위치 최종확정 
			//	int panelIndex = panelEntityIt - mTextCharacterPanelEntityVector.begin();
			//	float panelWorldX = (*panelEntityIt)->GetTransform().GetPositionWorld().x;
			//	if (panelWorldX >= mouseWorldX)
			//	{
			//		SetCursorPosition(panelIndex);
			//	}
			//	else
			//	{
			//		SetCursorPosition(panelIndex + 1);
			//	}
			//	return;
			//}


			panelEntityIt++;
			if (panelEntityIt == mTextCharacterPanelEntityVector.end() ||  (*panelEntityIt) == nullptr)
			{
				//마지막문자보다 더 오른쪽에 마우스가 위치한다.
				//마지막문자뒤에 커서가 위치해야한다.
				panelEntityIt--;
				int panelIndex = panelEntityIt - mTextCharacterPanelEntityVector.begin();
				SetCursorPosition(panelIndex + 1);
				return;
			}
		}


	}







}

void Quad::TextBox::ReSetDragRegion()
{

	if (mDragCursorStartPositionInText != -1)
	{

		int beforeCursorPositionInText = mTextCursorEntity->GetPositionInText();
		if (mDragCursorStartPositionInText > beforeCursorPositionInText)
			std::swap(mDragCursorStartPositionInText, beforeCursorPositionInText);

		for (int i = mDragCursorStartPositionInText; i < beforeCursorPositionInText; ++i)
		{
			if (mTextCharacterPanelEntityVector[i]->GetCharacter() != '\r')
				mTextCharacterPanelEntityVector[i]->SetDrawFlag(false);
		}

		mDragCursorStartPositionInText = -1;//드래그 시작지점 초기화
	}
}

bool Quad::TextBox::CheckFilterMode(UNICODE_VALUE newCharacter)
{

	ETextFilterMode filterMode = GetTextFilterMode();

	switch (filterMode)
	{
	case ETextFilterMode::eNone:


		break;
	case ETextFilterMode::eAlphabet:

		//알파벳을 필터링한다(알파벳일경우 통과하지못한다)

		return !(std::iswalpha(newCharacter));
	case ETextFilterMode::eDigit:
		//숫자인경우 통과하지못한다.

		return !(std::iswdigit(newCharacter));
	}

	return true; 




	return false;
}

void Quad::TextBox::SetChracterDrawInOneLine()
{


	std::for_each(mTextCharacterPanelEntityVector.begin(), mTextCharacterPanelEntityVector.begin() + mText.size(),
		[textBoxWidth = GetWidth()](TextCharacterPanelEntity* panel) {

			wchar_t ch = panel->GetCharacter();
			float chPosX = panel->GetTransform().GetPositionLocal().x;
			float chWidth = panel->GetWidth();
			if ((ch != L' ') && ((chPosX + chWidth / 2) >= -textBoxWidth / 2) && ((chPosX - chWidth / 2) <= textBoxWidth / 2))
			{
				panel->GetCharacterEntity()->SetDrawFlag(true);
				//panel->SetDrawFlag(true);
			}
			else
			{
				panel->GetCharacterEntity()->SetDrawFlag(false);
				panel->SetDrawFlag(false);
			}


		});


	//for (int i = 0; i < mText.size(); ++i)
	//{
	//	if (characterShift.x != 0)
	//		mTextCharacterPanelEntityVector[i]->GetTransform().TranslateLocal(characterShift);

	//	float characterPosX = mTextCharacterPanelEntityVector[i]->GetTransform().GetPositionLocal().x;
	//	float characterWidth = mTextCharacterPanelEntityVector[i]->GetWidth();

	//	if ((characterPosX + characterWidth / 2) >= -textBoxWidth / 2 && (characterPosX - characterWidth / 2) <= textBoxWidth / 2)
	//	{
	//		mTextCharacterPanelEntityVector[i]->GetCharacterEntity()->SetDrawFlag(true);
	//	}
	//	else
	//	{
	//		mTextCharacterPanelEntityVector[i]->GetCharacterEntity()->SetDrawFlag(false);
	//		mTextCharacterPanelEntityVector[i]->SetDrawFlag(false);

	//	}

	//}




}

void Quad::TextBox::SetOnlyEnglishFlag(bool flag)
{
	mOnlyEnglishFlag = flag;

}

bool Quad::TextBox::GetOnlyEnglishFlag() const
{
	return mOnlyEnglishFlag;
}

void Quad::TextBox::SetOnlyUpperCaseFlag(bool flag)
{
	mOnlyUpperCaseFlag = flag;
}

bool Quad::TextBox::GetOnlyUpperCaseFlag() const
{
	return mOnlyUpperCaseFlag;
}

void Quad::TextBox::InitCreating(int maxCharacterNum, ETextLineMode textLineMode, int fontSizeIndex, Font* font)
{
	UiEntity::InitCreating();

	SetTexture("TextBox.png");
	SetEffect("DefaultUi.effect");
	mMaxCharacterNum = maxCharacterNum;
	mTextLineMode = textLineMode;
	if (font == nullptr)
	{
		mFont = TextFactory::GetFont(L"DefaultFont.ttf", fontSizeIndex);
	}
	mTextCharacterPanelEntityVector.resize(maxCharacterNum, nullptr);

	SetLineHeight((mFont->GetAscender() - mFont->GetDescender()) * mLineHeightScaleFactor);

	SetBaseLinePanelCharacterPosY(GetLineHeight() / 2 - mFont->GetAscender() * mLineHeightScaleFactor);

	Map* map = GetMap();
	int mapLayerIndex = GetMapLayerID();

	mTextCursorEntity = TextCursorEntity::Create(map, mapLayerIndex);
	mTextCursorEntity->GetTransform().SetIndependentScaleFlag(true);
	mTextCursorEntity->SetSelectAvailableFlag(false);
	mTextCursorEntity->SetSize(1.0f, GetLineHeight());
	AddChildObject(mTextCursorEntity);
	SetCursorPosition(0);


	mTextCursorEntity->SetDrawFlag(false);
	mTextCursorEntity->SetActiveFlag(false);


	mVisibleLineStartIndex = 0;
	//mVisibleLineNum
}

