#pragma once
#include"header.h"
#include"Predeclare.h"
#include"Core/IProgramDirector.h"
#include"Core/GraphicCommand.h"
#include"Core/RenderSystem.h"
#include"ObjectManager/ObjectManager.h"
#include"ObjectManager/RuntimeObjectManager.h"
#include"ObjectManager/EditGameObjectManager.h"
#include"ObjectManager/RuntimeGameObjectManager.h"

#include"EditorDirector/ProjectDirector.h"
#include"EditorDirector/FrameWindowMenuDirector.h"
#include"EditorDirector/EditorModeDirector.h"

#include"Utility/Singleton.h"
#include"BuildModule.h"

#define EngineMode

namespace Quad
{
	class FileUiSystem;
	class ImportModule;

	class EditorDirector:public IProgramDirector ,public Singleton<EditorDirector>
	{
	public:
		EditorDirector();
		~EditorDirector()=default;


		virtual void Initialize() override; 


		virtual void PreUpdate(float deltaTime) override;
		virtual void Update(float deltaTime) override;
		virtual void EndUpdate(float deltaTime) override;


		virtual void Draw() override;

		void SetPlayModeState(bool state);
		static bool GetPlayModeState();


		const std::string& GetEditorPathA() const;
		const std::wstring& GetEditorPathW() const;

		//프레임윈도우씬으로 화면전환
		//다른 윈도우들은 정지 그리고 사라진다.

		//매프레임첫시작에 상태를보고 전환한다.
		static void SetSwitchWindowSceneModeFlag(int flag);
		
		const int GetSwitchWindowSceneModeFlag()const;


	private:
		void InitGamePlayWindow();
		void InitFileUiWindow();
		void InitAttributeWindow();
		void InitFrameWindow();
		void InitDragAndDropWindow();
		void InitPopupWindow();


		void InitGameProject(TaskWindow* window, DockingWindowController* controller,
			GamePlaySystem* gamePlaySystem, 
			WindowChromeSystem* windowLayoutSystem, ChildWindowDockingSystem* windowDockingSystem);
		void InitGamePlaySystem(TaskWindow* window, GamePlaySystem* system);
		void InitGamePlayUiSystem(TaskWindow* window, GamePlayUiSystem* system);
		//void InitAttributeSystem(TaskWindow* window, AttributeSystem* system);
		void InitAttributeUiSystem(TaskWindow* window, AttributeUiSystem* system);
		void InitFileUiUiSystem(TaskWindow* windows, FileUiUiSystem* system);
		void InitFileUiSystem2(TaskWindow* windows, FileUiSystem* system);

		//void test(TaskWindow* windows, FileUiSystem* system);

		//void InitFrameWindowSystem(TaskWindow* window, FrameWindowSystem* system);
		void InitFrameWindowUiSystem(TaskWindow* window, FrameWindowUiSystem* system);




		void InitGamePlayWindowLayoutSystem(TaskWindow* window, WindowChromeSystem* system);
		void InitAttributeWindowLayoutSystem(TaskWindow* window, WindowChromeSystem* system);
		void InitFileUiWindowLayoutSystem(TaskWindow* window, WindowChromeSystem* system);
		void InitFrameWindowLayoutSystem(TaskWindow* window, WindowChromeSystem* system);


		void InitFrameWindowDockingSystem(TaskWindow* window, FrameWindowDockingSystem* system);
		void InitChildWindowDockingSystem(TaskWindow* window, ChildWindowDockingSystem* system);



		//gamePlaySystem말고, 다른 window들의 시스템들의 맵 초기화 (맵 레이어생성,충돌공간생성,설정)
		void initDefaultMapSetting(TaskWindow* window, Map* currMap);






		void AddVisibleEditorAssetToFileWindow(const std::string& configFilePath);



#ifdef EngineMode

		void CreateDefaultMaterial();
		void CreateDefaultMesh();
#endif 


		//switchFrameWindowSceneFlag를보고 프레임윈도우(단독)으로 전환한다.
		void SwitchFrameWindow();
		void SwitchCommonEditWindow();








		HINSTANCE mHinstance;
		int mShowcmd;
		HWND mHwnd;
		//HWND mHwndClient;
		HWND mRenderHwnd;
		HWND mFileHwnd;

		UINT mClientWidth = 1200;
		UINT mClientHeight = 1000;

		UINT mFileUiWindowWidth = 1200;
		UINT mFileUiWindowHeight = 300;

		UINT mRenderWindowWidth;
		UINT mRenderWindowHeight;

		GraphicCommand*  mGraphicCommandObject;



		DXGI_FORMAT mBackBufferForamt = DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGI_FORMAT mSwapchainDepthStencilBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;


		bool mIspaused = false;
		bool mIsMinimized = false;
		bool mIsMaximized = false;
		bool mIsResizing = false;

	//	bool mPlayModeState = false;

	private:

		void AddEffect(RenderSystem* renderSystem, Effect* effect);



	private:

		Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
		Microsoft::WRL::ComPtr<IDXGIFactory4> mFactory;
		DescriptorHeapManagerMaster* mDescriptorHeapManagerMaster;
		

	private:

		//EditObjectManager mEditObjectManager;
		//EditGameObjectManager mEditGameObjectManager;
		//RuntimeObjectManager mRuntimeObjectManager;
		//RuntimeGameObjectManager mRuntimeGameObjectManager;

		RenderSystem mRenderSystem;
		RenderSystem mUiRenderSystem;
		RenderSystem mDragAndDropRenderSystem;
		RenderSystem mPopupRenderSystem;
		FrameWindowUiSystem* mFrameWindowUiSystem;
		WindowChromeSystem* mGamePlayWindowChromeSystem;

		int mCurrentActiveWindow = 0;



		
		//controller//
		DockingWindowController* mGameWindowPlayController;
		DockingWindowController* mFileUiWindowController;
		DockingWindowController* mAttributeWindowController;
		FrameWindowController* mFrameWindowController;
		PopupWindowController* mPopupWindowController;

		//window//
		GameRenderWindow* mRenderWindowTest;
		FrameWindow* mFrameWindow;
		AttributeWindow* mAttributeWindow;
		FileUiWindow* mFileUiWindow; //초기화
		DragAndDropWindow* mDragAndDropWindow;
		PopupWindow* mPopupWindow;
		////



		bool mPlayModeState = false;

		
		//CreatingProjectDirector mCreatingProjectDirector;
		ProjectDirector mProjectDirector;


		FrameWindowMenuDirector mFrameWindowMenuDirector;
		EditorModeDirector mEditorModeDirector;



		ImportModule* mImportMoudle;



		BuildModule mBuildModule;


		//에디터 실행파일이 존재하는 경로(editor.exe는 포함안됨)
		std::string mEditorPathA;
		std::wstring mEditorPathW;




		//0 : 상태유지 , 1:단독프레임윈도우모드로 전환, 2 : 일반에디터모드
		int mFrameWindowSceneSwitch;
		bool mWindowSwitchRenderFlag;//윈도우들을 스위치할때, 클리어,렌더가 수행되었으니, 창들을 띄워도된다는것을 나타내는 상태플래그
		




	};


}