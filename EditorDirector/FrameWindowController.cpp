#include "EditorDirector/FrameWindowController.h"
#include"EditorDirector/Window.h"
#include"Core/Event.h"
#include"Core/WindowRequestEvent.h"
#include<algorithm>
#include<queue>
#include"EditorDirector/FileUiWindow.h"
#include"EditorDirector/SizeControllerStickEntity.h"
#include"System/WindowChromeSystem.h"
#include"System/FrameWindowDockingSystem.h"
#include"Core/MouseEvent.h"

#include"DropFileHandlerComponent.h"

Quad::TaskWindow* Quad::FrameWindowController::mMovingWindow = nullptr;


Quad::FrameWindowController::FrameWindowController(RenderSystem* renderSystem)
	:DockingWindowController(renderSystem)
{
}

void Quad::FrameWindowController::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device,
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> graphicsCommandList, MeshManager* meshManager, 
	MaterialManager* materialManager, TextureManager* textureManager, MapManager* mapManager, TaskWindow* window,
	 System* gameWorldSystem, 
	WindowChromeSystem* windowLayoutSystem, FrameWindowDockingSystem*frameWindowDockingSystem,
	DescriptorHeapManagerMaster* descriptorHeapManagerMaster)
{

	mFrameWindowDockingSystem = frameWindowDockingSystem;
	DockingWindowController::Initialize(device, graphicsCommandList, meshManager, materialManager, textureManager, mapManager, window,
	 gameWorldSystem, windowLayoutSystem, frameWindowDockingSystem, descriptorHeapManagerMaster);
	mFrameWindowDockingSystem->SetDockingSystemState(true);


	//fraim window는 항상도킹된윈도우
	window->SetMaxClientWidth(10000);
	window->SetMaxClientHeight(10000);
	
	AddMsgHanlderComponent(new DropFileHandlerComponent(this));

}



void Quad::FrameWindowController::Update(float deltaTime)
{
	DockingWindowController::Update(deltaTime);
}

void Quad::FrameWindowController::Draw()
{
	DockingWindowController::Draw();

}

void Quad::FrameWindowController::OnResize(int clientWidth, int clientHeight ,int direction)
{
	
	TaskWindow* frameWindow = GetWindow();
	int preFrameWindowWidth = frameWindow->GetClientWidth();
	int preFrameWindowHeight = frameWindow->GetClientHeight();
	float preFrameWindowLogicalClientWidth = GetLogicalClientWidth();
	float preFrameWindowLogicalClientHeight = GetLogicalClientHeight();
	
	


	float DeltaX = (clientWidth - preFrameWindowWidth);
	float frameLogicalClientWidthRate = mRootWindowNode->mWidth /preFrameWindowWidth;

	float DeltaY = (clientHeight - preFrameWindowHeight);
	float frameLogicalClientHeightRate = mRootWindowNode->mHeight / preFrameWindowHeight;


	DockingWindowController::OnResize(clientWidth, clientHeight, direction);


	float frameWindowLogicalClientWidth = GetLogicalClientWidth();
	float frameWindowLogicalClientHeight = GetLogicalClientHeight();
	int frameWindowPosX = 0;// frameWindow->GetWindowPositionX();
	int frameWindowPosY = 0;// frameWindow->GetWindowPositionY();
	int frameLogicalClientPosX = frameWindowPosX;
	int frameLogicalClientPosY = frameWindowPosY + (clientHeight - frameWindowLogicalClientHeight); //(titlebar영역)



	mRootWindowNode->mWidth = frameWindowLogicalClientWidth-2.0f*margin;
	mRootWindowNode->mHeight = frameWindowLogicalClientHeight -2.0f*margin;
	mRootWindowNode->mPositionX = frameLogicalClientPosX+margin;
	mRootWindowNode->mPositionY = frameLogicalClientPosY+margin;
	mRootWindowNode->mDeltaX = frameLogicalClientWidthRate * DeltaX;
	mRootWindowNode->mDeltaY = frameLogicalClientHeightRate * DeltaY;


	//도킹된(자식창)들의 윈도우위치는 
	//클라이언트영역을기준으로 제공된다.
	// 
	// 
	//프레임윈도우의 사이즈조절이니. 
	//도킹된 자식윈도우의 사이즈도 조절해야한다.
	

	//root부터시작해서 사이즈,위치를 조정한다.
	std::queue<WindowNode*> windowNodeQueue;
	if (mRootWindowNode->mLeftChildWindowNode != nullptr)
	{
		windowNodeQueue.push(mRootWindowNode);
	}
	else
	{
		MoveWindow(mRootWindowNode->mWindow->GetWindowHandle(), mRootWindowNode->mPositionX, mRootWindowNode->mPositionY,
			mRootWindowNode->mWidth, mRootWindowNode->mHeight, true);
		return;
	}
	//루트노드까지는 계산을 사전에 외부에서 처리한다.



	while (!windowNodeQueue.empty())
	{

		WindowNode* windowNode = windowNodeQueue.front();
		windowNodeQueue.pop();

		if (windowNode->mLeftChildWindowNode != nullptr)
		{
			ModifyChildWindowSizePos(windowNode, direction);
			windowNodeQueue.push(windowNode->mLeftChildWindowNode);
			windowNodeQueue.push(windowNode->mRightChildWindowNode);
		}
	}




	
}

//void Quad::FrameWindowController::AddEvent(Event* pEvent)
//{
//	WindowController::AddEvent(pEvent);
//
//}

