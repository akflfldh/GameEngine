#include "ContentItemUiPanelEntity.h"

#include"ContentItemUiEntity.h"
#include"WindowResizeEvent.h"
#include"MouseEvent.h"
#include"System.h"

#include"Collision/CollisionHelper.h"
#include"ScrollBarEntity.h"

float Quad::ContentItemUiPanelEntity::mContentItemUiEntityStartOffsetX =50.0f;
float Quad::ContentItemUiPanelEntity::mContentItemUiEntityStartOffsetY = 50.0f;
float Quad::ContentItemUiPanelEntity::mContentItemUiEntityDistance = 50;

Quad::ContentItemUiPanelEntity::ContentItemUiPanelEntity(const std::string& name)
	:PanelUiEntity(name), mScrollBar(nullptr),mColumnIndex(0), mRowIndex(0), mPanelPerScrollBarShiftFactor(0)
{

}

Quad::ContentItemUiPanelEntity::~ContentItemUiPanelEntity()
{
}

void Quad::ContentItemUiPanelEntity::Initialize()
{

	PanelUiEntity::Initialize();

	Map* map = GetMap();




	RegisterAcceptEvent("WindowResizeEvent");
	RegisterAcceptEvent("Wheel");
	RegisterAcceptEvent("LButtonDown");
	RegisterAcceptEvent("LButtonUp");
	RegisterAcceptEvent("MouseMove");
	RegisterAcceptEvent("Hover");
	RegisterAcceptEvent("RbuttonDown");
	RegisterAcceptEvent("RbuttonUp");
	RegisterAcceptEvent("LButtonDownDouble");



}


Quad::ContentItemUiPanelEntity* Quad::ContentItemUiPanelEntity::Create(Map* map, int mapLayerIndex, float width, float height, float startOffsetX, float startOffsetY, float z, float clientWidth, float clientHeight)
{
	ContentItemUiPanelEntity* ob = static_cast<ContentItemUiPanelEntity*>(map->CreateObject("ContentItemUiPanelEntity", mapLayerIndex));

	//ob->Initialize();
	ob->InitCreating(width, height, startOffsetX, startOffsetY, z, clientWidth, clientHeight);;
//	ob->Start();
	return ob;
}

void Quad::ContentItemUiPanelEntity::Start()
{
	PanelUiEntity::Start();


}

void Quad::ContentItemUiPanelEntity::Update(float deltaTime)
{
	PanelUiEntity::Update(deltaTime);

}

void Quad::ContentItemUiPanelEntity::OnEvent(Event* event)
{
	PanelUiEntity::OnEvent(event);

	const std::string& eventName = event->GetName();

	if (eventName == "WindowResizeEvent")
	{
		WindowResizeEvent* resizeEvent = (WindowResizeEvent*)event;
		OnResize(resizeEvent->GetWindowWidth(),resizeEvent->GetWindowHeight());

		//HandleWindowResize(event);
	}
	else if (eventName == "Wheel")
	{
		HandleWheel(event);
	}
	else if (eventName == "LButtonDownDouble") 
	{

		HandleLButtonDownDouble(event);
	}

}

void Quad::ContentItemUiPanelEntity::AddContentItemUiEntity(ContentItemBaseUiEntity* entity)
{

	entity->GetTransform().SetIndependentScaleFlag(true);
	entity->GetTransform().SetIndependentRotationFlag(true);
	entity->SetSelectAvailableFlag(false);
	

	AddChildPanelUiEntity(entity);
	//this->AddChildObject(entity);

	//들어온 아이템의 위치를 계산한다.
	int entityNum  = mContentItemUiEntityVector.size();
	int entityRowIndex = entityNum / (mColumnIndex+1);
	int entityColumnIndex = entityNum % (mColumnIndex+1);
	DirectX::XMFLOAT3 pos;
	CaculateContentItemUiEntityLocalPosition(entityRowIndex, entityColumnIndex, pos);
	entity->GetTransform().SetPositionLocal(pos);

	mRowIndex = entityRowIndex;//갱신
	mContentItemUiEntityVector.push_back(entity);



	AdjustPanelPerScrollBarShiftFactor();

	

	//스크롤바 위치조정까지 

}

void Quad::ContentItemUiPanelEntity::ActivatePanel(bool flag)
{
	GetTransform().SetDirtyFlag(true);
	SetActiveFlag(true);

	SetDrawFlag(flag);
	SetSelectAvailableFlag(flag);

	for (int i = 0; i < mContentItemUiEntityVector.size(); ++i)
	{
		mContentItemUiEntityVector[i]->SetOnOff(flag);
	}
	//mScrollBar->SetDrawFlag(flag);
	//mScrollBarSetSelectAvailableFlag(flag);
	mScrollBar->SetActiveFlag(flag);

}

