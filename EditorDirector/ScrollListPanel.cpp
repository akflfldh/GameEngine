#include "ScrollListPanel.h"
#include"ScrollBarEntity.h"

#include"MouseEvent.h"
#include"Collision/CollisionHelper.h"
#include"Map/Map.h"
int Quad::ScrollListPanel::mStencilValue = 1;

Quad::ScrollListPanel::ScrollListPanel(const std::string& name)
	:PanelUiEntity(name), mCurrentChildPanelNum(0), mActiveChildPanelHeightSum(0), mChildPanelDeltaShiftY(0), mCurrentSelectChildPanelIndex(0),mScrollBarPanel(nullptr)
{
}

Quad::ScrollListPanel::~ScrollListPanel()
{
}

void Quad::ScrollListPanel::Initialize()
{
	PanelUiEntity::Initialize();

	RegisterAcceptEvent("Wheel");

}

Quad::ScrollListPanel* Quad::ScrollListPanel::Create(Map* map, int mapLayerIndex, float width, float height, int maxNum)
{

	ScrollListPanel* ob = static_cast<ScrollListPanel*>(map->CreateObject("ScrollListPanel", mapLayerIndex));
	//ob->Initialize();
	ob->InitCreating(width, height, maxNum);


	return ob;
}

void Quad::ScrollListPanel::Update(float deltaTime)
{




}

void Quad::ScrollListPanel::OnEvent(Event* event)
{
	PanelUiEntity::OnEvent(event);

	const std::string& eventName = event->GetName();

	if (eventName == "Wheel")
	{
		HandleWheel(event);
	}








}

void Quad::ScrollListPanel::AddChildPanelUiEntity(UiEntity* childPanel)
{
	PanelUiEntity::AddChildPanelUiEntity(childPanel);

	childPanel->SetEffect("DefaultUi.effect");

	childPanel->GetTransform().SetIndependentScaleFlag(true);
	childPanel->GetTransform().SetIndependentRotationFlag(true);

	float height =childPanel->GetHeight();
	childPanel->SetSize(GetWidth() - mScrollBarPanel->GetWidth(), height);
	childPanel->SetStencilRefValue(GetStencilRefValue());

	//0번은 scrollbar
	DirectX::XMFLOAT3 pos;
	pos.z = -1;
	pos.x = -mScrollBarPanel->GetWidth()/2;
	int childPanelNum = GetChildPanelUiEntityVector().size();
	if (childPanelNum == 2)
	{
		//첫 자식판넬이다. - > 판넬 맨위 
		pos.y = GetHeight() / 2-childPanel->GetHeight() / 2;

	}
	else
	{
		//마지막판넬 밑으로
		pos = GetChildPanelUiEntityVector()[childPanelNum - 2]->GetTransform().GetPositionLocal();
		pos.y -= (GetChildPanelUiEntityVector()[childPanelNum - 2]->GetHeight() / 2 + childPanel->GetHeight() / 2);
	
	}

	childPanel->GetTransform().SetPositionLocal(pos);

	//자식판넬의 사이즈를 조정, 위치조정 
	//effect설정(스텐실버퍼를사용)

	mCurrentChildPanelNum++;


}

void Quad::ScrollListPanel::SetSelectedObject(Object* object)
{
	mSelectedObject = object;
}

Quad::Object* Quad::ScrollListPanel::GetSelectedObject() const
{
	return mSelectedObject;
}