void Quad::FrameWindowController::InitChildWindowSetting(TaskWindow* gameWindow, TaskWindow * attributeWindow, TaskWindow* fileUiWindow)
{
	
	//TaskWindow* rootWindow = fileUiWindow;

	gameWindow->SetNotDockingMaxClientWidth(gameWindow->GetMaxClientWidth());
	gameWindow->SetNotDockingMaxClientHeight(gameWindow->GetMaxClientHeight());
	gameWindow->SetNotDockingMinClientWidth(gameWindow->GetMinClientWidth());
	gameWindow->SetNotDockingMinClientHeight(gameWindow->GetMinClientHeight());

	gameWindow->SetMaxClientHeight(10000);
	gameWindow->SetMaxClientWidth(10000);
	gameWindow->SetMinClientWidth(0);
	gameWindow->SetMinClientHeight(0);













	TaskWindow* frameWindow = GetWindow();
	HWND frameWindowHandle = frameWindow->GetWindowHandle();
	HWND gameWindowHandle = gameWindow->GetWindowHandle();
	HWND attributeWindowHandle =  attributeWindow->GetWindowHandle();
	HWND fileUiWindowHandle =  fileUiWindow->GetWindowHandle();
	SetParent(gameWindowHandle, frameWindowHandle);
	SetParent(attributeWindowHandle, frameWindowHandle);
	SetParent(fileUiWindowHandle, frameWindowHandle);
	mChildWindowVector.push_back(gameWindow);
	mChildWindowVector.push_back(attributeWindow);
	mChildWindowVector.push_back(fileUiWindow);

	for (int i = 0; i < mChildWindowVector.size(); ++i)
		mChildWindowVector[i]->SetEdgeDragResizeFlag(true);




	mRootWindowNode = new WindowNode;
	mRootWindowNode->mWindow = gameWindow;	//main game playwindow가 루트윈도우가 되어야할거같다.;
	//초기 gameWindow의 크기를 프레임윈도우의 클라이언트영역에 설정해주어야한다.
	//init
	//width,height,positionX,positionY
	mRootWindowNode->mWidth = GetLogicalClientWidth() -2.0f*margin;
	mRootWindowNode->mHeight = GetLogicalClientHeight()-2.0f*margin;
	mRootWindowNode->mPositionX = margin;
	mRootWindowNode->mPositionY =margin+ frameWindow->GetClientHeight() - GetLogicalClientHeight();
	

	MoveWindow(mRootWindowNode->mWindow->GetWindowHandle(), mRootWindowNode->mPositionX, mRootWindowNode->mPositionY,
		mRootWindowNode->mWidth, mRootWindowNode->mHeight, true);

	//rootnode

	//set edge drag state 
	gameWindow->SetEdgeDragResizableDirection(EWindowEdgeDragResizableDirection::eLeft, false);
	gameWindow->SetEdgeDragResizableDirection(EWindowEdgeDragResizableDirection::eRight, false);
	gameWindow->SetEdgeDragResizableDirection(EWindowEdgeDragResizableDirection::eTop, false);
	gameWindow->SetEdgeDragResizableDirection(EWindowEdgeDragResizableDirection::eBottom, false);


	AddChildWindow(attributeWindow, gameWindow, EDockingDirection::eRight);
 	AddChildWindow(fileUiWindow, gameWindow, EDockingDirection::eBottom);

	gameWindow->SetDockingState(true);
	attributeWindow->SetDockingState(true);
	fileUiWindow->SetDockingState(true);



}



