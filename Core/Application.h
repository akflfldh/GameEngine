#pragma once
// #include"header.h"

#include "Predeclare.h"
#include <RuntimeServices.h>
#include <wrl.h>
// using namespace Microsoft::WRL;

// #pragma comment(lib,"d2d1_1.lib")
#include "Core/CoreDllExport.h"
#include <memory>
#include <vector>

namespace SystemInitializer
{
class ISystemInitializer;
}
namespace UI
{
class UISystem;
}

namespace CoreAsset
{
class TextureManager;
}

namespace Import
{
class TextureImporter;

}

namespace Core
{
class LogicalWindow;
}

namespace Quad
{
template <typename collider> class SpacePartitioningStructureFactory;
class UIRenderItemBuilder;

struct AppInitData
{
    HINSTANCE hInstance;
    int nShowCmd;
    class IProgramDirector *programDirector;
    /*ICollisionWorldFactoryImpl* collisionWorldFactoryImpl;
    ISpacePartitioningStructureFactoryImpl* spacePartitoingStructureFactoryImpl;*/
};

class CORE_API_LIB Application
{
  public:
    static Application *GetInstance();
    Application();
    ~Application();

    bool Initialize(AppInitData &appInitData);
    int Run();

    virtual void EndSystem();

    void MoveToNextFrame();

    //	LRESULT Proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    // LRESULT FileWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    // void SetPlayModeState(bool state);
    //	static bool GetPlayModeState();

    // Microsoft::WRL::ComPtr<ID3D12Device> GetD3D12Device() const;
    // Microsoft::WRL::ComPtr<IDXGIFactory4> GetD3DFactory() const;
    // GraphicCommand& GetGraphicCommand();
    // DescriptorHeapManagerMaster* GetDescriptorHeapManagerMaster() ;
    HINSTANCE GetHinstance() const;

    void NotifyLogicalWindowActive(Core::LogicalWindow *logicalWindow);
    void RegisterLogicalWindow(Core::LogicalWindow *logicalWindow);
    void UnRegisterLogicalWindow(Core::LogicalWindow *logicalWindow);

    int GetCurrentFrameIndex() const;
    uint64_t GetCurrentFrameFenceValue() const;
    uint64_t GetTotalFrameCount() const;

    uint64_t GetLastCompletedFenceValue() const;

    int GetCurrentBackBufferIndex() const;

    void SetRenderStop(bool flag);
    void SetMinimizeFlag(bool flag);

  protected:
    Application(const Application &) = delete;
    Application &operator=(const Application &) = delete;
    Application(Application &&) = delete;
    Application &operator=(Application &&) = delete;

    void InitSystems();
    void InitCommonSystems();

    void InitReflectSystem();

    // void CreateInitGameWindowEntity(GamePlaySystem *system);

    void CreateCommandObjects();

    void Begin();
    void PreUpdate(float deltaTime);
    void Update(float deltaTime);
    void EndUpdate(float deltaTime);
    void CleanUp();

    void Draw(float timer);
    void EndFrame();

    void OnResize();
    void MouseDown(WPARAM wParam, int x, int y);
    void MouseUp(WPARAM wParam, int x, int y);
    void MouseMove(WPARAM wParam, int x, int y);

    void FlushCommandQueue();

    // float GetAspectRatio() const;

  protected:
    void CalculateFrameStats();

  protected:
    HINSTANCE mHinstance;
    int mShowcmd;
    HWND mHwnd;
    // HWND mHwndClient;
    HWND mRenderHwnd;
    HWND mFileHwnd;

    UINT mClientWidth = 1200;
    UINT mClientHeight = 1000;

    UINT mFileUiWindowWidth = 1200;
    UINT mFileUiWindowHeight = 300;

    UINT mRenderWindowWidth;
    UINT mRenderWindowHeight;

    // ComPtr<IDXGIFactory4> mFactory;
    // ComPtr<ID3D12Device> mDevice;
    // ComPtr<ID3D12Fence> mFence;
    // UINT64 mCurrentFence = 0;

    // ComPtr<ID3D12CommandQueue> mCommandQueue;
    // ComPtr<ID3D12CommandAllocator>mCommandAllocator;
    // ComPtr<ID3D12GraphicsCommandList>mGraphicscommandList;
    // GraphicCommand mGraphicCommandObject;

    // UINT mRtvdescriptorSize;
    // UINT mDsvdescriptorSize;
    // UINT mCbvsrvdescriptorSize;
    // UINT mSamplerdescriptorSize;

    // UINT m4xmsaaQuality;

    // DXGI_FORMAT mBackBufferForamt = DXGI_FORMAT_R8G8B8A8_UNORM;
    // DXGI_FORMAT mSwapchainDepthStencilBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    bool mIspaused = false;

    bool mIsMinimized = false;
    bool mIsMaximized = false;
    bool mIsResizing = false;

    bool mRenderStop = false;