void Quad::ScrollListPanel::ActivateChildListPanel(int num)
{
	
	//활성화한다 기존 활성화된 판넬들보다 더많이 활성화할땐 새롭게 활성화되는 판넬들은 이전 마지막 활성화된 판넬 밑에 위치하도록 조정된다.
	

	bool scrollPanelDrawFlag = GetDrawFlag();



	mActiveChildPanelHeightSum = 0;

	const std::vector<UiEntity*> & childPanelVector = GetChildPanelUiEntityVector();
	int childPanelNum = childPanelVector.size();
	childPanelNum -= 1;//scrollbar는 제외

	int min = min(childPanelNum, num);
	


	for (int i = 1; i <= min; ++i)
	{
		//활성화
		childPanelVector[i]->SetDrawFlag(scrollPanelDrawFlag);
		childPanelVector[i]->SetActiveFlag(true);
		//active되었으니 트랜스폼이 갱신되어야한다.(초기에 false로 시작한 child들은 위치가 update가 되지않았을것)
		childPanelVector[i]->GetTransform().SetDirtyFlag(true);

		mActiveChildPanelHeightSum += childPanelVector[i]->GetHeight();
	}

	//for (int i = min + 1; i < mChildPanelMaxNum; ++i)
	for (int i = min + 1; i <= mCurrentChildPanelNum; ++i)
	{
		//비활성화
		//보여지지않으면되지.//업데이트도될필요도없다.
		childPanelVector[i]->SetDrawFlag(false);
		childPanelVector[i]->SetActiveFlag(false);
	}



	//자식판넬들이 총이동할수있는 이동값
	float childPanelTotalShiftY = mActiveChildPanelHeightSum - GetHeight();
	if (childPanelTotalShiftY < 0)
		childPanelTotalShiftY = 0.0f;

	
	if (mCurrentChildPanelNum < num)
	{

		//새로활성화된 자식 위치업데이트(이전마지막활성화자식판넬의 밑에위치하도록)(초기값은 설정이올바를수있어도 활성화 -비활성화를
		//반복하면 위치값이 올바르지않은값임으로 설정해주어야한다.
		//mCurrentChildPanelNum+1 부터 num까지의 새로 활성화된 자식판넬들을 연속해서 위치하도록 조정한다.
		DirectX::XMFLOAT3 childPanelPos;
		if (mCurrentChildPanelNum == 0)
		{
			childPanelPos.x = -mScrollBarPanel->GetWidth() / 2;
			childPanelPos.y = GetHeight() / 2;
			childPanelPos.z = -1;
		}
		else
		{
			childPanelPos = childPanelVector[mCurrentChildPanelNum]->GetTransform().GetPositionLocal();
			childPanelPos.y -= childPanelVector[mCurrentChildPanelNum]->GetHeight() / 2;
		}

		for (int i = mCurrentChildPanelNum + 1; i <= num; ++i)
		{
			childPanelPos.y -= childPanelVector[i]->GetHeight() / 2;
			childPanelVector[i]->GetTransform().SetPositionLocal( childPanelPos);
			childPanelPos.y -= childPanelVector[i]->GetHeight() / 2;

		}
		
		//DirectX::XMFLOAT3 lastChildPanelPos = childPanelVector[num - 1]->GetTransform().GetPositionLocal();
		//lastChildPanelPos.y -= (childPanelVector[num - 1]->GetHeight() / 2 + childPanelVector[num]->GetHeight() / 2);
		//childPanelVector[num]->GetTransform().SetPositionLocal(lastChildPanelPos);




		//추가
		//스크롤바를 가장밑으로, 화면도 가장마지막 자식판넬이 보이도록 조정

		//이동해야하는값이 없다는것은 자식의개수가 그 리스트판넬을 벗어나기에 개수나 ,높이값의 합이 충분하지않다는거니 움직일이유가없다.



		//0

		if (childPanelTotalShiftY != 0.0f)
		{

			float scrollBarY = mScrollBarPanel->GetTransform().GetPositionLocal().y;
			//기존자식판넬의 마지막 판넬이 보이도록하기위해 움직여야하는값.
			float childPanelShiftY = -GetHeight() / 2 - (childPanelVector[num]->GetTransform().GetPositionLocal().y- childPanelVector[num]->GetHeight()/2);
			//그값에 새로 추가된(활성화된)판넬의 높이도 추가한다.  
		
			//그값만큼 판넬들이 위로 이동한다.

			DirectX::XMFLOAT3 translationLocal = { 0,childPanelShiftY,0 };
			for (int i = 1; i <= min; ++i)
			{
				childPanelVector[i]->GetTransform().TranslateLocal(translationLocal);
			}
			float aa = scrollBarY + GetHeight() / 2 - mScrollBarPanel->GetHeight() / 2;
			mScrollBarPanel->GetTransform().TranslateLocal({ 0,-(scrollBarY + GetHeight() / 2 - mScrollBarPanel->GetHeight() / 2),0 });

			

		}
	}
	else
	{
		//제거
	}




	mCurrentChildPanelNum = num;










	//스크롤바가 scrollList판넬에서 움직일때 자식판넬들이 움직여야하는 이동비율값
	mChildPanelDeltaShiftY = childPanelTotalShiftY / (GetHeight() - mScrollBarPanel->GetHeight());




	//자식판넬이 선택되어있는경우 갱신
	if (mCurrentSelectChildPanelIndex > mCurrentChildPanelNum)
	{
		mCurrentSelectChildPanelIndex = 0;
	}


}

