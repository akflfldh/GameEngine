#include "Core/Application.h"
#include <tchar.h>

#include "Core/GlobalAppHelper.h"
#include <D3DGpuResourceManager/IGpuResourceManager.h>

#include "Core/ProjectConfig.h"
#include <Core/Component.h>
#include <Core/IProgramDirector.h>
#include <Core/IRenderProxyManager.h>
#include <Core/LogicalWindow.h>
#include <Core/ObjectController.h>
#include <CoreAllocator/AllocatorManager.h>
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/TextureManager.h>
#include <CoreBase/MainTimer.h>
#include <CoreBase/TimerManager.h>
#include <CoreDevice/CoreDevice.h>
#include <D3DGpuResourceManager/GpuBufferContextSystem.h>
#include <InputSystem/InputSystem.h>
#include <Logger/Logger.h>
#include <ReflectSystem/ReflectionSystem.h>
#include <RenderSystem/IRenderSystem.h>
#include <UISystem/UIManager.h>
#ifdef D3DX

#include <SystemInitializer/D3DSystemInitializer.h>

#endif

namespace Quad
{

Application *Application::GetInstance()
{
    static Application instance;

    return &instance;
}

Application::Application() : mCurrentActiveLogcialWindow(nullptr), mCurrentFrame(-1), mBackbufferIndex(-1)
{
    // mEditObjectFactory = EditObjectFactory::GetInstance();
}

Application::~Application()
{
    FlushCommandQueue(); // 명령대기열에 명령이남아있는데 종료하게되면 gpu 가 충돌(crash)할수있다.
}

bool Application::Initialize(AppInitData &appInitData)
{

    mHinstance = appInitData.hInstance;
    mShowcmd = appInitData.nShowCmd;

    Core::GlobalAppHelper *globalAppHelper = Core::GlobalAppHelper::GetInstance();
    globalAppHelper->Initialize(mHinstance);

    UI::UIManager::GetInstance()->mMouseCaptureCallbackSystem.Register([globalAppHelper]()
                                                                       { globalAppHelper->NotifyMouseCapture(); });

    UI::UIManager::GetInstance()->mMouseReleaseCaptureCallbackSystem.Register(
        [globalAppHelper]() { globalAppHelper->NotifyMouseReleaseCapture(); });

    UI::UIManager::GetInstance()->mKeyboardCaptureCallbackSystem.Register(
        [globalAppHelper]() { globalAppHelper->NotifyKeyboardCapture(); });

    UI::UIManager::GetInstance()->mKeyboardReleaseCaptureCallbackSystem.Register(
        [globalAppHelper]() { globalAppHelper->NotifyKeyboardReleaseCapture(); });

    mProgramDirector = appInitData.programDirector;

    // rojectConfig::GetInstance();

    InitSystems();

    return true;
}

int Application::Run()
{

    Begin();

#ifdef D3DX

    MSG msg = {0};

    TimerManager::GetInstance()->GetMainTimer().Reset();

    // TimerManager.GameTimer Reset

    while (msg.message != WM_QUIT)
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (msg.message == WM_QUIT)
            break;

        // TimerManager.Tick();
        TimerManager::GetInstance()->GetMainTimer().Tick();
        float deltaTime = TimerManager::GetInstance()->GetMainTimer().GetDeltaTime();

        if (!mIspaused)
        {
            CalculateFrameStats();
            // 게임코드
            PreUpdate(deltaTime);
            Update(deltaTime);
            EndUpdate(deltaTime);
            CleanUp();

            Draw(deltaTime);
            EndFrame();
        }
        else
        {
            Sleep(50);
        }
    }

    EndSystem();

    return (int)msg.wParam;
#endif
}

void Application::EndSystem()
{

    if (mProgramDirector)
    {
        mProgramDirector->EndSystem();
    }
}

void Application::MoveToNextFrame()
{

    if (mRenderStop == false)
    {

        mCurrentFrame = (mCurrentFrame + 1) % 3;
        mBackbufferIndex = (mBackbufferIndex + 1) % 2;

        // 해당프레임의 작업이 끝났는지확인
        // 끝나지않았다면 기다린다.
        Core::CoreDevice *coreDevice = Core::CoreDevice::GetInstance();

        coreDevice->WaitFenceValue(mFrameFenceValue[mCurrentFrame]);

        mLastCompletedFenceValue = mFrameFenceValue[mCurrentFrame];

        // 새로운펜스설정
        mTotalFrameCount++;

        mFrameFenceValue[mCurrentFrame] = coreDevice->GetNextFenceValue();
        coreDevice->IncreaseNextFenceValue();
    }
    else
    {
        mCurrentFrame = (mCurrentFrame + 1) % 3;
        Core::CoreDevice *coreDevice = Core::CoreDevice::GetInstance();
        coreDevice->WaitFenceValue(mFrameFenceValue[mCurrentFrame]);
        mLastCompletedFenceValue = mFrameFenceValue[mCurrentFrame];
        // 새로운펜스설정
        mTotalFrameCount++;
    }
}