void Quad::ContentItemUiPanelEntity::OnResize(float clientWidth, float clientHeight)
{
	//시작지점이 움직일수도있겠죠
	mVisibleWidth =	clientWidth - mStartX;
	mVisibleHeight = clientHeight + mStartY;//y offset은 음수일것이다.
	AdjustScrollbarPosition();
	int columnNum = (mVisibleWidth - mContentItemUiEntityStartOffsetX) / (100 + mContentItemUiEntityDistance);
	
	//너무작아져서 한열의 너비도 안된다고 계산되면 1로 고정	
	if (columnNum == 1)
	{
		int a = 2;
	}

	if (columnNum <= 0)
		columnNum = 1;	

	
	if ((mColumnIndex+1) != columnNum)
	{
		mColumnIndex = columnNum-1;
		mRowIndex = mContentItemUiEntityVector.size() / columnNum;
		if (mContentItemUiEntityVector.size() % columnNum == 0)
			mRowIndex -= 1;


		for (int i = 0; i < mContentItemUiEntityVector.size(); ++i)
		{
			int rowIndex = i / columnNum;
			int columnIndex = i % columnNum;
			DirectX::XMFLOAT3 pos;
			CaculateContentItemUiEntityLocalPosition(rowIndex, columnIndex, pos);
			mContentItemUiEntityVector[i]->GetTransform().SetPositionLocal(pos);
		}

	}


	//contentItemUiEntity들이 존재하는 영역의 높이 
	float actualPanelHeight = mContentItemUiEntityDistance + (140 + mContentItemUiEntityDistance) * (mRowIndex+1);
	float mPanelShiftDeltaY =	(actualPanelHeight - mVisibleHeight) / (mVisibleHeight- mScrollBar->GetHeight());
	if (mPanelShiftDeltaY < 0)
		mPanelShiftDeltaY = 0;
	


	DirectX::XMFLOAT3 scrollPos =mScrollBar->GetTransform().GetPositionLocal();
	mScrollBar->SetDefaultPositionLocal({ clientWidth - mScrollBar->GetWidth() / 2 , mStartY -mScrollBar->GetHeight() / 2, scrollPos.z });
	//X
	scrollPos.x = clientWidth - mScrollBar->GetWidth() / 2;
	//Y
	mScrollBar->GetTransform().SetPositionLocal(scrollPos);
	//max값조정
	mScrollBar->SetMaxAvailableShiftValue(mVisibleHeight - mScrollBar->GetHeight());
	
	//shift값도 조정


	AdjustPanelPerScrollBarShiftFactor();
	//scrollBar의 위치도 조정이필요

	AdjustPanelAndScrollBarPosition();
	

}

float Quad::ContentItemUiPanelEntity::GetPanelPerScrollBarShiftFactor() const
{
	return mPanelPerScrollBarShiftFactor;
}

float Quad::ContentItemUiPanelEntity::GetStartX() const
{
	return mStartX;
}

float Quad::ContentItemUiPanelEntity::GetStartY() const
{
	return mStartY;
}

const std::vector<Quad::ContentItemBaseUiEntity*>& Quad::ContentItemUiPanelEntity::GetContentItemBaseUiEntityVector() const
{

	return mContentItemUiEntityVector;
	// TODO: 여기에 return 문을 삽입합니다.
}