void Quad::ScrollListPanel::InActiveChildPanel(int index)
{

	std::vector<UiEntity*>& childPanelVector = GetChildPanelUiEntityVector();

	UiEntity* inActiveChild = childPanelVector[index];

	//스크롤바가있으니 같이고려해서 범위 재설정(비활성화되는 판넬은 활성화된 판넬범위 바로 뒤로이동한다(물론어차피 비활성화되니 판넬자체는의미가없어진것) 

	std::copy(childPanelVector.begin() + index + 1, childPanelVector.begin() + mCurrentChildPanelNum+1 , childPanelVector.begin() + index);
	childPanelVector[mCurrentChildPanelNum] = inActiveChild;


	inActiveChild->SetDrawFlag(false);
	inActiveChild->SetActiveFlag(false);
	
	mCurrentChildPanelNum--;
	

	mActiveChildPanelHeightSum -= inActiveChild->GetHeight();
	float childPanelTotalShiftY = mActiveChildPanelHeightSum - GetHeight();
	if (childPanelTotalShiftY < 0)
		childPanelTotalShiftY = 0.0f;

	mChildPanelDeltaShiftY = childPanelTotalShiftY / (GetHeight() - mScrollBarPanel->GetHeight());


	//기존 자식들 위치조정(그 비활성화된 판넬의 높이만큼 위로 이동한다.)(비활성화된 판넬 이후의 자식들에대해)
	DirectX::XMFLOAT3 transtaion = { 0, inActiveChild->GetHeight(),0 };
	for (int i = index; i <= mCurrentChildPanelNum; ++i)
	{
		childPanelVector[i]->GetTransform().TranslateLocal(transtaion);
	}


	//자식판넬들의 위치조정을 하니 , 스크롤판넬 TOP 경계 위로는 보이지않는 자식판넬들이 있는데, 마지막자식판넬이 스크롤판넬 바텀에 딱 붙어있는게 아니라 위치가 더올라간상태라면
	//추가 조정이필요하다.	항상 마지막판넬은 위로올라간다면 스크롤판넬 바텀에까지만 올라가야함으로(물론 자식판넬들의 총 높이합이 스크롤판넬보다작으면 의미없음) 


	//이떄 자식들의 총높이합이 스크롤판넬의 높이보다작으면 이 작업은 할필요가없다.
	if (mActiveChildPanelHeightSum > GetHeight())
	{	//자식판넬들의 총 높이합이 스크롤판넬의 높이보다 클떄만 이작업을 수행한다.
		//활성화된 마지막판넬
		DirectX::XMFLOAT3 lastActiveChildPanelPos = childPanelVector[mCurrentChildPanelNum]->GetTransform().GetPositionLocal();
		float lastActiveChildPanelHeight = childPanelVector[mCurrentChildPanelNum]->GetHeight();
		if ((lastActiveChildPanelPos.y - lastActiveChildPanelHeight / 2) > -GetHeight() / 2)
		{
			//활성화된 마지막자식판넬이 스크롤판넬 바닥에붙어있지않은경우 (더밑에있는경우는상관없다)
			
			//모든 활성화된 자식판넬들을 마지막자식판넬이 바닥에 붙을만큼 이동시킨다.

			float shiftY = -GetHeight() / 2 - (lastActiveChildPanelPos.y - lastActiveChildPanelHeight / 2);
			DirectX::XMFLOAT3 translation = { 0,shiftY,0 };

			for (int i = 1; i <= mCurrentChildPanelNum; ++i)
			{
				childPanelVector[i]->GetTransform().TranslateLocal(translation);
			}

		}

	}


	if (mActiveChildPanelHeightSum > GetHeight())
	{

		//이제 정확히 마지막판넬의 위치가 현재 이동할수있는 거리를 계산하여 현재 scrollbar의 위치를 조정한다.
		DirectX::XMFLOAT3 lastActiveChildPos = childPanelVector[mCurrentChildPanelNum]->GetTransform().GetPositionLocal();

		//자식들이 총이동가능한값
		float childPanelAvailableTotalShiftY = mActiveChildPanelHeightSum - GetHeight();
		//scrollBar가 오른쪽탑 시작위치에서 이동해야하는값.
		//총이동가능한값에서 마지막판넬이 스크롤판넬바텀과의 차이를 뺸것이 현재까지 자식판넬들이 이동한값. 
		//자식판넬이동델타/스크롤바이동델타 의 역수를 곱해줌으로서 스크롤바가 시작위치에서 이동해야하는값을 구해낸다. 
		float scrollBarShiftY =((childPanelAvailableTotalShiftY) -  (-GetHeight() / 2 - (lastActiveChildPos.y - childPanelVector[mCurrentChildPanelNum]->GetHeight() / 2)) )/ mChildPanelDeltaShiftY;


		DirectX::XMFLOAT3 scrollBarPos = mScrollBarPanel->GetTransform().GetPositionLocal();
		scrollBarPos.y = GetHeight() / 2 - mScrollBarPanel->GetHeight() / 2 - scrollBarShiftY;
		mScrollBarPanel->GetTransform().SetPositionLocal(scrollBarPos);

	}
	else
	{// 총 자식판넬의 높이 합이 스크롤판넬높이보다작으면 , 스크롤바는 오르쪽상단에 고정된다.


		DirectX::XMFLOAT3 scrollBarPos = mScrollBarPanel->GetTransform().GetPositionLocal();
		scrollBarPos.y = GetHeight() / 2 - mScrollBarPanel->GetHeight() / 2;
		mScrollBarPanel->GetTransform().SetPositionLocal(scrollBarPos);
		
	}




	//자식판넬이 선택되어있는경우 갱신
	if (mCurrentSelectChildPanelIndex == index)
		mCurrentSelectChildPanelIndex = 0;	//리셋
	else if(mCurrentSelectChildPanelIndex > index)
	{
		mCurrentSelectChildPanelIndex--; //비활성화되는 판넬 뒤에위치했다면 앞으로 한칸씩 이동했을것이니 -1을한다.
	}


}

