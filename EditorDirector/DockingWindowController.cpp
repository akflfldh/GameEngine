#include "EditorDirector/DockingWindowController.h"
#include"EditorDirector/ButtonUiEntitiy.h"
#include"Event/EventTable.h"
//#include"WorldEditEntityFactory.h"
//#include"WorldEditUiEntity.h";
//#include"Text.h"
#include"ResourceManager/MeshManager.h"
#include"ResourceManager/TextureManager/TextureManager.h"
#include"Core/System.h"
#include"Core/RenderSystem.h"
#include"System/windowChromeSystem.h"
#include"System/WindowDockingSystem.h"
#include"Core/WindowRequestEvent.h"
#include"EditorDirector/Window.h"
#include"EditorDirector/FrameWindowController.h"
#include"Core/MouseEvent.h"
#include"Core/ResourceController.h"
#include"Core/SelectEvent.h"
#include"Core/Mouse.h"
#include"Core/KeyBoard.h"

#include"EditorDirector/GamePlayWindowController.h"

#include"System/GamePlaySystem.h"
#include"Effect/Effect.h"


#include"TaskWindowCommonHandlerComponent.h"
#include"MouseHandlerComponent.h"
#include"KeyInputHandlerComponent.h"

namespace Quad
{
	//CircleQueue<Event*> GamePlayWindowController::mEventQueue;
	//ReaderWriter GamePlayWindowController::mEventQueueSynObject;

	//CircleQueue<Event*>GamePlayWindowController:: mEventQueue;


	DockingWindowController::DockingWindowController(RenderSystem* renderSystem)
		:BaseWindowController(renderSystem)
	{
	}