void Quad::FrameWindowController::AddChildWindow(TaskWindow* srcWindow, TaskWindow* destWindow, EDockingDirection direction)
{
	//즉 추가한다는것은 도킹한다는것,

	//가장 기본설정
	srcWindow->SetNotDockingMaxClientWidth(srcWindow->GetMaxClientWidth());
	srcWindow->SetNotDockingMaxClientHeight(srcWindow->GetMaxClientHeight());
	srcWindow->SetNotDockingMinClientWidth(srcWindow->GetMinClientWidth());
	srcWindow->SetNotDockingMinClientHeight(srcWindow->GetMinClientHeight());

	srcWindow->SetMaxClientHeight(10000);
	srcWindow->SetMaxClientWidth(10000);
	srcWindow->SetMinClientWidth(0);
	srcWindow->SetMinClientHeight(0);

	srcWindow->SetEdgeDragResizeFlag(false);








	//destWindowNode를 찾는다. ; 
	WindowNode * destWindowNode = FindWindowNode(destWindow);
	if (destWindowNode == nullptr)
		return;

	//새롭게 분할된 WindowNode를생성; 
	WindowNode* splitWindowNodeSrc = new WindowNode;
	WindowNode* splitWindowNodeDest = new WindowNode;
	
	splitWindowNodeSrc->mWindow = srcWindow;
	splitWindowNodeDest->mWindow = destWindow;

	splitWindowNodeSrc->mParentWindowNode = destWindowNode;
	splitWindowNodeDest->mParentWindowNode = destWindowNode;

	//기존 destWindowNode는 영역윈도우노드가된다.
	WindowNode* regionWindowNode = destWindowNode;
	regionWindowNode->mWindow = nullptr;

	//stick을 만들어서 WindowLayoutSystem에 만든다.  내부에서만들어달라고요청하자.(그럼맵에 자동으로 들어갈것)
	SizeControllerStickEntity* sizeControllerStick = mFrameWindowDockingSystem->CreateSizeControlStick();

	//스틱방향
	//영역윈도우노드는 스틱엔티티 포인터를 가져야한다.
	regionWindowNode->mSizeControllerStick = sizeControllerStick;
	


	//direction: srcWindow가 기존 destWindow의 어느방향으로 들어왔는가(도킹되었는가).
	//0 left,1 right, 2 top ,3 bottom
	switch (direction)
	{
	case EDockingDirection::eLeft:	//left
		regionWindowNode->mLeftChildWindowNode = splitWindowNodeSrc;
		regionWindowNode->mRightChildWindowNode = splitWindowNodeDest;
		regionWindowNode->mSizeControllerStick->SetDirection(ESizeControlStickDirection::eVertical);
		break;

	case EDockingDirection::eRight:	//right

		regionWindowNode->mLeftChildWindowNode = splitWindowNodeDest;
		regionWindowNode->mRightChildWindowNode = splitWindowNodeSrc;
		regionWindowNode->mSizeControllerStick->SetDirection(ESizeControlStickDirection::eVertical);
		break;

	case EDockingDirection::eTop: //top
		//left-top ,right -bottom
		regionWindowNode->mLeftChildWindowNode = splitWindowNodeSrc;
		regionWindowNode->mRightChildWindowNode = splitWindowNodeDest;
		regionWindowNode->mSizeControllerStick->SetDirection(ESizeControlStickDirection::eHorizontal);
		break;

	case EDockingDirection::eBottom: //bottom
		regionWindowNode->mLeftChildWindowNode = splitWindowNodeDest;
		regionWindowNode->mRightChildWindowNode = splitWindowNodeSrc;
		regionWindowNode->mSizeControllerStick->SetDirection(ESizeControlStickDirection::eHorizontal);
		break;

	}


	//논리적으로 계층구조를 설계가되었고
	//위치와 크기값조정이필요하다.
	//창들은 영역의크기의 절반으로 조정된다.
	//

	//아직 창의크기가 조정되기전
	int regionWidth = destWindow->GetClientWidth();
	int regionHeight= destWindow->GetClientHeight();
	
	//방향에따라 너비혹은 높이를 절반씩나눈다. 
	//위치를 조정한다.

	float posX= destWindow->GetWindowPositionX();
	float posY= destWindow->GetWindowPositionY();

	switch (direction)
	{
		//margin
	case EDockingDirection::eLeft:	//원래윈도우의 left쪽으로 들어갔다.
		splitWindowNodeSrc->mWidth = (regionWidth - margin) / 2;
		splitWindowNodeSrc->mHeight = regionHeight;
		splitWindowNodeSrc->mPositionX = posX;
		splitWindowNodeSrc->mPositionY = posY;



		splitWindowNodeDest->mWidth = splitWindowNodeSrc->mWidth;
		splitWindowNodeDest->mHeight = splitWindowNodeSrc->mHeight;
		splitWindowNodeDest->mPositionX = posX+ splitWindowNodeSrc->mWidth +margin;
		splitWindowNodeDest->mPositionY = posY;

		//이중간에 margin너비만큼 막대가 크기를가지는거다.
		//stick ->setwidth()
		//stick->setheight()
		//stick의 위치도 지정해야한다. -1 
		//그리고 스틱을 움직이고 그 스틱이움직이면 그 region에있는 두 자식윈도우들의 크기가 수정되고(당연히위치도)
		//그 자손들도 영향을 받도록하는구조
		//스틱은 위치를 옮기면 
		regionWindowNode->mSizeControllerStick->SetWidthHeight(margin, splitWindowNodeSrc->mHeight);
		regionWindowNode->mSizeControllerStick->SetPosition(splitWindowNodeDest->mPositionX - margin / 2.0f,
			splitWindowNodeDest->mPositionY + splitWindowNodeDest->mHeight / 2.0f);






		MoveWindow(srcWindow->GetWindowHandle(), splitWindowNodeSrc->mPositionX, splitWindowNodeSrc->mPositionY
			, splitWindowNodeSrc->mWidth, splitWindowNodeSrc->mHeight, true);


		MoveWindow(destWindow->GetWindowHandle(), splitWindowNodeDest->mPositionX, splitWindowNodeDest->mPositionY,
			splitWindowNodeDest->mWidth, splitWindowNodeDest->mHeight, true);
		break;




	case EDockingDirection::eRight:	//right
		splitWindowNodeDest->mWidth = (regionWidth - margin) / 2;
		splitWindowNodeDest->mHeight = regionHeight;
		splitWindowNodeDest->mPositionX = posX;
		splitWindowNodeDest->mPositionY = posY;

		splitWindowNodeSrc->mWidth = splitWindowNodeDest->mWidth;
		splitWindowNodeSrc->mHeight = splitWindowNodeDest->mHeight;
		splitWindowNodeSrc->mPositionX = posX + splitWindowNodeDest->mWidth + margin;
		splitWindowNodeSrc->mPositionY = posY;

		//stick ->setwidth()
		//stick->setheight()
		regionWindowNode->mSizeControllerStick->SetWidthHeight(margin, splitWindowNodeSrc->mHeight);
	
		regionWindowNode->mSizeControllerStick->SetPosition(splitWindowNodeSrc->mPositionX - margin / 2.0f,
			splitWindowNodeDest->mPositionY + splitWindowNodeDest->mHeight / 2.0f);






		MoveWindow(srcWindow->GetWindowHandle(), splitWindowNodeSrc->mPositionX, splitWindowNodeSrc->mPositionY
			, splitWindowNodeSrc->mWidth, splitWindowNodeSrc->mHeight, true);

		MoveWindow(destWindow->GetWindowHandle(), splitWindowNodeDest->mPositionX, splitWindowNodeDest->mPositionY,
			splitWindowNodeDest->mWidth, splitWindowNodeDest->mHeight, true);

		break;

	case EDockingDirection::eTop: //top
		splitWindowNodeSrc->mWidth = regionWidth;
		splitWindowNodeSrc->mHeight = (regionHeight-margin)/2;
		splitWindowNodeSrc->mPositionX = posX;
		splitWindowNodeSrc->mPositionY = posY;

		splitWindowNodeDest->mWidth = splitWindowNodeSrc->mWidth;
		splitWindowNodeDest->mHeight = splitWindowNodeSrc->mHeight;
		splitWindowNodeDest->mPositionX = splitWindowNodeSrc->mPositionX;
		splitWindowNodeDest->mPositionY = splitWindowNodeSrc->mPositionY + splitWindowNodeSrc->mHeight +margin;


		//stick ->setwidth()
		//stick->setheight()
		regionWindowNode->mSizeControllerStick->SetWidthHeight(splitWindowNodeSrc->mWidth, margin);
		regionWindowNode->mSizeControllerStick->SetPosition(splitWindowNodeDest->mPositionX + splitWindowNodeDest->mWidth/2.0f,
			splitWindowNodeDest->mPositionY - margin/ 2.0f);


		MoveWindow(destWindow->GetWindowHandle(), splitWindowNodeDest->mPositionX, splitWindowNodeDest->mPositionY,
			splitWindowNodeDest->mWidth, splitWindowNodeDest->mHeight, true);

		MoveWindow(srcWindow->GetWindowHandle(), splitWindowNodeSrc->mPositionX, splitWindowNodeSrc->mPositionY
			, splitWindowNodeSrc->mWidth, splitWindowNodeSrc->mHeight, true);


		break;

	case EDockingDirection::eBottom: //bottom
		splitWindowNodeDest->mWidth = regionWidth;
		splitWindowNodeDest->mHeight = (regionHeight - margin) / 2;
		splitWindowNodeDest->mPositionX = posX;
		splitWindowNodeDest->mPositionY = posY;

		splitWindowNodeSrc->mWidth = splitWindowNodeDest->mWidth;
		splitWindowNodeSrc->mHeight = splitWindowNodeDest->mHeight;
		splitWindowNodeSrc->mPositionX = splitWindowNodeDest->mPositionX;
		splitWindowNodeSrc->mPositionY = splitWindowNodeDest->mPositionY + splitWindowNodeDest->mHeight + margin;


		//stick ->setwidth()
		//stick->setheight()
		regionWindowNode->mSizeControllerStick->SetWidthHeight(splitWindowNodeSrc->mWidth, margin);
		regionWindowNode->mSizeControllerStick->SetPosition(splitWindowNodeDest->mPositionX + splitWindowNodeDest->mWidth/2.0f,
			splitWindowNodeSrc->mPositionY-margin/2.0f);


		MoveWindow(destWindow->GetWindowHandle(), splitWindowNodeDest->mPositionX, splitWindowNodeDest->mPositionY,
			splitWindowNodeDest->mWidth, splitWindowNodeDest->mHeight, true);
	

		MoveWindow(srcWindow->GetWindowHandle(), splitWindowNodeSrc->mPositionX, splitWindowNodeSrc->mPositionY
			, splitWindowNodeSrc->mWidth, splitWindowNodeSrc->mHeight, true);
		break;
	}

	srcWindow->SetEdgeDragResizableDirection(EWindowEdgeDragResizableDirection::eLeft, false);
	srcWindow->SetEdgeDragResizableDirection(EWindowEdgeDragResizableDirection::eRight, false);
	srcWindow->SetEdgeDragResizableDirection(EWindowEdgeDragResizableDirection::eTop, false);
	srcWindow->SetEdgeDragResizableDirection(EWindowEdgeDragResizableDirection::eBottom, false);



}