void Quad::ScrollListPanel::SetDrawFlag(bool flag)
{
	Object::SetDrawFlag(flag);
	//PanelUiEntity::SetDrawFlag(flag);
	const std::vector<UiEntity*> & childPanelVector = GetChildPanelUiEntityVector();

	//childPanelVector[0]->SetDrawFlag(true);


	for (int i = 1; i <= mCurrentChildPanelNum; ++i)
		childPanelVector[i]->SetDrawFlag(flag);
	

	mScrollBarPanel->SetDrawFlag(flag);



}

int Quad::ScrollListPanel::GetActiveChildPanelNum() const
{
	return mCurrentChildPanelNum;
}

void Quad::ScrollListPanel::ResetLScrollBar()
{

	mScrollBarPanel->GetTransform().SetPositionLocal({ GetWidth() / 2 - mScrollBarPanel->GetWidth() / 2 ,GetHeight() / 2 - mScrollBarPanel->GetHeight() / 2 , -1 });

	std::vector<UiEntity*>& childPanelVector = GetChildPanelUiEntityVector();
	float childPanelY = GetHeight() / 2;
	float childPanelWidth = GetWidth() - mScrollBarPanel->GetWidth();
	for (int i = 1; i <= mCurrentChildPanelNum; ++i)
	{
		childPanelY -= childPanelVector[i]->GetHeight() / 2;
		childPanelVector[i]->GetTransform().SetPositionLocal({ -mScrollBarPanel->GetWidth() / 2 , childPanelY,-1 });
		childPanelY -= childPanelVector[i]->GetHeight() / 2;
	}






}

Quad::UiEntity* Quad::ScrollListPanel::GetCurrentSelectedChildPanel() const
{



	const std::vector<UiEntity*>& childPanelVector = GetChildPanelUiEntityVector();


	if (mCurrentSelectChildPanelIndex == 0)
		return nullptr;

	return childPanelVector[mCurrentSelectChildPanelIndex];


}

