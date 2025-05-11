#pragma once
#include"Core/resource.h"
#include"header.h"
#include"Core/GameTimer.h"
#include"Utility/DoException.h"
#include"Utility/Singleton.h"

#include"Predeclare.h"



#include"HeapManager/DescriptorHeapManagerMaster.h"
#include"HeapManager/CbvSrvUavHeapManager.h"
#include"HeapManager/DsvHeapManager.h"
#include"HeapManager/RtvHeapManager.h"
#include"HeapManager/SamplerHeapManager.h"



#include"Core/ResourceController.h"
//#include"GamePlayWindowController.h"


#include"Core/MapController.h"
#include"ResourceManager/MeshManager.h"
#include"ResourceManager/MaterialManager/MaterialManager.h"
#include"ResourceManager/TextureManager/TextureManager.h"
#include"MapManager.h"
#include"RenderSystem.h"
#include"ResourceLoader.h"
#include"ResourceStorer.h"
//#include"FileUISystem.h"
#include"Map/Map.h"
#include"Event/EventTable.h"
#include"Core/GraphicCommand.h"
#include"ObjectFactory/LineFactory.h"



//#include"GameRenderWindow.h"
//#include"PopupWindow.h"

//#include"WorldEditEntityFactory.h"


#include"Core/KeyBoard.h"
#include"Core/Mouse.h"


#include"harfBuzzsrc/hb.h"
#include"harfBuzzsrc/hb-ft.h"
#include"ft2build.h"
#include FT_FREETYPE_H
#include"ObjectFactory/TextFactory.h"

#include"Collision/CollisionHelper.h"
#include"Collision/ColliderGenerator.h"
#include"Core/EventDispatcher.h"
#include"ResourceFactory/TextureFactory/TextureFactory.h"

#include"Utility/HandleTable.h"

#include"ResourceManager/EffectManager/EffectManager.h"
#include"ObjectFactory/ObjectFactory.h"

#include"ObjectManager/ObjectManager.h"
#include"ObjectManager/RuntimeObjectManager.h"
#include"ObjectManager/RuntimeGameObjectManager.h"
#include"ObjectManager/EditGameObjectManager.h"

#include"Core/InputSystem.h"

#include"Core/BoneFactory.h"
//#include"CreatingProjectDirector.h"
//#include"ProjectDirector.h"


//#include"FrameWindowMenuDirector.h"
//#include"EditorModeDirector.h"


using namespace Microsoft::WRL;

//#pragma comment(lib,"d2d1_1.lib")

//한메인스레드에서 여러창을 띄울수있는데
//그럼이것을 싱글톤으로 만들어서는안된다.
//근데 windows메세지처리에서 인스턴스에대한 포인터를 가져올방법이 있나?
//쉽지않아.
//창의 분리,결합

#include"Core/CameraEventComponentFactory.h"

#include"Core/CoreDllExport.h"
#include"Core/CurveFactory.h"

#include"SpacePartitioningStructureFactory.h"


namespace Quad
{
	template<typename collider>
	class SpacePartitioningStructureFactory; 


	struct AppInitData
	{
		HINSTANCE hInstance;
		int nShowCmd;
		IProgramDirector* programDirector;
		ICollisionWorldFactoryImpl* collisionWorldFactoryImpl;
		ISpacePartitioningStructureFactoryImpl* spacePartitoingStructureFactoryImpl;

	};



	class CORE_API_LIB Application :public Singleton<Application>
	{
	public:
		Application();
		~Application();

		bool Initialize(AppInitData & appInitData);
		int Run();
		LRESULT Proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		//LRESULT FileWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		//void SetPlayModeState(bool state);
	//	static bool GetPlayModeState();


		Microsoft::WRL::ComPtr<ID3D12Device> GetD3D12Device() const;
		Microsoft::WRL::ComPtr<IDXGIFactory4> GetD3DFactory() const;
		GraphicCommand& GetGraphicCommand();
		DescriptorHeapManagerMaster* GetDescriptorHeapManagerMaster() ;
		HINSTANCE GetHinstance() const;

	protected:

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;
		Application(Application&&) = delete;
		Application& operator=(Application&&) = delete;

		bool InitD3d();
		void InitSystems();
		void InitCommonSystems();
		/*void InitGamePlayWindow();
		void InitFileUiWindow();  
		void InitAttributeWindow();
		void InitFrameWindow();
		void InitDragAndDropWindow();
		void InitPopupWindow();*/


		/*void InitGameProject(TaskWindow* window, DockingWindowController* controller,
			GamePlaySystem* gamePlaySystem, GamePlayUiSystem* gamePlayUiSystem, 
			WindowChromeSystem* windowLayoutSystem, ChildWindowDockingSystem* windowDockingSystem);
		void InitGamePlaySystem(TaskWindow* window,GamePlaySystem * system);
		void InitGamePlayUiSystem(TaskWindow* window,GamePlayUiSystem* system);
		void InitAttributeSystem(TaskWindow* window,AttributeSystem* system);
		void InitAttributeUiSystem(TaskWindow* window,AttributeUiSystem* system);
		void InitFileUiUiSystem(TaskWindow* windows, FileUiUiSystem* system);
		void InitFileUiSystem(TaskWindow* windows, FileUiSystem* system);

		void InitFrameWindowSystem(TaskWindow* window, FrameWindowSystem* system);
		void InitFrameWindowUiSystem(TaskWindow* window, FrameWindowUiSystem* system);




		void InitGamePlayWindowLayoutSystem(TaskWindow* window, WindowChromeSystem* system);
		void InitAttributeWindowLayoutSystem(TaskWindow* window, WindowChromeSystem* system);
		void InitFileUiWindowLayoutSystem(TaskWindow* window, WindowChromeSystem* system);
		void InitFrameWindowLayoutSystem(TaskWindow* window, WindowChromeSystem* system);

		
		void InitFrameWindowDockingSystem(TaskWindow * window, FrameWindowDockingSystem* system);
		void InitChildWindowDockingSystem(TaskWindow * window, ChildWindowDockingSystem * system);*/