void Quad::FrameWindowController::WindowActivate(int wmActivateState)
{

	switch (wmActivateState)
	{
	case WA_ACTIVE :
	case WA_CLICKACTIVE:

		//for (int i = 0; i < mChildWindowVector.size(); ++i)
		//{
		//	//도킹된윈도우들에대해서 앞으로보이게한다.
		//	TaskWindow* window = mChildWindowVector[i];
		//	if (window->GetDockingState())
		//	{
		//		//BringWindowToTop(window->GetWindowHandle());
		//		SetWindowPos(window->GetWindowHandle(), 0, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		//	}
		//}

		break;
	case WA_INACTIVE:
		//for (int i = 0; i < mChildWindowVector.size(); ++i)
		//{
		//	//도킹된윈도우들에대해서도 함께 비활성화한다.
		//	TaskWindow* window = mChildWindowVector[i];
		//	if (window->GetDockingState())
		//	{
		//		//BringWindowToTop(window->GetWindowHandle());
		//		SetWindowPos(window->GetWindowHandle(), HWND_BOTTOM, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
		//	}
		//}
		break;
	}



}

void Quad::FrameWindowController::WindowMove(int preWindowPosX, int preWindowPosY, 
	int currWindowPosX, int currWindowPosY)
{
	DockingWindowController::WindowMove(preWindowPosX,preWindowPosY,currWindowPosX,currWindowPosY);

	/*int shiftX = currWindowPosX - preWindowPosX;
	int shiftY = currWindowPosY - preWindowPosY;

	for(int i=0; i<mChildWindowVector.size(); ++i)
	{
		TaskWindow* childWindow = mChildWindowVector[i]->mWindow;
		if (childWindow->GetDockingState())
		{
			int preChildWindowPosX = childWindow->GetWindowPositionX();
			int preChildWindowPosY = childWindow->GetWindowPositionY();


			SetWindowPos(childWindow->GetWindowHandle(), HWND_TOPMOST, preChildWindowPosX + shiftX,
				preChildWindowPosY + shiftY, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);

		}

	}*/




}

void Quad::FrameWindowController::HandleWindowReStore()
{
	DockingWindowController::HandleWindowReStore();
	


		SetRenderState(true);
		//ShowWindow(GetWindow()->GetWindowHandle(), SW_SHOW);
		
		for (int childWindowIndex = 0; childWindowIndex < mChildWindowVector.size(); ++childWindowIndex)
		{
			if (mChildWindowVector[childWindowIndex]->GetDockingState())
			{
				mChildWindowVector[childWindowIndex]->GetWindowController()->SetRenderState(true);
				//ShowWindow(mChildWindowVector[childWindowIndex]->GetWindowHandle(), SW_SHOW);
			}
		}
	

}

void Quad::FrameWindowController::WindowMinimize()
{
	EWindowShowState windowShowState = GetWindowShowState();
	//	SetWindowShowState(EWindowShowState::eMinimize);

	SetRenderState(false);
	for (int childWindowIndex = 0; childWindowIndex < mChildWindowVector.size(); ++childWindowIndex)
	{
		if (mChildWindowVector[childWindowIndex]->GetDockingState())
		{
			mChildWindowVector[childWindowIndex]->GetWindowController()->SetRenderState(false);
		}

	}

}