int Quad::ScrollListPanel::GetCurrentSelectedChildPanelIndex() const
{


	return mCurrentSelectChildPanelIndex;
}

void Quad::ScrollListPanel::InitCreating(float width, float height, int maxNum)
{
	PanelUiEntity::InitCreating();


	mChildPanelMaxNum = maxNum;
	SetSize(width, height);



	// 맨처음스크롤바가 0번인덱스 자식 (고정)
	/*mScrollbar = new ScrollBarEntity(GetName() + L"_Scrollbar");
	mScrollbar->SetSystem(GetSystem());
	mScrollbar->Initialize(this);
	mScrollbar->GetTransform().SetIndependentScaleFlag(true);
	mScrollbar->GetTransform().SetIndependentRotationFlag(true);
	mScrollbar->SetSelectAvailableFlag(false);
	mScrollbar->SetStencilRefValue(mStencilValue);
	mScrollbar->SetSize(20, 20);
	PanelUiEntity::AddChildPanelUiEntity(mScrollbar);*/


	Map* map = GetMap();
	int mapLayerIndex = GetMapLayerID();
	//mScrollBarPanel = new PanelUiEntity(GetName() + "_ScrollBarEntity");
	//mScrollBarPanel = static_cast<PanelUiEntity*>(map->CreateObject("PanelUiEntity"));
	mScrollBarPanel = PanelUiEntity::Create(map, mapLayerIndex);

	//	mScrollBarPanel->SetSystem(GetSystem());
	//mScrollBarPanel->Initialize();
	mScrollBarPanel->SetTexture("Gray.png");
	mScrollBarPanel->GetTransform().SetIndependentScaleFlag(true);
	mScrollBarPanel->GetTransform().SetIndependentRotationFlag(true);
	mScrollBarPanel->SetSelectAvailableFlag(false);
	mScrollBarPanel->SetStencilRefValue(mStencilValue);
	mScrollBarPanel->SetSize(20, 20);
	PanelUiEntity::AddChildPanelUiEntity(mScrollBarPanel);
	mScrollBarPanel->GetTransform().SetPositionLocal({ width / 2 - mScrollBarPanel->GetWidth() / 2,height / 2 - mScrollBarPanel->GetHeight() / 2,-1 });

	std::vector<UiEntity*>& childPanelVector = GetChildPanelUiEntityVector();
	mScrollBarPanel->RegisterEventCallback("MouseMove", [scrollBar = mScrollBarPanel, &childPanelDeltaShiftY = mChildPanelDeltaShiftY, &childPanelVector](Event* pEvent) mutable {

		StateComponent* stateComponent = scrollBar->GetStateComponent();
		if (!stateComponent->GetLButtonDownState())
			return;
		if (childPanelDeltaShiftY == 0)
			return;

		PanelUiEntity* parentPanel = (PanelUiEntity*)scrollBar->GetParentObject();
		float parentPanelHeight = parentPanel->GetHeight();



		MouseEvent* mouseEvent = (MouseEvent*)pEvent;

		float clientShiftY = mouseEvent->GetClientPositionY() - mouseEvent->GetPreClientPositionY();
		DirectX::XMFLOAT3 scrollBarPos = scrollBar->GetTransform().GetPositionLocal();
		float scrollBarHeight = scrollBar->GetHeight();


		scrollBarPos.y -= clientShiftY;

		//스크롤바는 리스트판넬 높이에딱맞게움직여야한다.
		//위치조정
		if ((scrollBarPos.y + scrollBarHeight / 2) > parentPanelHeight / 2)
		{
			//바텀경계를 넘어갈떄
			clientShiftY += ((scrollBarPos.y + scrollBarHeight / 2) - parentPanelHeight / 2);
			scrollBarPos.y = parentPanelHeight / 2 - scrollBarHeight / 2;

		}
		else if ((scrollBarPos.y - scrollBarHeight / 2) < -1.0f * parentPanelHeight / 2)
		{
			//탑경계를 넘어갈때
			clientShiftY += ((scrollBarPos.y - scrollBarHeight / 2) + parentPanelHeight / 2);
			scrollBarPos.y = -parentPanelHeight / 2 + scrollBarHeight / 2;
		}

		scrollBar->GetTransform().SetPositionLocal(scrollBarPos);

		//자식판넬이동
		float childPanelShiftY = childPanelDeltaShiftY * clientShiftY;
		DirectX::XMFLOAT3 translationLocal = { 0,childPanelShiftY,0 };
		for (int i = 1; i < childPanelVector.size(); ++i)
		{
			if (!childPanelVector[i]->GetDrawFlag())	//비활성화되었기때문 draw가 false일것이다.
				break;
			childPanelVector[i]->GetTransform().TranslateLocal(translationLocal);
		}

		});







	SetStencilRefValue(mStencilValue++);
	//추가되는 자식리스트판넬들은 1번부터시작.

	SetEffect("ScrollListPanelUi.effect");

	//mScrollbar->GetTransform().SetPositionLocal({ 0,0,-1 });
	//effect도 scroll list panel을 위한 effect을 사용한다.





}