void Application::InitSystems()
{

    ReflectionSystem::GetInstance()->Initialize();

#ifdef D3DX
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    mSystemInitializer =
        std::unique_ptr<SystemInitializer::ISystemInitializer>(new D3DSystemInitializer::D3DSystemInitializer());

    SystemInitializer::ISystemInitializer::SetSystemInitializerImpl(mSystemInitializer.get());
    mSystemInitializer->Initialize();

#endif

    InitReflectSystem();
    InitCommonSystems();

    return;
}

void Application::InitCommonSystems()
{
    mTextureManager = std::make_unique<CoreAsset::TextureManager>();

    mRuntimeServiecs.mAssetManager = CoreAsset::AssetManager::GetInstance();
    mRuntimeServiecs.mUIManager = UI::UIManager::GetInstance();
    mRuntimeServiecs.mRenderSystem = Render::IRenderSystem::GetInstance();
    mRuntimeServiecs.mGpuResourceManager = GRM::IGpuResourceManager::GetInstance();

    mProgramDirector->Initialize(mRuntimeServiecs);
}

void Application::InitReflectSystem()
{
    Quad::ReflectionSystem *reflectionSystem = Quad::ReflectionSystem::GetInstance();
    CoreAlloc::AllocatorManager *allocatorManager = CoreAlloc::AllocatorManager::GetInstance();
    reflectionSystem->RegisterObjectMemoryAllocCallback([allocatorManager](size_t size, size_t alignment)
                                                        { return allocatorManager->DefaultAllocate(size, alignment); });
    reflectionSystem->RegisterObjectMemoryReleaseCallback([allocatorManager](void *pMem)
                                                          { allocatorManager->DefaultRelease(pMem); });

    reflectionSystem->RegisterObjectIDGetterCallback(
        [](void *pMem)
        {
            Object *ob = static_cast<Object *>(pMem);

            return ob->GetUniqueID();
        });

    reflectionSystem->RegisterComponentIDGetterCallback(
        [](void *pMem)
        {
            Component *com = static_cast<Component *>(pMem);
            CoreUtility::UniqueID id = com->GetUniqueID();

            uint32_t addr1 = (uint32_t)&com->GetUniqueID().mUniqueID;
            uint32_t addr2 = (uint32_t)&id.mUniqueID;

            for (int i = 0; i < 16; ++i)
            {
                char t1 = id.mUniqueID[i];

                char t2 = com->GetUniqueID().mUniqueID[i];

                int a = 2;
            }

            return id;
        });
}

void Application::CreateCommandObjects() {}

void Application::Begin()
{

    UI::UIManager *uiManager = UI::UIManager::GetInstance();
    if (uiManager)
    {
        uiManager->Begin();
    }

    mProgramDirector->Begin();
}

void Application::PreUpdate(float deltaTime)
{
    if (mIsMinimized)
    {
        mRenderStop = true;
    }
    else
    {
        mRenderStop = false;
    }

    MoveToNextFrame();

    InputSystem *inputSystem = InputSystem::GetInstance();
    if (inputSystem)
    {
        inputSystem->Update();
    }

    // float deltaTime = timer.GetDeltaTime();

    // GRM::GpuBufferContextSystem *gpuBufferContextSystem = GRM::GpuBufferContextSystem::GetInstance();
    // if (gpuBufferContextSystem != nullptr)
    //     gpuBufferContextSystem->PreUpdate();

    mProgramDirector->PreUpdate(deltaTime);
}

void Application::Update(float deltaTime)
{
    //  mResourceController.Update();

    // 사전에 effect들의 shader resource들을 초기화(특히,상수버퍼,구조적버퍼)

    // 아마 리소스 업로드문제 윈도우별,윈도우안의 시스템별
    // 업데이트순서문제로인해 깜빡거림 발생한는거같다.

    //  float deltaTime = timer.GetDeltaTime();

    mProgramDirector->Update(deltaTime);
}

void Application::EndUpdate(float deltaTime)
{
    //  float deltaTime = timer.GetDeltaTime();
    mProgramDirector->EndUpdate(deltaTime);

    // mMouse.EndUpdate();

    // mEditGameObjectManager.RemoveDeadObject();
    // mEditObjectManager.RemoveDeadObject();
    // mRuntimeGameObjectManager.RemoveDeadObject();
    // mRuntimeObjectManager.RemoveDeadObject();
}

