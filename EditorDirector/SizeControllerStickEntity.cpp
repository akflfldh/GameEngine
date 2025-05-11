#include "EditorDirector/SizeControllerStickEntity.h"

#include"Core/System.h"
#include"ResourceManager/MeshManager.h"
#include"ResourceManager/TextureManager/TextureManager.h"
#include"System/WindowDockingSystem.h"
#include"Core/MouseEvent.h"
#include"Event/EventTable.h"
#include"Core/MouseEvent.h"
#include"Core/Mouse.h"
#include"EditorDirector/Window.h"
#include"EditorDirector/FrameWindowController.h"
#include"Map/Map.h"


Quad::SizeControllerStickEntity::SizeControllerStickEntity(const std::string& name)
	:UiEntity(name)
{
}

void Quad::SizeControllerStickEntity::Initialize()
{
	UiEntity::Initialize();
	//MeshComponent* meshComponent = GetModel()->GetMeshComponent();
	//meshComponent->SetMesh(MeshManager::GetMesh("Rectangle"));

	//meshComponent->GetSubMesh()[0].mModelMaterial.SetDiffuseMap(TextureManager::GetTexture("SizeControllerStick.png"));
	//SetWidthHeight(100, 100);

	//meshComponent->GetSubMesh()[0].mModelMaterial.SetEffectName("DefaultUi.effect");
	System* system = GetSystem();
	system->RegisterEntityToEvent("LButtonUp", this);
	system->RegisterEntityToEvent("LButtonDown", this);
	
}

Quad::SizeControllerStickEntity* Quad::SizeControllerStickEntity::Create(Map* map, int mapLayerIndex)
{
	SizeControllerStickEntity* ob = static_cast<SizeControllerStickEntity*>(map->CreateObject("SizeControllerStickEntity", mapLayerIndex));
//	ob->Initialize();
	ob->InitCreating();


	return ob;
}

void Quad::SizeControllerStickEntity::Update(float deltaTime)
{
	UiEntity::Update(deltaTime);
}

void Quad::SizeControllerStickEntity::OnEvent(Event* event)
{

	UiEntity::OnEvent(event);

	const std::string& eventName = event->GetName();
	if (eventName == "Hover")
	{
		//스틱에서 
		//후버되면 드래그화살표 표시 로 바꿔줘야겠지.
		HandleHoverEvent(event);
	}
	else if (eventName == "LButtonDown")
	{
		HandleLButtonDownEvent(event);
	}
	else if (eventName == "LButtonUp")
	{
		HandleLButtonUpEvent(event);
	}
	else if (eventName == "MouseMove")
	{
		HandleMouseMoveEvent(event);


	}



}

void Quad::SizeControllerStickEntity::SetWidthHeight(float width, float height)
{
	GetTransform().SetScaleLocal({ width,height,1.0f });

}



void Quad::SizeControllerStickEntity::SetPosition(float frameWindowClientPosX, float frameWindowClientPosY)
{	
	//프레임윈도우의 화면에서의 위치,(클라이언트에서의 위치)

	mFrameWindowClientPositionX = frameWindowClientPosX;
	mFrameWindowClientPositionY = frameWindowClientPosY;
	//월드공간과 비교해서 X Y 시작좌표는 중심이아니라, 왼쪽위끝이고  Y좌표축은 거꾸로이다.
	
	//적절한 변환을 해준다.

	//그럼 프레임윈도우의 크기를 알아야한다.
	float frameWindowClientWidth  =GetSystem()->GetClientWidth();
	float frameWindowClientHeight =GetSystem()->GetClientHeight();

	GetTransform().SetPositionLocal({frameWindowClientPosX- frameWindowClientWidth/2.0f , 
		frameWindowClientHeight/2.0f-frameWindowClientPosY,1.0f });

}

float Quad::SizeControllerStickEntity::GetPositionX() const
{
	return mFrameWindowClientPositionX;
}