void Quad::FrameWindowController::ReleaseDockingChildWindow(TaskWindow* window)
{
	
	FrameWindowController* frameController = GetInstance();
	char margin = frameController->margin;

	WindowNode* windowNode = frameController->FindWindowNode(window);


	//최상위 루트노드이면 도킹해제할수없다.
	if (frameController->mRootWindowNode == windowNode)
		return;




	//도킹해제되는 윈도우의 위치를포함한 여러가지 설정
	//프레임윈도우는 항상 Not Docking
//자식윈도우들중에 Docking상태이라면
//투명하게,그리고 자식윈도우에 해제


	window->SetMaxClientWidth(window->GetNotDockingMaxClientWidth());
	window->SetMaxClientHeight(window->GetNotDockingMaxClientHeight());
	window->SetMinClientWidth(window->GetNotDockingMinClientWidth());
	window->SetMinClientHeight(window->GetNotDockingMinClientHeight());
	window->SetEdgeDragResizeFlag(true);




	window->SetDockingState(false);

	POINT screenWindowPosition = { (LONG)window->GetWindowPositionX() ,(LONG)window->GetWindowPositionY() };
	//프레임윈도우의 핸들이필요하다.
	ClientToScreen(frameController->GetWindow()->GetWindowHandle(), &screenWindowPosition);

	SetParent(window->GetWindowHandle(), NULL);
	//부모를 변경해서 다시그리게 되는데 윈도우의좌표는 프레임윈도우의 기준으로 가진 좌표를 가지고 
	//스크린공간에 다시그리니 왼쪾상단쪽으로 이동해버리는 문제가있다


	MoveWindow(window->GetWindowHandle(), screenWindowPosition.x, screenWindowPosition.y,
		window->GetClientWidth(), window->GetClientHeight(), TRUE);

	window->SetEdgeDragResizableDirection(EWindowEdgeDragResizableDirection::eLeft, true);
	window->SetEdgeDragResizableDirection(EWindowEdgeDragResizableDirection::eRight, true);
	window->SetEdgeDragResizableDirection(EWindowEdgeDragResizableDirection::eTop, true);
	window->SetEdgeDragResizableDirection(EWindowEdgeDragResizableDirection::eBottom, true);




	//계층구조에서 해당윈도우의 윈도우노드를 제거한다.
	//제거하면서 그 윈도우와 영역을 분할한  옆윈도우 노두가  해당영역을 다차지한다.- >즉 위치와 크기재조정이 일어난다.
	//계층구조에따라 그 밑에 자식노드들도 다 연쇄적으로 처리가 수행됨.



	WindowNode* parentWindowNode = windowNode->mParentWindowNode;

	WindowNode* otherChildWindowNode = parentWindowNode->mLeftChildWindowNode;

	if (windowNode == otherChildWindowNode)
		otherChildWindowNode = parentWindowNode->mRightChildWindowNode;

	float deltaX = 0;
	float deltaY = 0;
	int direction = 0;		//other child window Node 기준의으로의 size조절방향


	//너비와 위치 조정 , 어느방향으로 사이즈를 조절한것과 동일한가?
	if (windowNode->mPositionX != otherChildWindowNode->mPositionX)
	{
		//세로선에의해서 좌우로 분리됨

		//너비는 = w1 + w2 +margin
		//높이는 그대로

		deltaX = windowNode->mWidth + margin;
		otherChildWindowNode->mWidth += deltaX;
	
		//위치는 
		//y는 그대로 x는 작은쪽으로

		direction = 2;
		if (otherChildWindowNode->mPositionX > windowNode->mPositionX)
		{
			otherChildWindowNode->mPositionX = windowNode->mPositionX;
			direction = 1;
		}
		otherChildWindowNode->mDeltaX = deltaX;
		otherChildWindowNode->mDeltaY = 0;
	}
	else
	{
		//가로선에의해서 위아래로 분리됨.
		//너비는 그대로
		//높이는 h1+h2+margin
		deltaY = margin + windowNode->mHeight;
		otherChildWindowNode->mHeight += deltaY;


		//위치는 
		//x는 그대로 y는 작은쪽
		direction = 4;

		if (otherChildWindowNode->mPositionY > windowNode->mPositionY)
		{
			otherChildWindowNode->mPositionY = windowNode->mPositionY;
			direction = 3;
		}

		otherChildWindowNode->mDeltaY = deltaY;
		otherChildWindowNode->mDeltaX = 0;
	}



	//부모자식계층구조를 재구축 
	if (parentWindowNode != frameController->mRootWindowNode)
	{
		if (parentWindowNode->mParentWindowNode->mLeftChildWindowNode == parentWindowNode)
		{
			parentWindowNode->mParentWindowNode->mLeftChildWindowNode = otherChildWindowNode;
		}
		else
		{
			parentWindowNode->mParentWindowNode->mRightChildWindowNode = otherChildWindowNode;
		}
		otherChildWindowNode->mParentWindowNode = parentWindowNode->mParentWindowNode;
	}
	else
	{
		frameController->mRootWindowNode = otherChildWindowNode;
		otherChildWindowNode->mParentWindowNode = nullptr;
	}

	//참고로 빠져나가는 윈도우는 leaf노드이니 문제없고
	//부모노드인 영역노드는 다른자식이 조부모자식으로 수정되었으니 ok

	frameController->mFrameWindowDockingSystem->ReleaseSizeControlStick(parentWindowNode->mSizeControllerStick);
	delete parentWindowNode;
	delete windowNode;
	
	if (otherChildWindowNode->mWindow != nullptr)
	{//otherChildWindowNode가 창을나타낸다면

		MoveWindow(otherChildWindowNode->mWindow->GetWindowHandle(), otherChildWindowNode->mPositionX, otherChildWindowNode->mPositionY,
			otherChildWindowNode->mWidth, otherChildWindowNode->mHeight, true);
		
	}
	else
	{
		//otherChildWindow가 region을 나타내면
		//자식윈도우들에게 효과연쇄전달 

		std::queue<WindowNode*> windowNodeQueue;
		windowNodeQueue.push(otherChildWindowNode);
		while (!windowNodeQueue.empty())
		{

			WindowNode* windowNode = windowNodeQueue.front();
			windowNodeQueue.pop();

			if (windowNode->mLeftChildWindowNode != nullptr)
			{
				frameController->ModifyChildWindowSizePos(windowNode, direction);
				windowNodeQueue.push(windowNode->mLeftChildWindowNode);
				windowNodeQueue.push(windowNode->mRightChildWindowNode);
			}
		}
	}




}

void Quad::FrameWindowController::SetDockingChildWindow(TaskWindow* window, EDockingDirection direction)
{

	FrameWindowController* frameWindowControllerInstance = GetInstance();

	/*if (mMovingWindow == nullptr)
		return;*/
	
	//윈도우 노드 계층구조에 넣는것이다.
	TaskWindow* movingWindow = GetMovingWindow();


	//여기서 moving윈도우하면서 그 처리가안된건가
	
	//add childWindow에서 사이즈조절방향플래그도 설정해야한다.
	frameWindowControllerInstance->AddChildWindow(movingWindow,window, direction);
	SetParent(movingWindow->GetWindowHandle(), frameWindowControllerInstance->GetWindow()->GetWindowHandle());
	movingWindow->SetDockingState(true);


	SetMovingWindow(nullptr);

	//사이즈조절방향이 설정도있어야한다.
	

	//



}




void Quad::FrameWindowController::OnChildWindowDockingSystem(int mouseScreenPositionX, int mouseScreenPositionY)
{
	
	FrameWindowController * frameControllerInstance = GetInstance();

	TaskWindow* movingWindow = GetMovingWindow();
	TaskWindow* frameWindow = frameControllerInstance->GetWindow();

	//도킹이해제된 윈도우끼리는 작동하지않는다. 항상 프레임윈도우안에서만 작동한다.
	//




	POINT mouseFrameWindowClientPosition = { mouseScreenPositionX ,mouseScreenPositionY };
	ScreenToClient(frameWindow->GetWindowHandle(), &mouseFrameWindowClientPosition);

	std::vector<TaskWindow*>& childWindowVector = frameControllerInstance->mChildWindowVector;
	for(int childWindowIndex =0 ;  childWindowIndex < childWindowVector.size(); ++childWindowIndex)
	{ 
		if (movingWindow == childWindowVector[childWindowIndex])
			continue;

		RECT windowRect;
		windowRect.left = childWindowVector[childWindowIndex]->GetWindowPositionX();
		windowRect.top = childWindowVector[childWindowIndex]->GetWindowPositionY();
		windowRect.right = windowRect.left + childWindowVector[childWindowIndex]->GetClientWidth();
		windowRect.bottom = windowRect.top + childWindowVector[childWindowIndex]->GetClientHeight();

		
		DockingWindowController* childWindowController = childWindowVector[childWindowIndex]->GetWindowController();
		if ((windowRect.left < mouseFrameWindowClientPosition.x) && (windowRect.right > mouseFrameWindowClientPosition.x))
		{
			if ((windowRect.top < mouseFrameWindowClientPosition.y) && (windowRect.bottom > mouseFrameWindowClientPosition.y))
			{

				if (frameControllerInstance->FindWindowNode(childWindowVector[childWindowIndex]))
				{
					//이 도킹된자식창의 docking system 실행//이미실행중이라면 
					//떨어져있는 자식창에도 할수있도록 나중에 확장해야할것
					childWindowController->OnDockingSystem(mouseScreenPositionX, mouseScreenPositionY);
					continue;
				}
			}
		}
		if(childWindowController->GetWindowDockingSystemState())
			childWindowVector[childWindowIndex]->GetWindowController()->OffDockingSystem();

	}







}