void Application::CleanUp()
{

    mProgramDirector->CleanUp();
}

void Application::Draw(float deltaTime)
{
    mProgramDirector->Draw();
}

void Application::EndFrame()
{
    mProgramDirector->EndFrame();
    InputSystem::GetInstance()->EndFrame();

    UI::UIManager::GetInstance()->EndFrame();

    auto proxyManager = Core::IRenderProxyManager::GetInstance();
    if (proxyManager)
    {
        proxyManager->EndFrame();
    }
}

void Application::OnResize() {}

void Application::MouseDown(WPARAM wParam, int x, int y) {}

void Application::MouseUp(WPARAM wParam, int x, int y) {}

void Application::MouseMove(WPARAM wParam, int x, int y) {}

void Application::FlushCommandQueue() {}

int Application::GetCurrentFrameIndex() const
{
    return mCurrentFrame;
}

uint64_t Application::GetCurrentFrameFenceValue() const
{
    return mFrameFenceValue[mCurrentFrame];
}

uint64_t Application::GetTotalFrameCount() const
{
    return mTotalFrameCount;
}

uint64_t Application::GetLastCompletedFenceValue() const
{
    return mLastCompletedFenceValue;
}

int Application::GetCurrentBackBufferIndex() const
{
    return mBackbufferIndex;
}

void Application::SetRenderStop(bool flag)
{

    mRenderStop = flag;
}

void Application::SetMinimizeFlag(bool flag)
{

    mIsMinimized = flag;
}

// float Application::GetAspectRatio() const
//{
//     return static_cast<float>(mClientWidth) / mClientHeight;
// }
//
// Microsoft::WRL::ComPtr<ID3D12Device> Application::GetD3D12Device() const
//{
//     return mDevice;
// }
//
// Microsoft::WRL::ComPtr<IDXGIFactory4> Application::GetD3DFactory() const
//{
//     return mFactory;
// }
//
// GraphicCommand& Application::GetGraphicCommand()
//{
//     return mGraphicCommandObject;
//     // TODO: 여기에 return 문을 삽입합니다.
// }

HINSTANCE Application::GetHinstance() const
{
    return mHinstance;
}

void Application::NotifyLogicalWindowActive(Core::LogicalWindow *logicalWindow)
{
    // 다른 모든 logicalWindow들에게 active false라고 알린다.
    // 즉 너 비활성화되었어라고알린다. 그러면 애초에 비활성화되어있던 윈도우들은 뭐 아무것도 안할거고
    // 활성화되어있다가 비활성화된 친구가 바뀌거나, 혹은, 아니면

    // 이렇게 활성화된 active window만 유지

    if (mCurrentActiveLogcialWindow)
    {
        // mCurrentActiveLogcialWindow->NotifyUnActive
        mCurrentActiveLogcialWindow->SetActiveState(false);
    }

    mCurrentActiveLogcialWindow = logicalWindow;
    mCurrentActiveLogcialWindow->SetActiveState(true);

    Quad::InputSystem *inputSystem = InputSystem::GetInstance();
    if (inputSystem)
    {
        Core::GlobalAppHelper::GetInstance()->SetCurrentKeyboardActiveLogicalWindow(logicalWindow);
        //  inputSystem->SetKeyboardInputFocusHandler(logicalWindow->GetObjectController());
    }
}

void Application::RegisterLogicalWindow(Core::LogicalWindow *logicalWindow)
{
    mLogicalWindowList.push_back(logicalWindow);
}

void Application::UnRegisterLogicalWindow(Core::LogicalWindow *logicalWindow)
{
    if (logicalWindow == nullptr)
        return;

    auto it = (std::find(mLogicalWindowList.begin(), mLogicalWindowList.end(), logicalWindow));
    if (it != mLogicalWindowList.end())
    {
        mLogicalWindowList.erase(it);
    }
}

// bool Application::GetPlayModeState()
//{
//    auto instance = GetInstance();
//     return instance->mPlayModeState;
// }

void Application::CalculateFrameStats()
{
    static int frameCnt = 0;

    static float timeElapsed = 0.0f;
    frameCnt++;

    float totalTime = TimerManager::GetInstance()->GetMainTimer().TotalTime();

    if (totalTime - timeElapsed >= 1.0f)
    {

        float fps = (float)frameCnt; // fps=frameCnt/1(s);

        float mspf = 1.000f / fps;

        std::wstring fpsStr = std::to_wstring(fps);
        std::wstring mspfStr = std::to_wstring(mspf);

        std::wstring windowText = L"fps : " + fpsStr + L" mspf : " + mspfStr;
        SetWindowTextW(mHwnd, windowText.c_str());

        frameCnt = 0;
        timeElapsed += 1.0f;
    }
}

} // namespace Quad