	void DockingWindowController::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device,
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> graphicsCommandList,
		MeshManager* meshManager, MaterialManager* materialManager,
		TextureManager* textureManager, MapManager* mapManager, TaskWindow* window,
		System* worldSystem,WindowChromeSystem * windowLayoutSystem,
		WindowDockingSystem* windowDockingSystem,
		DescriptorHeapManagerMaster* descriptorHeapManagerMaster, bool playMode)
	{
		Controller::Initialize();

		mDevice = device;
		mGraphicsCommandList = graphicsCommandList;
		mMeshManager = meshManager;
		mMaterialManager = materialManager;
		mTextureManager = textureManager;
		mMapManager = mapManager;
		//mRenderSystem = renderSystem;
		//mUiSystem = uiSystem;
		mWorldSystem = worldSystem;
		mWindowLayoutSystem = windowLayoutSystem;
		mWindowDockingSystem = windowDockingSystem;

		mDescriptorHeapManagerMaster = descriptorHeapManagerMaster;
		mWindow = window;
		//SetPlayMode(playMode);


		//mEventQueue.Initialize(100);
		//mEventQueueSynObject.Initialize();

		//InitGamePlayUiSystem();


		AddMsgHanlderComponent(new TaskWindowCommonHandlerComponent(this));
		AddMsgHanlderComponent(new MouseHandlerComponent(this));
		AddMsgHanlderComponent(new KeyInputHandlerComponent(this));



	}





	void DockingWindowController::Update(float deltaTime)
	{

		

		Event* pEvent = nullptr;


	


		while (1)
		{	
		Event* pEvent = GetEvent();//or unique_ptr 로 리턴 그럼마지막에 delete를 명시하는것을 체크하지않아도된다.
		if (pEvent == nullptr)
			break;
		


		HandleEvent(pEvent);
		delete pEvent;
		}

		bool playMode = GetPlayMode();


	  //도킹시스템은 ON되었을때만 동작한다
	  //.프레임윈도우는 도킹시스템이없다.
	  //매번체크하는게 좋지않아,

	  if (mWindowDockingSystem->GetActiveState() && mWindowDockingSystem->GetDockingSystemState())
	  {
		  mWindowDockingSystem->Update(deltaTime);
	  }
	 

	//	if(mUiSystem->GetActiveState())
		//	mUiSystem->Update(deltaTime, playMode);


		if(mWorldSystem->GetActiveState())
			mWorldSystem->Update(deltaTime, playMode);

		if(mWindowLayoutSystem->GetActiveState())
			mWindowLayoutSystem->Update(deltaTime);
	  
	  //지금 맵이 사실은 GamePlaySystem으로 바꿔야된다.


	


	}

	void DockingWindowController::UploadObjectToRenderSystem()
	{
		//update다음에 호출되어야한다.
		// 
		// 
		//렌더링되지않는다면 처리할이유가없다.


		RenderSystem* renderSystem = GetRenderSystem();

		if (!renderSystem->GetRenderState())
			return;

		renderSystem->PreUpdate();


		//이렇게 렌터 항목,아이템들을 넘기는것을 함수로처리하자.
		std::vector<Object*> objectVector;

	/*	mGameWorldSystem->GetEntity(objectVector);
		mRenderSystem->SetEntityVector(objectVector, ESystemType::eMainSystem);
		objectVector.clear();*/

		if (mWorldSystem->GetActiveState())
		{
			const std::vector<MapLayer> mapLayerVectorMain = mWorldSystem->GetEntity();
			renderSystem->SetMapLayerVector(mapLayerVectorMain, ESystemType::eMainSystem);			
		}

		/*if (mUiSystem->GetActiveState())
		{
			const std::vector<MapLayer>& mapLayerVectorUi = mUiSystem->GetEntity();
			renderSystem->SetMapLayerVector(mapLayerVectorUi, ESystemType::eUiSystem);
		}*/

		if (mWindowLayoutSystem->GetActiveState())
		{
			const std::vector<MapLayer> mapLayerVectorLayout = mWindowLayoutSystem->GetEntity();
			renderSystem->SetMapLayerVector(mapLayerVectorLayout, ESystemType::eWindowLayoutSystem);
		}

		
		if (mWindowDockingSystem->GetActiveState() && mWindowDockingSystem->GetDockingSystemState())
		{
			const std::vector<MapLayer> mapLayerVectorDocking = mWindowDockingSystem->GetEntity();
			renderSystem->SetMapLayerVector(mapLayerVectorDocking, ESystemType::eDockingSystem);

		}



	/*	mUiSystem->GetEntity(objectVector);
		mRenderSystem->SetEntityVector(objectVector, ESystemType::eUiSystem);
		objectVector.clear();

		mWindowLayoutSystem->GetEntity(objectVector);
		mRenderSystem->SetEntityVector(objectVector, ESystemType::eWindowLayoutSystem);
		objectVector.clear();


		RenderSettingItem renderSettingItem;
		mGameWorldSystem->GetRenderSettingItem(renderSettingItem);
		mRenderSystem->SetRenderSettingItem(renderSettingItem, ESystemType::eMainSystem);

		mUiSystem->GetRenderSettingItem(renderSettingItem);
		mRenderSystem->SetRenderSettingItem(renderSettingItem, ESystemType::eUiSystem);

		mWindowLayoutSystem->GetRenderSettingItem(renderSettingItem);
		mRenderSystem->SetRenderSettingItem(renderSettingItem, ESystemType::eWindowLayoutSystem);*/


		//docking system
		/*if (mWindowDockingSystem->GetDockingSystemState())
		{
			mWindowDockingSystem->GetEntity(objectVector);
			mRenderSystem->SetEntityVector(objectVector, ESystemType::eDockingSystem);

			mWindowDockingSystem->GetRenderSettingItem(renderSettingItem);
			mRenderSystem->SetRenderSettingItem(renderSettingItem, ESystemType::eDockingSystem);

		}*/

		renderSystem->Update();



	}

	void DockingWindowController::EndUpdate(float deltaTime)
	{
		

		/*if (mUiSystem != nullptr && mUiSystem->GetActiveState())
			mUiSystem->EndUpdate();*/
		

		if(mWorldSystem !=nullptr && mWorldSystem->GetActiveState())
			mWorldSystem->EndUpdate(deltaTime);

		if(mWindowLayoutSystem!=nullptr && mWindowLayoutSystem->GetActiveState())
			mWindowLayoutSystem->EndUpdate(deltaTime);
		
		if(mWindowDockingSystem!=nullptr && mWindowDockingSystem->GetDockingSystemState() && mWindowDockingSystem->GetActiveState())
			mWindowDockingSystem->EndUpdate(deltaTime);




	}

	void DockingWindowController::Draw()
	{

		auto renderSystem =	GetRenderSystem();

		if (!renderSystem->GetRenderState())
			return;

		

		renderSystem->Draw();

	
	}

	void DockingWindowController::OnResize(int clientWidth, int clientHeight,int direction)
	{
		auto renderSystem = GetRenderSystem();


		//사이즈를 직접조절한 윈도우의경우여기서 크기랑 위치도계산을해야하는데 여기서 위치를 
		// 계산을할수가없다.
		//resize될때마다 처리되어야하는 엔티티들도 있으니 그들도 처리가되어야한다. event를 전달해야돼.
		TaskWindow* window = GetWindow();
		window->SetClientWidth(clientWidth);
		window->SetClientHeight(clientHeight);
		window->SetClientWidthFloat((float)clientWidth);
		window->SetClientHeightFloat((float)clientHeight);
		//logical client widhtmheight를 설정
		int titleBarHeight = mWindowLayoutSystem->GetTitleBarHeight();
		mLogicalClientHeight = clientHeight - titleBarHeight;
		mLogicalClientWidth = clientWidth;


		mWindowLayoutSystem->OnResize(clientWidth, clientHeight);


		//mUiSystem->OnResize(clientWidth, clientHeight);
		//mUiSystem->OnResize(mLogicalClientWidth, mLogicalClientHeight);//view port의 조정이필요할것이다
	//	mGameWorldSystem->OnResize(clientWidth, clientHeight);
		mWorldSystem->OnResize(mLogicalClientWidth, mLogicalClientHeight);
		if (mWindowDockingSystem)
		{
			mWindowDockingSystem->OnResize(clientWidth, clientHeight);
		}
		renderSystem->OnResize(clientWidth, clientHeight);





		if(GetWindowActiveFlag())
			UpdateAndDrawDockingWindow();
	}

	/*void WindowController::AddEvent(Event* event)
	{
		Controller::AddEvent(event);
	}*/


	TaskWindow* DockingWindowController::GetWindow() const
	{
		return mWindow;
	}

	void DockingWindowController::SetBackgroundColor(float r, float g, float b, float a)
	{
		auto renderSystem = GetRenderSystem();
		renderSystem->SetBackgroundColor(r, g, b, a);
	}

	void DockingWindowController::WindowActivate(int wmActivateState)
	{





	}

	void DockingWindowController::WindowMove(int preWindowPosX, int preWindowPosY, int currWindowPosX, int currWindowPosY)
	{
		
		TaskWindow* window = GetWindow();
		FrameWindowController* frameWindowController = dynamic_cast<FrameWindowController*>(this);


		if (!window->GetDockingState() && GetDraggingWindowOnTitleBarState())
		{

			POINT screenPosition = { mMouseClientPositionX ,mMouseClientPositionY };
			ClientToScreen(window->GetWindowHandle(), &screenPosition);

			//프레임윈도우컨트롤러 메서드 호출 ,도킹되지않은 윈도우가 타이틀바를 클릭해서 움직이고있다
			//마우스위치는 다음과 같고  , 그 위치를 보고 마우스가 올라와있는 도킹된 창을 확인해서
			//그 도킹된창에 도킹시스템을 작동시켜라. 
			
			FrameWindowController::SetMovingWindow(window);
			FrameWindowController::OnChildWindowDockingSystem(screenPosition.x, screenPosition.y);
		}





	}

	void DockingWindowController::WindowMinimize()
	{
	}

	void DockingWindowController::DragAndDropFile(const std::vector<std::string> & dropFileVector)
	{


	}

	//void DockingWindowController::DragAndDropFile(std::vector<std::string>&& dropFileVector)
	//{
	//}

	void DockingWindowController::OnDockingSystem(int mouseScreenPositionX, int mouseScreenPositionY)
	{

		POINT mouseClientPosition = { mouseScreenPositionX ,mouseScreenPositionY };
		ScreenToClient(GetWindow()->GetWindowHandle(), &mouseClientPosition);
		mWindowDockingSystem->SetDockingSystemState(true);
	

		
		//마우스는 눌려져있는상태이지 그러니깐 전송되는이벤트는 
		// hover, LbuttonUp이될것이다.
		//여기서 마우스 move event를 전송
		MouseEvent* mouseMoveEvent = new MouseEvent("MouseMove");
		mouseMoveEvent->SetClientPosition(mouseClientPosition.x, mouseClientPosition.y);
		
		AddEvent(mouseMoveEvent);
		UpdateAndDrawDockingWindow();


		//한 스레드내에 한 window가 타이틀바를 클릭하여 드래그하는작업을 하면  그 스레드의 메세지루프가 
		//윈도우즈내부의 모달루프로 들어가서 다른창들이 전혀 진행되지않는다.
		//정상적인것인데.
		//docking system을 작동한 창은 update와 draw가 계속되어져야한다
		//따라서 여기서 update render를 수행한다
		//가장 간단한방법 
		//다른방법으로는 애초에 update draw를 멀티스레드를 이용하여 처리한다.
		//또다른방법으로는 창마다 메세지루프를 다른스레드에서 수행한다.



	}

	void DockingWindowController::OffDockingSystem()
	{
		mWindowDockingSystem->SetDockingSystemState(false);

		UpdateAndDrawDockingWindow();


	}

	void DockingWindowController::EndDockingSystem(int mouseScreenPositionX, int mouseScreenPositionY)
	{
		POINT mouseClientPosition = { mouseScreenPositionX ,mouseScreenPositionY };
		ScreenToClient(GetWindow()->GetWindowHandle(), &mouseClientPosition);
		


		MouseEvent* mouseLButtonUpEvent = new MouseEvent("LButtonUp");
		mouseLButtonUpEvent->SetClientPosition(mouseClientPosition.x, mouseClientPosition.y);

		AddEvent(mouseLButtonUpEvent);
		UpdateAndDrawDockingWindow();

	}

	void DockingWindowController::UpdateAndDrawDockingWindow()
	{
		ResourceController::Update();
		Update(0.0f);
		UploadObjectToRenderSystem();
		Draw();

	}



	void DockingWindowController::SetTitleBarSize(bool usage,UINT width, UINT height)
	{
		TaskWindow* window = GetWindow();
		mLogicalClientWidth = window->GetClientWidth();
		mLogicalClientHeight = window->GetClientHeight() - height;
		mWindowLayoutSystem->SetTitleBarSize(width,height);
		
		//타이틀바가 작동하도록 한다.
		//사이즈가 변할때마다 logicalClient영역을 조정해야하고

		//D3D12_VIEWPORT gameWorldSystemViewport=	mGameWorldSystem->GetViewPort();
		//gameWorldSystemViewport.TopLeftY = 500;
		//mGameWorldSystem->SetViewPort(gameWorldSystemViewport);

		//D3D12_VIEWPORT uiSystemViewport = mUiSystem->GetViewPort();
		//uiSystemViewport.TopLeftY = 500;
		//mUiSystem->SetViewPort(uiSystemViewport);


	}

	int DockingWindowController::GetTitleBarHeight()
	{
		return mWindowLayoutSystem->GetTitleBarHeight();
	}

	int DockingWindowController::GetLogicalClientWidth() const
	{
		return mLogicalClientWidth;
	}

	int DockingWindowController::GetLogicalClientHeight() const
	{
		return mLogicalClientHeight;
	}

	int DockingWindowController::GetEdgeDragDirection() const
	{
		return mEdgeDragDirection;
	}

	void DockingWindowController::SetEdgeDragDirection(int direction)
	{
		mEdgeDragDirection = direction;
	}

	void DockingWindowController::SetDragginWindowOnTitleBarState(bool state)
	{
		mIsDraggingWindowOnTitleBar = state;
	}

	bool DockingWindowController::GetDraggingWindowOnTitleBarState() const
	{
		return mIsDraggingWindowOnTitleBar;
	}

	bool DockingWindowController::GetMousePositionState() const
	{
		return mMousePositionState;
	}

	void DockingWindowController::SetMousePositionState(bool state)
	{
		mMousePositionState = state;
	}

	void DockingWindowController::StartTrackMouseWindowOut()
	{
		TRACKMOUSEEVENT trackMouseEvent;
		trackMouseEvent.cbSize = sizeof(trackMouseEvent);
		trackMouseEvent.dwFlags = TME_LEAVE; //결국 클라이언트영역을 나갔느냐 안나갔느냐 
		trackMouseEvent.hwndTrack = mWindow->GetWindowHandle();
		trackMouseEvent.dwHoverTime = INFINITE;

		TrackMouseEvent(&trackMouseEvent);

	}

	void DockingWindowController::SetWindowShowState(EWindowShowState showState)
	{

		mWindowShowState = showState;
	}

	Quad::EWindowShowState DockingWindowController::GetWindowShowState() const
	{
		return mWindowShowState;
	}

	void DockingWindowController::SetRenderState(bool state)
	{
		auto renderSystem = GetRenderSystem();
		renderSystem->SetRenderState(state);
	}

	bool DockingWindowController::GetRenderState() const
	{
		auto renderSystem = GetRenderSystem();
		return renderSystem->GetRenderState();
	}

	void DockingWindowController::HandleWindowReStore()
	{


	}

	bool DockingWindowController::GetWindowDockingSystemState() const
	{
		return mWindowDockingSystem->GetDockingSystemState();
	}

	void DockingWindowController::SetDuplicateResizeFlag(bool state)
	{
		mDuplicateResizeState = state;
	}

	bool DockingWindowController::GetDuplicateResizeFlag() const
	{
		return mDuplicateResizeState;
	}

	void DockingWindowController::AddEffect(Effect* effect)
	{
		mEffectTable. try_emplace(effect->GetName(), effect);
	}

	void DockingWindowController::SetPlayMode(bool playMode)
	{
		if (playMode == true)
		{
		//	mUiSystem->OnRuntimeMode();
			mWorldSystem->OnRuntimeMode();
		}
		else
		{
			//mUiSystem->OffRunTimeMode();
			mWorldSystem->OffRunTimeMode();
		}
		//mPlayMode = playMode;


		//mUiSystem->SetPlayModeState(playMode);

		//mGameWorldSystem->SetPlayModeState(playMode);

	}

	bool DockingWindowController::GetPlayMode() const
	{
		return mPlayMode;




	}

	void DockingWindowController::SetPause(bool state)
	{
		mPauseState = state;

		mWorldSystem->SetPause(state);
		//mUiSystem->SetPause(state);

	}

	bool DockingWindowController::GetPause() const
	{
		return mPauseState;
	}

	void DockingWindowController::SetWindowActiveFlag(bool flag)
	{
		mWindowActiveFlag = flag;

		HWND hwnd = mWindow->GetWindowHandle();
		if (flag == true)
		{
			ShowWindow(hwnd, SW_SHOW);
		}
		else
		{
			ShowWindow(hwnd, SW_HIDE);
		}

	}

	bool DockingWindowController::GetWindowActiveFlag() const
	{
		return mWindowActiveFlag;
	}

	void DockingWindowController::SetSystemActiveState(int systemNum, bool state)
	{
		if (systemNum == 0)
		{
			mWorldSystem->SetActiveState(state);
		}
		else if (systemNum == 1)
		{
			mWindowLayoutSystem->SetActiveState(state);
		}
		else if (systemNum == 2)
		{
			mWindowDockingSystem->SetActiveState(state);
		}

	}

	void DockingWindowController::HandleEvent(Event * pEvent)
	{
		

		if (pEvent != nullptr)
		{
		
		
			
			//문자열 비교는 비용이 많이들어간다.
			const std::string& eventName = pEvent->GetName();
			EEventID eventID = pEvent->GetEventID();
			if (eventName == "DragAndDrop")
			{

			}
			else if (eventName == "LButtonDownDouble")
			{
				LButtonDownDouble(pEvent);
			}
			else if (eventName == "LButtonDown")
			{
				LButtonDown(pEvent);
			}
			else if (eventName == "LButtonUp")
			{
				LButtonUp(pEvent);
			}
			else if (eventName == "RButtonDown")
			{
				RButtonDown(pEvent);
			}
			else if (eventName == "RButtonUp")
			{
				RButtonUp(pEvent);

			}
			else if (eventName == "MouseMove")
			{
				MouseMove(pEvent);
			}
			else if (eventName == "GamePlay")
			{


			}
			else if (eventName == "KeyDown")
			{
				KeyDown(pEvent);


			}
			else if (eventName == "KeyUp")
			{
				KeyUp(pEvent);


			}
			else if (eventName == "WindowRequestEvent")
			{
				HandleWindowRequestEvent(pEvent);
			}
			else if (eventName == "MouseInOut")
			{
				HandleMouseInOut(pEvent);
			}

			else if (eventID == EEventID::eSelectEvent)
			{


				SelectEvent* selectEvent = (SelectEvent*)pEvent;
				if(selectEvent->GetSelectState())
				{
					Object* object = selectEvent->GetSelectedObject();
					object->GetSystem()->SetSelectObject(object);

					if (selectEvent->GetExclusiveEventHandleFlag())
					{
						object->GetSystem()->SetEventFocusFlag(true);
					}
					else
					{
						object->GetSystem()->SetEventFocusFlag(false);
						//object->GetSystem()->SetSelectObject(nullptr);
					}
			
				}else
				{
					Object* object = selectEvent->GetSelectedObject();
					object->GetSystem()->SetSelectObject(nullptr);
					object->GetSystem()->SetEventFocusFlag(false);
				}
			}
			else
			{


				HandleDefault(pEvent);




			}




			////해시함수,해시테이블을 사용해서 문자열ID는 정수로 바꿔서 Switch문에사용하자.
			//switch (pEvent->GetEventType())
			//{
			//case EventType::eDragAndDrop:

			//	break;
			//case EventType::eLButtonDown:
			//	LButtonDown(pEvent);
			//	break;
			//case EventType::eLButtonUp:

			//	break;
			//case EventType::eRButtonDown:
			//	RButtonDown(pEvent);
			//	break;
			//case EventType::eRButtonUp:

			//	break;
			//case EventType::eMouseMove:
			//	
			//	break;
			//}

		}


	}

	/*Event* WindowController::GetEvent()
	{
		return Controller::GetEvent();
	}*/


	//Event* WindowController::GetEvent()
	//{
	//	//WriterStart(); 

	//	mEventQueueSynObject.WriterStart();
	//	Event* pEvent = nullptr;
	//	if (!mEventQueue.isEmpty())
	//	{
	//		pEvent = mEventQueue.Front();
	//		mEventQueue.Pop();

	//		WriterEnd();
	//		return pEvent;
	//	}
	//	mEventQueueSynObject.WriterEnd();
	//	//WriterEnd();
	//	return pEvent;
	//}

	void DockingWindowController::InitGamePlayUiSystem()
	{









		





	//	//system별로 초기화가 이루어져야돼 
	//	//system안에는 map이라는것을 있을것이고 map별로 초기화나 엔티티들을 로드해서 초기화하여 씬을 구성하는거지,


	//	//uisystem->init()
	//	//시스템 내부를 초기화하는것과
	//	//게임을 구성하는 요소를들을 로드하고 생성하는 초기화

	//	//gameworldsystem->init()


	//	WorldEditUiEntity* buttonUiEntity = (WorldEditUiEntity*)WorldEditEntityFactory::GetEntityClass("buttonC", "ButtonEntity");

	//	Mesh* rectMesh = mMeshManager->GetMesh("Rectangle");
	//	//	buttonUiEntity->Initialize("button1", rectMesh);


	//		//Texture* defaultTexture = mTextureManager->GetTexture("Button.png");
	//	Texture* defaultTexture = mTextureManager->GetTexture("textTexture");
	//	//buttonUiEntity->GetModel().SetTextureToAllSubMesh(defaultTexture);



	//	buttonUiEntity->GetTransform().SetScaleLocal({ 100.0f,100.0f,1.0f });
	//	//buttonUiEntity->GetTransform().SetPositionLocal({ 0, 0, 10.0f });

	//	mUiSystem->AddEntity(buttonUiEntity);

	//	//이벤트를 잘정의하는것도 중요하겠다 벌써 헷갈린다.
	//	//문서화도 필요하다.

	//	/*EventTable::AddObject("MouseSelect", buttonUiEntity);
	//	EventTable::AddObject("MouseMove", buttonUiEntity);
	//	EventTable::AddObject("LButtonUp", buttonUiEntity);*/


	//	((Object*)buttonUiEntity)->mUiSystem = mUiSystem;
	//	((Object*)buttonUiEntity)->mMap = mUiSystem->GetMap();


	//	//텍스처도 입혀야되고  이런것을보면 팩토리 패턴이 필요하다. 혹은 리소스컨트롤러가 팩토리패턴일수도있고 
	//	//파일로부터 읽어와서 처리하면좋겠다 이런게 데이터주도 방식인가?

	//	ButtonUiEntitiy* playButtonUiEntity = new ButtonUiEntitiy;
	//	playButtonUiEntity->Initialize("playButton", rectMesh);

	//	Texture* playButtonTexture = mTextureManager->GetTexture("PlayButtonOff.png");
	//	playButtonUiEntity->GetModel().SetTextureToAllSubMesh(playButtonTexture);

	//	playButtonUiEntity->GetTransform().SetScaleLocal({ 100.0f,100.0f,100.0f });
	//	playButtonUiEntity->GetTransform().SetPositionLocal({ 0,200.0f,10.0f });

	//	mUiSystem->AddEntity(playButtonUiEntity);

	//	WorldEditUiEntity* copiedInstance = (WorldEditUiEntity*)
	//		WorldEditEntityFactory::GetEntityClass("button", "ButtonEntity");

	//	copiedInstance->GetTransform().SetScaleLocal({ 100.0f,100.0f,1.0f });
	//	copiedInstance->GetTransform().SetRotationLocal({ 0.0f,0.0f,45.0f });


	//	//초기화를해야하는데

	//	mUiSystem->AddEntity(copiedInstance);
	//	//AddWorldEditUiEntityToMap(mGamePlayUiSystem->GetMap(),copiedInstance);
	////copiedInstance->GetTransform().ResetChangeValue();

	////mGamePlayUiSystem->AddUiEntity(copiedInstance);

	////텍스트 같은경우네느 월드에딧이나 ,런타임게임엔진에서나 기능이 거의 동일할거같은데
	////worldEditUiEntityTable을 상속해서 또 텍스트 클래스를 정의해야하는가 아닌가.
	////

	////워낙 기능이 유사하고 그냥 텍스트는 따로두어야하나 
	////아니면 

	////아니면 text는 특별하잖아,스키마파일에서 텍스트 파일을 보면 텍스트 인스턴스를 생성하는거지
	////world eidtuientity가아니라

	//	/*Text* textEntity = new Text;
	//	textEntity->Initialize("text", rectMesh, 600.0f, 400.0f);

	//	Texture* textBoxTexture = TextureManager::GetTexture("TextBox.png");
	//	textEntity->GetModel().SetTextureToAllSubMesh(textBoxTexture);
	//	textEntity->GetTransform().SetScaleLocal({ 600.0f,400.0f,1.0f });
	//	textEntity->GetTransform().SetPositionLocal({ 0, 0, 100.0f });

	//	mUiSystem->AddEntity(textEntity);*/













	//	//	((Object*)textEntity)->mSystem = mGamePlayUiSystem;
	//		//((Object*)textEntity)->mMap = mGamePlayUiSystem->GetMap();


	//		//Text* textEntity2 = new Text;
	//		//textEntity2->Initialize("text2", rectMesh);

	//		//Texture* textBoxTexture2 = TextureManager::GetTexture("TextBox.png");
	//		//textEntity2->GetModel().SetTextureToAllSubMesh(textBoxTexture2);
	//		////textEntity->GetTransform().SetScalingLocal({ 100.0f,100.0f,100.0f });
	//		//textEntity2->GetTransform().SetPositionLocal({ 0, 0, 100.0f });

	//		//mGamePlayUiSystem->AddUiEntity(textEntity2);


	//		//((Object*)textEntity2)->mSystem = mGamePlayUiSystem;
	//		//((Object*)textEntity2)->mMap = mGamePlayUiSystem->GetMap();




	}

	void DockingWindowController::LButtonDown(Event* event)
	{

		//어떤시스템으로 전달되어야하는지가 있을것이다.

		//ESystemID systemID = event->GetSystemID();

		//Object* capturedObject = Mouse::GetCapturedObject();
		//if (capturedObject != nullptr)
		//{

		//	event->SetSystemID(capturedObject->GetSystem()->GetSystemID());

		//	//capturedObject->OnEvent(event);
		//	//return;
		//}


		//if (systemID == mWindowDockingSystem->GetSystemID())
		//{
		//	mWindowDockingSystem->HandleEvent(event);
		//	return;
		//}
		//else if (systemID == mWindowLayoutSystem->GetSystemID())
		//{
		//	mWindowLayoutSystem->HandleEvent(event);
		//	return;
		//}else if (systemID == mUiSystem->GetSystemID())
		//{
		//	mUiSystem->HandleEvent(event);
		//	return;
		//}else if (systemID == mGameWorldSystem->GetSystemID())
		//{
		//	mGameWorldSystem->HandleEvent(event);
		//	return;
		//}
		//else  if (systemID == ESystemID::eNone)
		//{
		//	//딱히 지정되지않았다.
		//	//근데 한시스템에서 처리하고나오니 캡처되어있을수있다.'
		//	//이렇게보면 엄청복잡해
		//	//각 시스템에서 캡처가있는지 확인. 있다면 시스템id가 같은지 확인
		//	//그에따라 처리


			mWindowDockingSystem->HandleEvent(event);



			mWindowLayoutSystem->HandleEvent(event);


			//mUiSystem->HandleEvent(event);


			mWorldSystem->HandleEvent(event);
		//}




	}

	void DockingWindowController::RButtonDown(Event* event)
	{
		//어떤시스템으로 전달되어야하는지가 있을것이다.

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
		/*else if (systemID == mUiSystem->GetSystemID())
		{
			mUiSystem->HandleEvent(event);
			return;
		}*/
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


			//mUiSystem->HandleEvent(event);


			mWorldSystem->HandleEvent(event);
		}

	}

	void DockingWindowController::LButtonUp(Event* event)
	{

		TaskWindow* window = GetWindow();
		MouseEvent* mouseEvent = (MouseEvent*)event;

		mMouseClientPositionX = mouseEvent->GetClientPositionX();
		mMouseClientPositionY = mouseEvent->GetClientPositionY();

		if (mWindowDockingSystem != nullptr)
		{
			//프레임윈도우도 도킹시스템을가진다. 단 항상 도킹이 되어잇는 상태라고인식하여 바로 밑 코드는 수행되지않는다

			if (!window->GetDockingState() && GetDraggingWindowOnTitleBarState())
			{
				//타이틀바를 클릭하여 윈도우를 드래그하고있을때 LButtonUp을하여 윈도우이동을 종료
				//도킹될수있는지 없는지 판단하고 처리한다.
				POINT screenPosition = { mMouseClientPositionX ,mMouseClientPositionY };
				ClientToScreen(window->GetWindowHandle(), &screenPosition);
				SetDragginWindowOnTitleBarState(false);
				FrameWindowController::OffChildWindowDockingSystem(screenPosition.x, screenPosition.y);
				
				
			}
			else if (GetWindowDockingSystemState())
			{
				//외부 윈도우가 LButtonUp을 했을때
				//도킹시스템이 활성화된윈도우라면 
				//도킹판정을수행한다.

				//여기서 도킹시스템상태를 false로하면 프레임윈도우 도킹시스템이종료되는문제가생긴다
				//프레임윈도우의 도킹시스템은 종료되서는안된다.
				mWindowDockingSystem->HandleEvent(event);
				
				//도킹시스템상태 = false;
			}
		}

		Object* capturedObject = Mouse::GetCapturedObject();
		if (capturedObject != nullptr)
		{
			capturedObject->OnEvent(event);
			return;
		}
		
		mWindowLayoutSystem->HandleEvent(event);
		///mUiSystem->HandleEvent(event);
		mWorldSystem->HandleEvent(event);
	}

	void DockingWindowController::RButtonUp(Event* event)
	{
		//어떤시스템으로 전달되어야하는지가 있을것이다.

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
		/*else if (systemID == mUiSystem->GetSystemID())
		{
			mUiSystem->HandleEvent(event);
			return;
		}*/
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

	void DockingWindowController::LButtonDownDouble(Event* event)
	{

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
		/*else if (systemID == mUiSystem->GetSystemID())
		{
			mUiSystem->HandleEvent(event);
			return;
		}*/
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


	void DockingWindowController::MouseMove(Event* event)
	{
		MouseEvent* mouseEvent = (MouseEvent*)event;
		mMouseClientPositionX = mouseEvent->GetClientPositionX();
		mMouseClientPositionY = mouseEvent->GetClientPositionY();

		if (mWindowDockingSystem !=nullptr && GetWindowDockingSystemState())
		{
			mWindowDockingSystem->HandleEvent(event);
			return;
		}

		GamePlaySystem* gameSystem = dynamic_cast<GamePlaySystem*>(mWorldSystem);
		if (gameSystem != nullptr)
		{
			int a = 2;
		}


		Object * capturedObject =	Mouse::GetCapturedObject();
	
		if (capturedObject)
		{
			ESystemType systemType = capturedObject->GetSystem()->GetSystemType();
			if (systemType == ESystemType::eDockingSystem)
			{
				mWindowDockingSystem->HandleEvent(event);
			}
			else if (systemType == ESystemType::eWindowLayoutSystem)
			{
				mWindowLayoutSystem->HandleEvent(event);
			}
			else if (systemType == ESystemType::eMainSystem)
			{

				mWorldSystem->HandleEvent(event);
			}
		}
		else
		{
			mWindowDockingSystem->HandleEvent(event);
			mWindowLayoutSystem->HandleEvent(event);
			mWorldSystem->HandleEvent(event);

		}
			//mUiSystem->HandleEvent(event);






		
			//mWorldSystem->HandleEvent(event);
		




	}

	void DockingWindowController::KeyDown(Event* event)
	{
		ESystemID systemID = event->GetSystemID();


		Object* capturedObject = KeyBoard::GetCapturedObject();
		if (capturedObject != nullptr)
		{
			capturedObject->OnEvent(event);
			return;
		}



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
		/*else if (systemID == mUiSystem->GetSystemID())
		{
			mUiSystem->HandleEvent(event);
			return;
		}*/
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
		//이것도 어떤시스템이 포커싱되면 다른 시스템은 이벤트를 받으면안되는다.
		

	}

	void DockingWindowController::KeyUp(Event* event)
	{
		//어떤시스템으로 전달되어야하는지가 있을것이다.

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
		/*else if (systemID == mUiSystem->GetSystemID())
		{
			mUiSystem->HandleEvent(event);
			return;
		}*/
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


			//mUiSystem->HandleEvent(event);


			mWorldSystem->HandleEvent(event);
		}





	}

	void DockingWindowController::GamePlay(Event* event)
	{
		mWindowLayoutSystem->HandleEvent(event);
		//mUiSystem->HandleEvent(event);
	}

	void DockingWindowController::HandleWindowRequestEvent(Event* event)
	{
		WindowRequestEvent* pEvent = (WindowRequestEvent*)event;


		switch (pEvent->GetEventType())
		{
		case EWindowRequestEventType::eExitProgram:



			break;
		case EWindowRequestEventType::eSetWindowPosition:


			break;

		case EWindowRequestEventType::eWindowMove:	//제목표시줄을 클릭하여 윈도우를 움직인다.

			
			if(mWindow->GetDockingState())
			{ 
				FrameWindowController::ReleaseDockingChildWindow(mWindow);
			}
			SetDragginWindowOnTitleBarState(true);
			SetLayeredWindowAttributes(mWindow->GetWindowHandle(), 0, 100, LWA_ALPHA); // 200/255의 투명도 설정 (~78% 불투명)
			SendMessage(mWindow->GetWindowHandle(), WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);

		break;
		case EWindowRequestEventType::eWindowMoveEnd:
			//타이틀바를통한 윈도우 이동이 종료되었다.
			SetDragginWindowOnTitleBarState(false);
			SetLayeredWindowAttributes(mWindow->GetWindowHandle(), 0, 255, LWA_ALPHA);



		break;
		case EWindowRequestEventType::eWindowEnd:
			//SendMessage(mWindow->GetWindowHandle(), WM_SYSCOMMAND, HTCLOSE, 0);



			break;

		}



		
	}

	void DockingWindowController::HandleMouseInOut(Event* event)
	{
		mWindowLayoutSystem->HandleEvent(event);
	//	mUiSystem->HandleEvent(event);
		mWorldSystem->HandleEvent(event);
		

	}

	void DockingWindowController::HandleDefault(Event* event)
	{



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
		/*else if (systemID == mUiSystem->GetSystemID())
		{
			mUiSystem->HandleEvent(event);
			return;
		}*/
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



			//mUiSystem->HandleEvent(event);



			mWorldSystem->HandleEvent(event);
		}




	}


}