void Quad::FrameWindowController::OffChildWindowDockingSystem(int screenPositionX, int screenPositionY)
{
	FrameWindowController* frameControllerInstance = GetInstance();

	TaskWindow* movingWindow = GetMovingWindow();
	TaskWindow* frameWindow = frameControllerInstance->GetWindow();

	std::vector<TaskWindow*>& childWindowVector = frameControllerInstance->mChildWindowVector;


	for (int childWindowIndex = 0; childWindowIndex < childWindowVector.size(); ++childWindowIndex)
	{
		DockingWindowController* childWindowController = childWindowVector[childWindowIndex]->GetWindowController();
		if (childWindowController->GetWindowDockingSystemState())
		{
			childWindowController->EndDockingSystem(screenPositionX, screenPositionY);
			break;
		}
	}







}

void Quad::FrameWindowController::SetMovingWindow(TaskWindow* window)
{
	mMovingWindow = window;
}

Quad::TaskWindow* Quad::FrameWindowController::GetMovingWindow() 
{
	return mMovingWindow;
}

void Quad::FrameWindowController::MoveSizeControllerStick(SizeControllerStickEntity* stick, float deltaX, float deltaY)
{

	//해당 막대를 가지는 region window node를 찾아
	// 막대 위치도 당연히 변경
	//그 region부터해서 자식노드들의 위치,크기를 조정해,
	//당연히 그 자식노드들의 막대의 위치와 크기도 변경

	
	std::queue<WindowNode*> windowNodeQueue;
	

	windowNodeQueue.push(mRootWindowNode);
	WindowNode* startWindowNode = nullptr;
	while (!windowNodeQueue.empty())
	{
		startWindowNode=windowNodeQueue.front();
		if (startWindowNode->mSizeControllerStick == stick)
		{
			break;
		}
		windowNodeQueue.pop();

		if (startWindowNode->mLeftChildWindowNode != nullptr)
		{
			windowNodeQueue.push(startWindowNode->mLeftChildWindowNode);
			windowNodeQueue.push(startWindowNode->mRightChildWindowNode);
		}
	}

	if (startWindowNode == nullptr)
	{
		MessageBox(nullptr, L"이 윈도우스틱을 가지는윈도우노드가없다,", L"error", 0);
		assert(0);
	}


	//이막대를 가지는 영역윈도우노드의 크기와 위치는 변하지않는다 자식들이변하지.
	//해당노드의 left,right자식윈도우노드에대해서 크기값,위치값,막대값을게산해주고
	//그다음부터는 각 자식노드들에 델타값이 결정되었느니 resize와 동일하게 자식에게 연쇄적용시킨다.
	
	WindowNode* leftChildWindowNode = startWindowNode->mLeftChildWindowNode;
	WindowNode* rightChildWindowNode = startWindowNode->mRightChildWindowNode;

	switch (stick->GetDirection())
	{
	case ESizeControlStickDirection::eVertical:
	{
		//mouse는 좌우 ,위아래 동시에움직여도 막대 방향에따라 한방향은 델타값이 0이다.
		//프레임윈도우의 클라이언트영역기준임으로
		//증가하면 right로 움직인것이고(즉양수)
		//감소하면 left로 움직인것(음수)

		//left , right	
		leftChildWindowNode->mDeltaX = deltaX;
		leftChildWindowNode->mDeltaY = 0;
		rightChildWindowNode->mDeltaX = -deltaX;
		rightChildWindowNode->mDeltaY = 0;
		
		leftChildWindowNode->mWidth += leftChildWindowNode->mDeltaX;
		rightChildWindowNode->mWidth += rightChildWindowNode->mDeltaX;

		//사이즈조절은 끝났으니 위치

		rightChildWindowNode->mPositionX = leftChildWindowNode->mPositionX + leftChildWindowNode->mWidth+ margin;


	}
	break;
	case ESizeControlStickDirection::eHorizontal:
	{
		//증가하면 bottom으로 움직인것
		//감소하면 top으로 움직인것


		//top bottom
		leftChildWindowNode->mDeltaY = deltaY;
		leftChildWindowNode->mDeltaX = 0;
		rightChildWindowNode->mDeltaY = -deltaY;
		rightChildWindowNode->mDeltaX = 0;

		leftChildWindowNode->mHeight += leftChildWindowNode->mDeltaY;
		rightChildWindowNode->mHeight += rightChildWindowNode->mDeltaY;

		rightChildWindowNode->mPositionY = leftChildWindowNode->mPositionY + leftChildWindowNode->mHeight+ margin;

	}
	break;
	}


	if (leftChildWindowNode->mWindow)
		MoveWindow(leftChildWindowNode->mWindow->GetWindowHandle(), leftChildWindowNode->mPositionX,
			leftChildWindowNode->mPositionY, leftChildWindowNode->mWidth,leftChildWindowNode->mHeight, true);

	if (rightChildWindowNode->mWindow)
		MoveWindow(rightChildWindowNode->mWindow->GetWindowHandle(), rightChildWindowNode->mPositionX,
			rightChildWindowNode->mPositionY, rightChildWindowNode->mWidth, rightChildWindowNode->mHeight, true);


	//움직임을 유발한 막대는 크기가 바뀌지않는다 .막대위치만계산.
	int direction = 0;
	switch (stick->GetDirection())
	{
	
		case ESizeControlStickDirection::eVertical:
		{
			stick->SetPosition(rightChildWindowNode->mPositionX - margin / 2.0f,
				rightChildWindowNode->mPositionY + rightChildWindowNode->mHeight / 2.0f);
			direction = 1;
		}
		break;
		case ESizeControlStickDirection::eHorizontal:
		{
			stick->SetPosition(rightChildWindowNode->mPositionX + rightChildWindowNode->mWidth/2.0f,
				rightChildWindowNode->mPositionY -margin/2.0f);
			direction = 3;
		}
		break;

	}



	//두자식윈도우노드들이 적절히 처리되었으니 
	//자손윈도우노드들에게 연쇄적으로 적용시킨다.

	std::queue<WindowNode*> childWindowNodeQueue;

	if(leftChildWindowNode->mLeftChildWindowNode!=nullptr)
		childWindowNodeQueue.push(leftChildWindowNode);

	if (rightChildWindowNode->mLeftChildWindowNode != nullptr)
	childWindowNodeQueue.push(rightChildWindowNode);
	


	while (!childWindowNodeQueue.empty())
	{
		WindowNode* childWindowNode = childWindowNodeQueue.front();
		childWindowNodeQueue.pop();

		if (childWindowNode->mLeftChildWindowNode != nullptr)
		{
			ModifyChildWindowSizePos(childWindowNode, direction);
			childWindowNodeQueue.push(childWindowNode->mLeftChildWindowNode);
			childWindowNodeQueue.push(childWindowNode->mRightChildWindowNode);
		}

	}

	

}