  public:
  private:
    // 필요한 핵심 object들을 생성 ex) 화면전체렌더링을 위한 직사각형 메시와 object
    void CreateDefaultCoreResource();

  private:
    //	DescriptorHeapManagerMaster mDescriptorHeapManagerMaster;
    //	CbvSrvUavHeapManager mCbvSrvUavHeapManager;
    //	DsvHeapManager mDsvHeapManager;
    //	RtvHeapManager mRtvHeapManager;
    //	SamplerHeapManager mSamplerHeapManager;

    //	//HandleTable<Object*> mObjectHandleTable;

    //	CollisionHelper mCollisionHelper;
    //	ColliderGenerator mColliderGenerator;

    //	ResourceController mResourceController;
    //	MapController mMapController;
    //	MeshManager mMeshManager;
    //	MaterialManager mMaterialManager;
    //	TextureManager mTextureManager;
    ////	EffectManager mEffectManager;
    //	MapManager mMapManager;
    //	LineFactory mLineFactory;
    ////	EditObjectManager mEditObjectManager;
    //	//EditGameObjectManager mEditGameObjectManager;
    //	//RuntimeObjectManager mRuntimeObjectManager;
    //	//RuntimeGameObjectManager mRuntimeGameObjectManager;
    //	//ObjectFactory mObjectFactory;
    //	EditObjectFactory* mEditObjectFactory;
    //

    //	BoneFactory mBoneFactory;

    //	RenderSystem mRenderSystem;
    //	RenderSystem mUiRenderSystem;
    //	RenderSystem mDragAndDropRenderSystem;
    //	RenderSystem mPopupRenderSystem;

    //	//FrameWindowUiSystem* mFrameWindowUiSystem;
    //	//WindowChromeSystem* mGamePlayWindowChromeSystem;

    //	//int mCurrentActiveWindow = 0;

    //	ResourceLoader mResourceLoader;
    //	ResourceStorer mResourceStorer;
    //	//FileUISystem mFileUiSystem;
    //	//GamePlayUiSystem* mGamePlayUiSystem;

    //	//Quad::Map* mCurrMap;

    //	SpacePartitioningStructureFactory<Collider>* m3DSpacePartitioningStructureFactory;
    //	SpacePartitioningStructureFactory<UiCollider>* mUiSpacePartitioningStructureFactory;
    //	CollisionWorldFactory* mCollisionWorldFactory;

    //	CameraEventComponentFactory mCameraEventComponentFactory;

    //////////////////////////////////

    ///////////

    // window//
    // FrameWindow* mFrameWindow;
    // GameRenderWindow* mRenderWindowTest;
    // DockingWindowController* mGameWindowPlayController;
    // DockingWindowController* mFileUiWindowController;
    // DockingWindowController* mAttributeWindowController;
    // FrameWindowController* mFrameWindowController;
    // PopupWindowController* mPopupWindowController;

    // AttributeWindow* mAttributeWindow;
    // FileUiWindow* mFileUiWindow; //초기화
    // DragAndDropWindow* mDragAndDropWindow;
    // PopupWindow* mPopupWindow;
    //////

    // void CreateBitMap(FT_Bitmap bitmap);
    // void HarfBuzzTest();

    // void BitmapToTextureResource(FT_Bitmap bitmap);

    // Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;
    // Microsoft::WRL::ComPtr<ID3D12Resource> mTextTexture;

    //	TextFactory mTextFactory;
    // KeyBoard mKeyBoard;
    //	Mouse mMouse;

    // EventDispatcher mEventDispatcher;
    // TextureFactory mTextureFactory;

    bool mPlayModeState = false;

    // CreatingProjectDirector mCreatingProjectDirector;
    /*	ProjectDirector mProjectDirector;


        FrameWindowMenuDirector mFrameWindowMenuDirector;
        EditorModeDirector mEditorModeDirector;
*/

    // InputSystem mInputSystem;
    IProgramDirector *mProgramDirector;

    //	CurveFactory mCurveFactory;

    //	RootSignatureGeneratorHelper mRootSignatureGeneratorHelper;
    //	GraphicPipelineStateGeneratorHelper mGraphicPipelineStateGeneratorHelper;
    //	CoreEffectInstaller mCoreEffectInstaller;

    std::unique_ptr<SystemInitializer::ISystemInitializer> mSystemInitializer;
    //	std::unique_ptr< UIRenderItemBuilder> mUIRenderItemBuilder;
    //	std::unique_ptr<UI::UISystem> mUISystem;

    std::unique_ptr<CoreAsset::TextureManager> mTextureManager;

    std::vector<Core::LogicalWindow *> mLogicalWindowList;
    Core::LogicalWindow *mCurrentActiveLogcialWindow;

    int mCurrentFrame;
    uint64_t mTotalFrameCount = 0;
    uint64_t mFrameFenceValue[3];
    uint64_t mLastCompletedFenceValue = 0;
    int mBackbufferIndex;

    RuntimeServices mRuntimeServiecs;
};

} // namespace Quad