void Quad::ScrollListPanel::HandleMouseMove(Event* pEvent)
{


	UiEntity* preHoverEntity = (UiEntity*)GetChildHoverObject();


	PanelUiEntity::HandleMouseMove(pEvent);

	UiEntity* currHoverEntity = (UiEntity*)GetChildHoverObject();

	if (currHoverEntity != nullptr)
	{
		int a = 2;
	}

	//if (preHoverEntity == currHoverEntity)
	//{
	//	//아무일도 없다.
	//	return;
	//}

	//if (preHoverEntity != currHoverEntity)
	//{
	//	if (preHoverEntity != nullptr)
	//	{
	//		preHoverEntity->SetColorItensity({ 1.0F,1.0F,1.0F });
	//	}

	//	if (currHoverEntity != nullptr)
	//	{
	//		currHoverEntity->SetColorItensity({ 2.0f,2.0f,2.0f });

	//	}
	//	return;
	//}

}

void Quad::ScrollListPanel::HandleHover(Event* pEvent)
{
	MouseEvent* event = (MouseEvent*)pEvent;
	bool state = event->GetHoverState();
	if (state == false)
	{
		UiEntity* preHoverEntity = (UiEntity*)GetChildHoverObject();
		if (preHoverEntity != nullptr)
		preHoverEntity->OnEvent(pEvent);
		
	}

	PanelUiEntity::HandleHover(pEvent);


	
}

void Quad::ScrollListPanel::HandleLButtonDown(Event* pEvent)
{

	StateComponent* stateComponent = GetStateComponent();
	const std::vector<UiEntity*>& childPanelVector = GetChildPanelUiEntityVector();



	Object* object = GetChildHoverObject();
	if (object != nullptr)
	{
		mCurrentSelectChildPanelIndex = 0;

		std::vector<UiEntity*> ::const_iterator it = std::find(childPanelVector.cbegin() + 1, childPanelVector.cbegin() + mCurrentChildPanelNum + 1,
			object);

		if(it != (childPanelVector.cbegin()+ mCurrentChildPanelNum+1))
		mCurrentSelectChildPanelIndex = it - childPanelVector.cbegin();
	}




	auto callback = GetEventCallback("LButtonDown");
	if (callback != nullptr)
		(*callback)(pEvent);

	if (stateComponent->GetHoverState())
	{
		stateComponent->SetLButtonDownState(true);
	}





	for (int i = 0; i < childPanelVector.size(); ++i)
		childPanelVector[i]->OnEvent(pEvent);





	



}

void Quad::ScrollListPanel::HandleWheel(Event* pEvent)
{

	if (!GetStateComponent()->GetHoverState())
		return;

	MouseEvent* mouseEvent = (MouseEvent*)pEvent;


	int wheelShift = -mouseEvent->GetWheelShift();

	mScrollBarPanel->GetStateComponent()->SetLButtonDownState(true);

	wheelShift /= 120; //wheelShift값은 물리적으로 한번움직이면 120, -120이 입력되니 . 120으로 나눈다.
	MouseEvent mouseMoveEvent("MouseMove");
	int wheelShiftFactor = 5; 

	mouseMoveEvent.SetClientPosition(0, wheelShift * wheelShiftFactor);
	mouseMoveEvent.SetPreClientPosition(0, 0);
	mScrollBarPanel->OnEvent(&mouseMoveEvent);

	mScrollBarPanel->GetStateComponent()->SetLButtonDownState(false);







}