void Quad::FrameWindowController::HandleEvent(Event* pEvent)
{

	if (pEvent == nullptr)
		return;




	DockingWindowController::HandleEvent(pEvent);



	const std::string& eventName = pEvent->GetName();

	if (eventName == "WindowRequestEvent")
	{
		HandleWindowRequestEvent(pEvent);

	}



}

void Quad::FrameWindowController::HandleWindowRequestEvent(Event* pEvent)
{
	WindowRequestEvent* windowRequestEvent = (WindowRequestEvent*)pEvent;


	switch (windowRequestEvent->GetEventType())
	{
	case  EWindowRequestEventType::eWindowEnd:
	{
		//프레임윈도우 창을 닫는다 -> 프로그램종료
		//도킹된창(자식창)뿐만아니라 도킹이풀린창들도 종료

		for (int i = 0; i < mChildWindowVector.size(); ++i)
		{
			if (!mChildWindowVector[i]->GetDockingState())
			{
				DestroyWindow(mChildWindowVector[i]->GetWindowHandle());
			}
		}
		DestroyWindow(GetWindow()->GetWindowHandle());
	}
	break;
	case EWindowRequestEventType::eWindowMaximize:

	{
		SetEdgeDragDirection(5);
		EWindowShowState windowShowState = GetWindowShowState();
		if (windowShowState == EWindowShowState::eNormal)
		{
			SetWindowShowState(EWindowShowState::eMaximize);
			ShowWindow(GetWindow()->GetWindowHandle(), SW_MAXIMIZE);
		}
		else if (windowShowState == EWindowShowState::eMaximize)
		{
			
			SetWindowShowState(EWindowShowState::eNormal);
			ShowWindow(GetWindow()->GetWindowHandle(), SW_RESTORE);
		}
		//최대화하는것이니 ,도킹된자식창들의 크기와위치재조정
	}
	break;
	case EWindowRequestEventType::eWindowMinimize:

	{
		ShowWindow(GetWindow()->GetWindowHandle(), SW_MINIMIZE);
		WindowMinimize();
	}
	break;



	}





}

Quad::FrameWindowController::WindowNode* Quad::FrameWindowController::FindWindowNode(TaskWindow* window)
{
	std::queue<WindowNode*> windowNodeQueue;

	windowNodeQueue.push(mRootWindowNode);


	while(!windowNodeQueue.empty())
	{
		WindowNode* windowNode = windowNodeQueue.front();
		windowNodeQueue.pop();

		if (windowNode->mWindow == window)
		{
			return windowNode;
		}

		
		if (windowNode->mLeftChildWindowNode != nullptr)
			windowNodeQueue.push(windowNode->mLeftChildWindowNode);
		
		if (windowNode->mRightChildWindowNode != nullptr)
			windowNodeQueue.push(windowNode->mRightChildWindowNode);
	}
	return nullptr;

	
}