float Quad::SizeControllerStickEntity::GetPositionY() const
{
	return mFrameWindowClientPositionY;
}

Quad::ESizeControlStickDirection Quad::SizeControllerStickEntity::GetDirection() const
{
	return mDirection;
}

void Quad::SizeControllerStickEntity::SetDirection(ESizeControlStickDirection direction)
{
	 mDirection = direction;
}

void Quad::SizeControllerStickEntity::InitCreating()
{
	UiEntity::InitCreating();

	MeshComponent* meshComponent = GetModel()->GetMeshComponent();
	//meshComponent->SetMesh(MeshManager::GetMesh("Rectangle"));

	meshComponent->GetSubMesh()[0].mModelMaterial.SetDiffuseMap(TextureManager::GetTexture("SizeControllerStick.png"));
	//SetWidthHeight(100, 100);
	SetSize(100, 100);
	meshComponent->GetSubMesh()[0].mModelMaterial.SetEffectName("DefaultUi.effect");
}

void Quad::SizeControllerStickEntity::HandleHoverEvent(Event* pEvent)
{
	
	StateComponent* stateComponent = GetStateComponent();
	MouseEvent* hoverEvent = (MouseEvent*)pEvent;
	
	stateComponent->SetHoverState(hoverEvent->GetHoverState());

	OutputDebugString(L"hover\n");
	if (hoverEvent->GetHoverState())
	{
		//<->화살표모양으로 변경
		//이 스틱이 어떤방향의 스틱인지에따라 위냐 아래냐
		//SetCapture(GetWindow()->GetWindowHandle());

		Mouse::CaptureObject(this);
		stateComponent->SetHoverState(true);


		if (mDirection == ESizeControlStickDirection::eHorizontal)
		{
			Mouse::SetHorizontalArrowCursor();
		}
		else
		{
			Mouse::SetVerticalArrowCursor();
		}

	}
	else
	{
		stateComponent->SetHoverState(false);
		if (!stateComponent->GetLButtonDownState())
		{
			Mouse::SetDefaultCursor();
			Mouse::ReleaseCaptureObject(this);
		}

	}


}

void Quad::SizeControllerStickEntity::HandleLButtonDownEvent(Event* pEvent)
{
	StateComponent* stateComponent = GetStateComponent();

	if (stateComponent->GetHoverState())
	{
	
		stateComponent->SetLButtonDownState(true);
	}




}

void Quad::SizeControllerStickEntity::HandleLButtonUpEvent(Event* pEvent)
{

	StateComponent* stateComponent = GetStateComponent();

	if (stateComponent->GetLButtonDownState())
	{
		stateComponent->SetLButtonDownState(false);

		Mouse::ReleaseCaptureObject(this);
		Mouse::SetDefaultCursor();
	}

	//stateComponent->SetExclusiveHoverEventFlag(false);

//	ReleaseCapture();
	//ReleaseCapture();
	
}

void Quad::SizeControllerStickEntity::HandleMouseMoveEvent(Event* pEvent)
{
	StateComponent* stateComponent = GetStateComponent();
	MouseEvent* mouseEvent = (MouseEvent*)pEvent;
	
	bool arr = Mouse::GetArrowCursorState();

	if (stateComponent->GetLButtonDownState())
	{
		//움직임값과 함께 프레임윈도우(즉 이엔티티가 속한)에게 알려야한다.
		//그럼 프레임윈도우는 그에따라 적절한 처리를수행한다.

		FrameWindowController * frameWindowController =	(FrameWindowController *)GetSystem()->GetController();


		float deltaX=mouseEvent->GetClientPositionX() - mouseEvent->GetPreClientPositionX();
		float deltaY=mouseEvent->GetClientPositionY() - mouseEvent->GetPreClientPositionY();

		//막대를 움직였다!!(실제로는 막대를 누른채로 마우스를움직인거지만)
		frameWindowController->MoveSizeControllerStick(this, deltaX, deltaY);
	}


}