void Quad::ContentItemUiPanelEntity::InitCreating(float width, float height, float startOffsetX, float startOffsetY, float z, float clientWidth, float clientHeight)
{
	PanelUiEntity::InitCreating();

	SetTexture("Gray.png");
	SetColorItensity({ 1.0f,1.0f,1.0f });
	SetSize(width, height);
	GetTransform().SetPositionWorld({ startOffsetX + width / 2.0f, -1.0f * height / 2 + startOffsetY ,z });
	//SetColorItensity({ 0.8F,0.8F,0.8F });
	mVisibleWidth = clientWidth - startOffsetX;
	mVisibleHeight = clientHeight + startOffsetY;
	mStartX = startOffsetX;
	mStartY = startOffsetY;


	Map* map = GetMap();
	int mapLayerIndex = GetMapLayerID();


	//mScrollBar = new ScrollBarEntity(GetName() + "_ScrollBar");
	//mScrollBar = static_cast<ScrollBarEntity*>(map->CreateObject("ScrollBarEntity"));
	mScrollBar = ScrollBarEntity::Create(map, mapLayerIndex, EScrollDirection::eVertical, 40, 60, mVisibleHeight - 60);

	//	mScrollBar->SetSystem(GetSystem());
	//mScrollBar->Initialize(EScrollDirection::eVertical, 40, 60, mVisibleHeight - 60);
	mScrollBar->SetTexture("Green.png");
	mScrollBar->GetTransform().SetIndependentScaleFlag(true);
	mScrollBar->GetTransform().SetIndependentTransitionFlag(true);
	mScrollBar->GetTransform().SetIndependentRotationFlag(true);


	mScrollBar->SetDrawFlag(false);
	mScrollBar->SetSelectAvailableFlag(false);
	mScrollBar->SetActiveFlag(false);
	AddChildPanelUiEntity(mScrollBar);


	OnResize(clientWidth, clientHeight);


	DirectX::XMFLOAT3 pos = GetTransform().GetPositionWorld();
	float panelWidth = GetWidth();
	float panelHeight = GetHeight();

	pos.x = pos.x - panelWidth / 2 + mVisibleWidth - mScrollBar->GetWidth() / 2;
	pos.y = pos.y + panelHeight / 2 - mScrollBar->GetHeight() / 2;
	pos.z -= 1;
	mScrollBar->SetDefaultPositionLocal(pos);
	mScrollBar->Reset();
	mScrollBar->SetWheelShiftFactor(10.0f);

	//	mScrollBar->SetW

	mPanelPerScrollBarShiftFactor = 0.0f;

	mScrollBar->RegisterEventCallback("MouseMove", [scrollBar = mScrollBar, panel = this](Event* pEvent) {

		if (scrollBar->GetStateComponent()->GetLButtonDownState())
		{

			float currentScrollBarShiftValue = scrollBar->GetCurrentShiftValue();
			float panelShiftValue = currentScrollBarShiftValue * panel->GetPanelPerScrollBarShiftFactor();

			DirectX::XMFLOAT3 panelPos = panel->GetTransform().GetPositionLocal();
			//panelPos.x = panel->GetStartX() + panel->GetWidth() / 2;
			panelPos.y = panel->GetStartY() - panel->GetHeight() / 2 + panelShiftValue;
			//panelPos.y = panel->GetStartY() + panelShiftValue;

			panel->GetTransform().SetPositionLocal(panelPos);
		}
		});

}

void Quad::ContentItemUiPanelEntity::CaculateContentItemUiEntityLocalPosition(int rowIndex, int colummIndex, DirectX::XMFLOAT3& oPos)
{


	oPos.x = (100 + mContentItemUiEntityDistance) * colummIndex + mContentItemUiEntityStartOffsetX + 50.0f;
	oPos.y = -1.0f * ((140 + mContentItemUiEntityDistance) * rowIndex + mContentItemUiEntityStartOffsetY + 70.0f);

	oPos.x -= GetWidth() / 2;
	oPos.y += GetHeight() / 2;


	oPos.z = -1;



}

void Quad::ContentItemUiPanelEntity::AdjustScrollbarPosition()
{

	//DirectX::XMFLOAT3 pos = GetTransform().GetPositionWorld();
	//float panelWidth = GetWidth();
	//float panelHeight = GetHeight();

	//

	//pos.x = pos.x - panelWidth / 2  + mVisibleWidth - mScrollBar.GetWidth()/2;
	////pos.y = pos.y + panelHeight / 2 - mScrollBar.GetHeight() / 2;
	//pos.y = mScrollBar.GetTransform().GetPositionWorld().y;
	//pos.z -= 1;




	//mScrollBar.GetTransform().SetPositionWorld(pos);


}

void Quad::ContentItemUiPanelEntity::AdjustPanelPerScrollBarShiftFactor()
{
	//판넬이 움직일수있는 최대거리가 변경될수있다(새로운행에 contentUiEntity가 추가된다면)
	//그렇다면 panelPerScrollShiftFactor도 변경된다.


	//startY는 밑으로 내려갔음으로 음수일것이다. 따라서 양수로 바꾼다.
	//contentItemUiEntity가 존재하는 영역의 높이가 (실제 유효한값) 
	float totalVaildPanelHeight = mContentItemUiEntityStartOffsetY + (140 + mContentItemUiEntityDistance) * (mRowIndex + 1);
	//이값에서 visibleHeight값을 뺸 값만큼 panel이 움직일것이다

	float toatalPanelShiftY = totalVaildPanelHeight - mVisibleHeight;
	if (toatalPanelShiftY < 0)
		toatalPanelShiftY = 0.0f;	//contentItem이 적어서 유효한 영역높이가 visibleheight보다  작으면 움직이지않는다.

	mPanelPerScrollBarShiftFactor = toatalPanelShiftY / mScrollBar->GetMaxAvailableShiftValue();


}