void Quad::FrameWindowController::ModifyChildWindowSizePos(WindowNode* parentNode, int direction)
{
	//부모노드의 영역은 이미 위치와 사이즈가 바뀐상태인데
	//delta가 어떻게되는가를 알아야돼

	WindowNode* childNodeOne = parentNode->mLeftChildWindowNode;
	WindowNode* childNodeTwo = parentNode->mRightChildWindowNode;

	//먼저 자식윈도우들혹은 영역들의 너비를 다시 계산한다.

	//위치를 계산한다.

	//그리고 만약에 창이라면(mWindow!=nullptr) MoveWindow()호출


	//너 자식창 기존부모창 너비에 어느정도 비율이였니

	float preParentWidth = parentNode->mWidth  - parentNode->mDeltaX;
	float preParentHeight = parentNode->mHeight - parentNode->mDeltaY;


	if (direction == 5 ||direction == 1 || direction == 2)
	{	//left,right에서 사이즈조절
		float ChildOneWidthRate = childNodeOne->mWidth / preParentWidth;
		float ChildOneDeltaX = ChildOneWidthRate * parentNode->mDeltaX;
		childNodeOne->mWidth += ChildOneDeltaX;
		childNodeOne->mDeltaX = ChildOneDeltaX;


		float ChildTwoWidthRate = childNodeTwo->mWidth / preParentWidth;
		float ChildTwoDeltaX = ChildTwoWidthRate * parentNode->mDeltaX;
		childNodeTwo->mWidth += ChildTwoDeltaX;
		childNodeTwo->mDeltaX = ChildTwoDeltaX;

		childNodeOne->mDeltaY = 0.0;
		childNodeTwo->mDeltaY = 0.0f;


	}
	 if (direction == 5 ||direction == 3 || direction == 4)
	{
		//top ,bottom 테투리에서 사이즈조절
		float ChildOneHeightRate = childNodeOne->mHeight / preParentHeight;
		float ChildOneDeltaY = ChildOneHeightRate * parentNode->mDeltaY;
		childNodeOne->mHeight += ChildOneDeltaY;
		childNodeOne->mDeltaY = ChildOneDeltaY;

		float ChildTwoHeightRate = childNodeTwo->mHeight / preParentHeight;
		float ChildTwoDeltaY = ChildTwoHeightRate * parentNode->mDeltaY;
		childNodeTwo->mHeight += ChildTwoDeltaY;
		childNodeTwo->mDeltaY = ChildTwoDeltaY;

		childNodeOne->mDeltaX = 0.0f;
		childNodeTwo->mDeltaX = 0.0f;

	}
	


	//위치설정
	if (childNodeOne->mPositionX == childNodeTwo->mPositionX)
	{
		//가로선에의해서 위아래로 분리되었다.

		//앞서 너비높이 부동소수오차누적문제
		float ErrorCorrectionWidth= (parentNode->mWidth -childNodeOne->mWidth);
		childNodeOne->mWidth += ErrorCorrectionWidth;
		childNodeTwo->mWidth += ErrorCorrectionWidth;


		float ErrorCorrectionHeight = (parentNode->mHeight - (childNodeOne->mHeight+childNodeTwo->mHeight)-margin) / 2.0f;
		childNodeOne->mHeight += ErrorCorrectionHeight;
		childNodeTwo->mHeight += ErrorCorrectionHeight;



		if (direction == 5 || direction == 1 || direction == 2)
		{//가로선에의해서 위아래로 분리되어있는상태에서 , 좌우로 움직여서 사이즈가 조절된상태 
			
			childNodeOne->mPositionX = parentNode->mPositionX;
			childNodeTwo->mPositionX = parentNode->mPositionX;
		}
		
		if (direction == 5 || direction == 3 || direction == 4)
		{//가로선에의해서 위아래로 분리되어있는상태에서 , 위아래로 움직여서 사이즈가 조절된상태(즉 너비는그대로 높이가변경되고 ,위치값재조정) 
			//top
			childNodeOne->mPositionY = parentNode->mPositionY ;
			childNodeTwo->mPositionY = parentNode->mPositionY + childNodeOne->mHeight + margin ;
		}

	}
	else
	{
		//세로선에의해서 좌우로 분리되었다.

		//앞서 너비높이 부동소수오차누적문제
		float ErrorCorrectionWidth = (parentNode->mWidth - (childNodeOne->mWidth+ childNodeTwo->mWidth) -  margin) / 2.0f;
		childNodeOne->mWidth += ErrorCorrectionWidth;
		childNodeTwo->mWidth += ErrorCorrectionWidth;

		float ErrorCorrectionHeight = (parentNode->mHeight - (childNodeOne->mHeight));
		childNodeOne->mHeight += ErrorCorrectionHeight;
		childNodeTwo->mHeight += ErrorCorrectionHeight;



		if (direction == 5 || direction == 1 || direction == 2)
		{//세로선에의해서 좌우로 분리되어있는상태에서 , 좌우로 움직여서 사이즈가 조절된상태 

			childNodeOne->mPositionX = parentNode->mPositionX ;

			childNodeTwo->mPositionX = parentNode->mPositionX + childNodeOne->mWidth +margin;
		}
		if (direction == 5 || direction == 3 || direction == 4)
		{//세로선에의해서 좌우로 분리되어있는상태에서 , 위아래로 움직여서 사이즈가 조절된상태 

			childNodeOne->mPositionY = parentNode->mPositionY ;
			childNodeTwo->mPositionY = parentNode->mPositionY ;
		}
	}

	if (childNodeOne->mWindow != nullptr)
	{
		MoveWindow(childNodeOne->mWindow->GetWindowHandle(), childNodeOne->mPositionX, childNodeOne->mPositionY, childNodeOne->mWidth, childNodeOne->mHeight, true);
	}


	if (childNodeTwo->mWindow != nullptr)
	{

	   MoveWindow(childNodeTwo->mWindow->GetWindowHandle(), childNodeTwo->mPositionX, childNodeTwo->mPositionY, childNodeTwo->mWidth, childNodeTwo->mHeight, true);

	}

	
	if (parentNode->mWindow == nullptr)
	{
	
			switch (parentNode->mSizeControllerStick->GetDirection())
			{
			case ESizeControlStickDirection::eVertical:
			{
				//수직으로 세워져있는 막대의 가로는 줄어들지않는다.
				//즉 항상 margin사이즈를 가진다
				parentNode->mSizeControllerStick->SetWidthHeight(margin, childNodeOne->mHeight);
				parentNode->mSizeControllerStick->SetPosition(childNodeTwo->mPositionX - margin / 2.0f, childNodeTwo->mPositionY +childNodeTwo->mHeight/2.0f);
				}
			break;
			case ESizeControlStickDirection::eHorizontal:
			{
				//수평으로 누워있는 막대의 세로는 줄어들지않는다 
				parentNode->mSizeControllerStick->SetWidthHeight(childNodeOne->mWidth, margin);
				parentNode->mSizeControllerStick->SetPosition(childNodeTwo->mPositionX +childNodeTwo->mWidth/2.0f , childNodeTwo->mPositionY - margin / 2.0f);

			}
			break;
		}
	}

}

void Quad::FrameWindowController::ReleaseDockingChildWindowRestructRegionNode(WindowNode* parentWindowNode, WindowNode* otherChildWindowNode,
	WindowNode* dockingWindowNode, int direction)
{



	

}

void Quad::FrameWindowController::MouseMove(Event* event)
{
	MouseEvent* mouseEvent = (MouseEvent*)event;
	mMouseClientPositionX = mouseEvent->GetClientPositionX();
	mMouseClientPositionY = mouseEvent->GetClientPositionY();


	ESystemID systemID = event->GetSystemID();

	if (systemID == mWindowDockingSystem->GetSystemID())
	{
		mWindowDockingSystem->HandleEvent(event);
		return;
	}
	else if (systemID == mWindowLayoutSystem->GetSystemID())
	{
		mWindowLayoutSystem->HandleEvent(event);
		return;
	}
	else if (systemID == mWorldSystem->GetSystemID())
	{
		mWorldSystem->HandleEvent(event);
		return;
	}
	else  if (systemID == ESystemID::eNone)
	{
		//딱히 지정되지않았다.
		
		mWindowDockingSystem->HandleEvent(event);
		mWindowLayoutSystem->HandleEvent(event);
	//	mUiSystem->HandleEvent(event);
		mWorldSystem->HandleEvent(event);
	}




}