		void CreateInitGameWindowEntity(GamePlaySystem* system);



		void CreateCommandObjects();

		void PreUpdate(GameTimer& timer);
		void Update(GameTimer& timer);
		void EndUpdate(GameTimer& timer);

		void Draw(GameTimer& timer);
		void OnResize();
		void MouseDown(WPARAM wParam, int x, int y);
		void MouseUp(WPARAM wParam, int x, int y);
		void MouseMove(WPARAM wParam, int x, int y);


		void FlushCommandQueue();

		float GetAspectRatio() const;

		


	
	protected:

		void CalculateFrameStats();





	protected:

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





		ComPtr<IDXGIFactory4> mFactory;
		ComPtr<ID3D12Device> mDevice;
		ComPtr<ID3D12Fence> mFence;
		UINT64 mCurrentFence = 0;

		ComPtr<ID3D12CommandQueue> mCommandQueue;
		ComPtr<ID3D12CommandAllocator>mCommandAllocator;
		ComPtr<ID3D12GraphicsCommandList>mGraphicscommandList;
		GraphicCommand mGraphicCommandObject;



		UINT mRtvdescriptorSize;
		UINT mDsvdescriptorSize;
		UINT mCbvsrvdescriptorSize;
		UINT mSamplerdescriptorSize;

		UINT m4xmsaaQuality;


		DXGI_FORMAT mBackBufferForamt = DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGI_FORMAT mSwapchainDepthStencilBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;


		GameTimer mGameTimer;
		bool mIspaused = false;

		bool mIsMinimized = false;
		bool mIsMaximized = false;
		bool mIsResizing = false;





	public:


	private:

		void AddEffect(RenderSystem* renderSystem, Effect* effect);












	private:
		DescriptorHeapManagerMaster mDescriptorHeapManagerMaster;
		CbvSrvUavHeapManager mCbvSrvUavHeapManager;
		DsvHeapManager mDsvHeapManager;
		RtvHeapManager mRtvHeapManager;
		SamplerHeapManager mSamplerHeapManager;


		//HandleTable<Object*> mObjectHandleTable;



		CollisionHelper mCollisionHelper;
		ColliderGenerator mColliderGenerator;

		ResourceController mResourceController;
		MapController mMapController;
		MeshManager mMeshManager;
		MaterialManager mMaterialManager;
		TextureManager mTextureManager;
	//	EffectManager mEffectManager;
		MapManager mMapManager;
		LineFactory mLineFactory;
		EditObjectManager mEditObjectManager;
		EditGameObjectManager mEditGameObjectManager;
		RuntimeObjectManager mRuntimeObjectManager;
		RuntimeGameObjectManager mRuntimeGameObjectManager;
		//ObjectFactory mObjectFactory;
		EditObjectFactory* mEditObjectFactory;
		

		BoneFactory mBoneFactory;

		RenderSystem mRenderSystem;
		RenderSystem mUiRenderSystem;
		RenderSystem mDragAndDropRenderSystem;
		RenderSystem mPopupRenderSystem;


		//FrameWindowUiSystem* mFrameWindowUiSystem;
		//WindowChromeSystem* mGamePlayWindowChromeSystem;

		//int mCurrentActiveWindow = 0;


		ResourceLoader mResourceLoader;
		ResourceStorer mResourceStorer;
		//FileUISystem mFileUiSystem;
		//GamePlayUiSystem* mGamePlayUiSystem;


		//Quad::Map* mCurrMap;





		SpacePartitioningStructureFactory<Collider>* m3DSpacePartitioningStructureFactory;
		SpacePartitioningStructureFactory<UiCollider>* mUiSpacePartitioningStructureFactory;
		CollisionWorldFactory* mCollisionWorldFactory;

		CameraEventComponentFactory mCameraEventComponentFactory;

		//////////////////////////////////





		///////////


		//window//
		//FrameWindow* mFrameWindow;
		//GameRenderWindow* mRenderWindowTest;
		//DockingWindowController* mGameWindowPlayController;
		//DockingWindowController* mFileUiWindowController;
		//DockingWindowController* mAttributeWindowController;
		//FrameWindowController* mFrameWindowController;
		//PopupWindowController* mPopupWindowController;


		//AttributeWindow* mAttributeWindow;
		//FileUiWindow* mFileUiWindow; //초기화
		//DragAndDropWindow* mDragAndDropWindow;
		//PopupWindow* mPopupWindow;
		//////
		
		


		void CreateBitMap(FT_Bitmap bitmap);
		void HarfBuzzTest();

		void BitmapToTextureResource(FT_Bitmap bitmap);
	
		Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> mTextTexture;


		TextFactory mTextFactory;
		KeyBoard mKeyBoard;
		Mouse mMouse;



	

		EventDispatcher mEventDispatcher;
		//TextureFactory mTextureFactory;








		bool mPlayModeState= false;



		




		//CreatingProjectDirector mCreatingProjectDirector;
	/*	ProjectDirector mProjectDirector;


		FrameWindowMenuDirector mFrameWindowMenuDirector;
		EditorModeDirector mEditorModeDirector;
*/







		InputSystem mInputSystem;
		IProgramDirector* mProgramDirector;

		CurveFactory mCurveFactory;

		
	};


}