void Quad::ContentItemUiPanelEntity::AdjustPanelAndScrollBarPosition()
{


	float totalVaildPanelHeight = mContentItemUiEntityStartOffsetY + (mRowIndex + 1) * (140 + mContentItemUiEntityDistance);


	float panelAvailableShiftValue = totalVaildPanelHeight - mVisibleHeight;
	if (panelAvailableShiftValue < 0)
		panelAvailableShiftValue = 0.0f;

	//마지막 행이 보이는 영역 바텀보다 위에있는가?(즉 마지막 contenItemuiEntity의 위치)
	//위에있으면 내려야한다. // 스크롤바를 가장밑으로 위치시켜야한다(즉 이동값 최대)

	//근데 마지막행이 보이는영역 아래에있다 . 그럼 스크롤바만 위치를 조정해주면된다.

	// 개선 -  그냥 현재 스크롤바의 이동값을 다시같은값으로 재설정한다. 그럼 끝(판넬이 움직일것이다)
	if (mContentItemUiEntityVector.size() == 0)
		return;


	float currentScrollBarShiftValue = mScrollBar->GetCurrentShiftValue();

	mScrollBar->SetShiftValue(currentScrollBarShiftValue);





}

void Quad::ContentItemUiPanelEntity::HandleWheel(Event* pEvent)
{

	StateComponent* stateComponent = GetStateComponent();
	if (stateComponent->GetHoverState())
	{
		mScrollBar->OnEvent(pEvent);
	}

}

void Quad::ContentItemUiPanelEntity::HandleLButtonDownDouble(Event* pEvent)
{
	StateComponent* stateComponent = GetStateComponent();
	if (stateComponent->GetHoverState())
	{
		Object * hoverChildObject = GetChildHoverObject();
		if (hoverChildObject != nullptr)
		{
			hoverChildObject->OnEvent(pEvent);
		}
		


	}

	
}

void Quad::ContentItemUiPanelEntity::HandleWindowResize(Event* pEvent)
{



}

//void Quad::ContentItemUiPanelEntity::HandleLButtonDown(Event* pEvent)
//{
//	StateComponent* stateComponent = GetStateComponent();
//	
//	if (stateComponent->GetHoverState())
//	{
//
//		stateComponent->SetLButtonDownState(true);
//
//
//		for (int i = 0; i < mContentItemUiEntityVector.size(); ++i)
//		{
//			mContentItemUiEntityVector[i]->OnEvent(pEvent);
//		}
//
//	}
//}


//void Quad::ContentItemUiPanelEntity::HandleMouseMove(Event* pEvent)
//{
//	StateComponent* stateComponent = GetStateComponent();
//
//	MouseEvent* mouseEvent = (MouseEvent*)pEvent;
//
//	PanelUiEntity::OnEvent(pEvent);
//	 
//	//if(stateComponent->GetHoverState() || stateComponent->GetLButtonDownState())
//	//{ 
//
//	//	//자식들에대해서 ray판정을 수행
//
//	//	//이미 hover인지아닌지도 판단해서 hover상태 전송
//	//	const Ray  & ray =  mouseEvent->GetRay();
//
//	//	float minT = FLT_MAX;
//	//	int index = -1;
//	//	for (int i = 0; i < mContentItemUiEntityVector.size(); ++i)
//	//	{
//	//		float t = FLT_MAX;
//	//		if (CollisionHelper::Intersect(mContentItemUiEntityVector[i]->GetCollider(), ray, t))
//	//		{
//	//			if (minT > t)
//	//			{
//	//				minT = t;
//	//				index = i;
//	//			}
//
//	//		}
//
//	//	}
//
//	//	if (index != -1)
//	//	{
//	//		if (mContentItemUiEntityVector[index]->GetStateComponent()->GetHoverState() == false)
//	//		{
//	//			//hover event 
//	//			MouseEvent hover("Hover");
//	//			mContentItemUiEntityVector[index]->OnEvent(&hover);
//
//
//	//			if (mHoverChildObject != nullptr)
//	//			{
//	//				hover.SetHoverState(false);
//	//				mHoverChildObject->OnEvent(&hover);
//	//			}
//	//			mHoverChildObject = mContentItemUiEntityVector[index];
//	//		}
//
//	//	}
//	//	else
//	//	{
//
//	//		if (mHoverChildObject != nullptr)
//	//		{
//	//			MouseEvent hover("Hover");
//	//			hover.SetHoverState(false);
//	//			mHoverChildObject->OnEvent(&hover);
//	//			mHoverChildObject = nullptr;
//	//		}
//	//	}
//
//	//	//mouse move
//	//	
//	//	for (int i = 0; i < mContentItemUiEntityVector.size(); ++i)
//	//	{
//	//		mContentItemUiEntityVector[i]->OnEvent(pEvent);
//	//	}
//
//	//	//mContentItemUiEntityVector
//
//
//	//
//	//
//	//}
//
//
//
//}
//


