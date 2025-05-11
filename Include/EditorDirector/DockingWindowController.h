#pragma once

#include"Predeclare.h"
#include"header.h"
#include"Core/BaseWIndowController.h"
#include"Utility/ReaderWriter.h"
#include"Utility/CircleQueue.h"
#include"EditorDirector/Window.h"


namespace Quad
{

	class DockingWindowController:public BaseWindowController, public ReaderWriter
	{
		friend class ProjectDirector;
	public:
		DockingWindowController(RenderSystem * renderSystem);
		~DockingWindowController() = default;


		virtual void Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device,
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> graphicsCommandList,
			MeshManager * meshManager, MaterialManager * materialManager, TextureManager * textureManager,
			MapManager * mapManager, TaskWindow * window ,
			System* worldSystem, WindowChromeSystem* windowLayoutSystem,
			WindowDockingSystem * windowDockingSystem,
			DescriptorHeapManagerMaster * descriptorHeapManagerMaster, bool playMode=true);


	
		//event 메세지가 오면 적절하게 처리한다.
		//

	
		virtual void Update(float deltaTime);
		virtual void UploadObjectToRenderSystem();
		virtual void EndUpdate(float deltaTime);
		virtual void Draw();

		virtual void OnResize(int clientWidth, int clientHeigh, int direction);

		// virtual void AddEvent(Event * event) override;

		virtual TaskWindow* GetWindow()const override;

		void SetBackgroundColor(float r, float g, float b, float a);

		virtual void WindowActivate(int wmActivateState);
		virtual void WindowMove(int preWindowPosX,int preWindowPosY, int currWindowPosX, int currWindowPosY);
		virtual void WindowMinimize();
		
	
		//아무것도하지않는다 필요하면 오버라이딩할것
		virtual void DragAndDropFile(const std::vector<std::string>& dropFileVector);
	//	virtual void DragAndDropFile(std::vector<std::string>&& dropFileVector);



		//도킹시스템을 처음시작하든,작동중일떄에도 계속 마우스의 위치를 받는다.
		virtual void OnDockingSystem(int mouseScreenPositionX, int mouseScreenPositionY);
		virtual void OffDockingSystem();	//mouse가 창밖으로 나가서 도킹시스템종료
		virtual void EndDockingSystem(int mouseScreenPositionX, int mouseScreenPositionY);	//LButtonUp으로인해 도킹시스템종료+프레임윈도우에게도킹작업요청
		//도킹시스템이켜진윈도우는 현재 드래그되는 창이 메세지루프를 점유하고있어서 직접 업데이트,렌더해주어야한다.
		void UpdateAndDrawDockingWindow();


		void SetTitleBarSize(bool usage, UINT width, UINT height);
		int GetTitleBarHeight();
		int GetLogicalClientWidth()const;
		int GetLogicalClientHeight()const;

		int GetEdgeDragDirection() const;
		void SetEdgeDragDirection(int direction);

		void SetDragginWindowOnTitleBarState(bool state);
		bool GetDraggingWindowOnTitleBarState()const;


		bool GetMousePositionState()const;
		void SetMousePositionState(bool state);
		void StartTrackMouseWindowOut();



		void SetWindowShowState(EWindowShowState showState);
		EWindowShowState GetWindowShowState()const;

		void SetRenderState(bool state);
		bool GetRenderState()const;


		virtual void HandleWindowReStore();


		bool GetWindowDockingSystemState() const;

		void SetDuplicateResizeFlag(bool state);
		bool GetDuplicateResizeFlag()const;

		void AddEffect(Effect* effect);


		void SetPlayMode(bool playMode);
		bool GetPlayMode()const;


		void SetPause(bool state);
		bool GetPause()const;


		void SetWindowActiveFlag(bool flag);
		bool GetWindowActiveFlag()const;



		void SetSystemActiveState(int systemNum, bool state);		//0: main, 1" layout , 2: docking


	protected:
		virtual void HandleEvent(Event *pEvent);
		

		
		//여러 functions
		//Event* GetEvent();


		MeshManager* mMeshManager;
		MaterialManager* mMaterialManager;
		TextureManager* mTextureManager;
		MapManager* mMapManager;
	//	RenderSystem* mRenderSystem;
		//System* mUiSystem;
		System* mWorldSystem;
		WindowChromeSystem* mWindowLayoutSystem;
		//도킹시스템
		WindowDockingSystem* mWindowDockingSystem;//프레임윈도우는 가질이유가없다.
		DescriptorHeapManagerMaster* mDescriptorHeapManagerMaster;
		TaskWindow* mWindow;


	protected:
		//mouse -일단 임시 mouse system을 생각해야한다.
		int mMouseClientPositionX;
		int mMouseClientPositionY;


	private:
		void InitGamePlayUiSystem();

		void LButtonDown(Event * event);
		void RButtonDown(Event * event);
		void LButtonUp(Event * event);
		void RButtonUp(Event* event);
		void LButtonDownDouble(Event* event);
		
		virtual void MouseMove(Event * event);
		void KeyDown(Event * event);
		void KeyUp(Event * event);
		void GamePlay(Event * event);
		void HandleWindowRequestEvent(Event* event);
		void HandleMouseInOut(Event* event);

		void HandleDefault(Event* event);


	
	private:
		Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mGraphicsCommandList;


		//Quad::Map* mCurrentMap;		//currentMap이 잘못된거같다	, 이거는 system에들어가야하는게아닐까
		//CircleQueue<Event*> mEventQueue;
		//ReaderWriter mEventQueueSynObject;
		
	
		UINT mLogicalWindowWidth;
		UINT mLogicalWindowHeight;

		UINT mLogicalClientWidth;
		UINT mLogicalClientHeight;
		int mEdgeDragDirection = 0;			// 0 : none, 1: left, 2: right, 3: top ,4 : bottom ,5 : all 


		
		bool mMousePositionState=false;		//화면밖인가 안에있는가
		//아니면 최소화여부는 체크할필요가있나? 없어도될거같은데.
		//최소화상태를 나타낼이유가없다,-아직까지
		EWindowShowState mWindowShowState =EWindowShowState::eNormal;

		bool mIsDraggingWindowOnTitleBar;

	//bool mDockingSystemState();
		

		bool mDuplicateResizeState=false;


		std::unordered_map<std::string, Effect*> mEffectTable;



		//기본적으로 true (즉 항상런타임실행중)
		//단 gameWindow의 ui,main system은 false로 시작한다.
		bool mPlayMode = true;
		bool mPauseState = false;



		bool mWindowActiveFlag = false;






	};
}

