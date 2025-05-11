#include "EditorDirector/DragAndDropWindowController.h"
#include"EditorDirector/DragAndDropUiEntity.h"

#include"Object/Camera/Camera.h"
#include"Core/RenderSystem.h"
#include"EditorDirector/DragAndDropWindow.h"
#include"System/DragAndDropSystem.h"

#include"EditorDirector/ContentItemUiEntity.h"
#include"Core/ContentItem.h"

Quad::DragAndDropWindowController::~DragAndDropWindowController()
{
	delete mCamera;
	delete mImageEntity;
	delete mRenderSystem;

}
void Quad::DragAndDropWindowController::Initialize(DragAndDropWindow* dragAndDropWindow, 
	DragAndDropSystem * uiSystem,	RenderSystem * renderSystem)
{
	Initialize();
	mDragAndDropWindow = dragAndDropWindow;
	mUiSystem = uiSystem;
	mRenderSystem = renderSystem;
	
	mContentItemUiEntity = nullptr;
	EndWindow();



}

void Quad::DragAndDropWindowController::Update(float deltaTime)
{

	mUiSystem->Update(deltaTime);


}

void Quad::DragAndDropWindowController::EndUpdate(float deltaTime)
{
	mUiSystem->EndUpdate(deltaTime);
}

void Quad::DragAndDropWindowController::UploadObjectToRenderSystem()
{
	mRenderSystem->PreUpdate();


	//std::vector<Object*> objectVector;
//	mUiSystem->GetEntity(objectVector);

	const std::vector<MapLayer>  mapLayerVector = mUiSystem->GetEntity();

	mRenderSystem->SetMapLayerVector(mapLayerVector, ESystemType::eMainSystem);



	//RenderSettingItem renderSettingItem;
//	mUiSystem->GetRenderSettingItem(renderSettingItem);
//	mRenderSystem->SetRenderSettingItem(renderSettingItem, ESystemType::eUiSystem);

	mRenderSystem->Update();

}

void Quad::DragAndDropWindowController::OnResize(unsigned short clientWidth, unsigned short clientHeight)
{




}

void Quad::DragAndDropWindowController::Draw()
{
	mRenderSystem->Draw();
}

void Quad::DragAndDropWindowController::RunWindow(ContentItem * contentItem )
{


	DragAndDropWindowController* controllerInstance = GetInstance();
	Map* map = controllerInstance->mUiSystem->GetMap();

	controllerInstance->mWindowRunningState = true;
	controllerInstance->mWindowShowState = true;
	if (controllerInstance->mContentItemUiEntity == nullptr)
	{
		//맨처음 어떤엔티티를 드래그할떄
	//	controllerInstance->mContentItemUiEntity = ContentItemUiEntity::Create(controllerInstance->mUiSystem, contentItem,ContentItemUiEntity::EType::eDragAndDropVersion);
		//controllerInstance->mContentItemUiEntity = static_cast<ContentItemUiEntity*>(map->CreateObject("ContentItemUiEntity"));
		controllerInstance->mContentItemUiEntity = ContentItemUiEntity::Create(map, 0, contentItem, ContentItemUiEntity::EType::eDragAndDropVersion);
		//controllerInstance->mContentItemUiEntity->Initialize(contentItem, ContentItemUiEntity::EType::eDragAndDropVersion);
		//controllerInstance->mUiSystem->AddEntity(controllerInstance->mContentItemUiEntity);
	}
	else
	{
		controllerInstance->mContentItemUiEntity->ChangeContentItem(contentItem);

	}



	ShowWindow(controllerInstance->mDragAndDropWindow->GetWindowHandle(), SW_SHOW);
	UpdateWindow(controllerInstance->mDragAndDropWindow->GetWindowHandle());




}

void Quad::DragAndDropWindowController::EndWindow()
{
	DragAndDropWindowController* controllerInstance = GetInstance();
	controllerInstance->mWindowRunningState = false;
	controllerInstance->mWindowShowState = false;
	ShowWindow(controllerInstance->mDragAndDropWindow->GetWindowHandle(), SW_HIDE);
	UpdateWindow(controllerInstance->mDragAndDropWindow->GetWindowHandle());
}

bool Quad::DragAndDropWindowController::GetWindowRunningState() 
{
	DragAndDropWindowController* controllerInstance = GetInstance();
	return controllerInstance->mWindowRunningState;
}

bool Quad::DragAndDropWindowController::GetWindowShowState()
{
	DragAndDropWindowController* controllerInstance = GetInstance();
	return controllerInstance->mWindowShowState;
}

void Quad::DragAndDropWindowController::SetShowState(bool state)
{
	DragAndDropWindowController* controllerInstance = GetInstance();
	controllerInstance->mWindowShowState = state;

	if (state)
	{
		ShowWindow(controllerInstance->mDragAndDropWindow->GetWindowHandle(), SW_SHOW);
	}
	else
	{
		ShowWindow(controllerInstance->mDragAndDropWindow->GetWindowHandle(), SW_HIDE);
	}

}

Quad::BaseWindow* Quad::DragAndDropWindowController::GetWindow()
{
	DragAndDropWindowController* controllerInstance = GetInstance();
	return controllerInstance->mDragAndDropWindow;
	
}

void Quad::DragAndDropWindowController::SetContentItemUiEntity(ContentItemUiEntity* contentItemUiEntity)
{
	DragAndDropWindowController* controllerInstance = GetInstance();

	//controllerInstance



}



void Quad::DragAndDropWindowController::Initialize()
{
	Controller::Initialize();
}

