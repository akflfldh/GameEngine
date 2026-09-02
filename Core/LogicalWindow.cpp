#include "LogicalWindow.h"
#include <Core/Application.h>
#include <Core/CameraComponent.h>
#include <Core/ObjectController.h>
#include <Core/SuperController.h>
#include <Core/World.h>
#include <CoreMath/Geometry.h>
#include <InputSystem/InputSystem.h>
#include <InputSystem/InputType.h>
#include <UiSystem/UIManager.h>
#include <Window/BaseWindow.h>

Core::LogicalWindow::LogicalWindow() : mCurrActiveCanvas(nullptr), mWorld(nullptr) /*, mCameraComponent(nullptr)*/
{

    Quad::Application *app = Quad::Application::GetInstance();
    if (app)
    {

        app->RegisterLogicalWindow(this);
    }
}

Core::LogicalWindow::~LogicalWindow()
{

    Quad::Application *app = Quad::Application::GetInstance();
    if (app)
    {

        app->UnRegisterLogicalWindow(this);
    }
}

void Core::LogicalWindow::Begin() {}

void Core::LogicalWindow::SetOwnerController(SuperController *superController)
{
    mSuperController = superController;

    auto windowSize = superController->GetWindowSize();
    OnResizeWindow(windowSize.first, windowSize.second);
}

void *Core::LogicalWindow::GetWindowHandle() const
{

    return mSuperController->GetWindow()->GetWindowHandle();
}

void Core::LogicalWindow::SetWorld(World *world)
{
    mWorld = world;
}
World *Core::LogicalWindow::GetWorld() const
{

    return mWorld;
}

::Map *Core::LogicalWindow::GetMap() const
{
    if (mWorld)
        return mWorld->GetCurrentMap();

    return nullptr;
}

void Core::LogicalWindow::SetActiveCanvas(UI::UICanvas *uiCanvas)
{

    if (uiCanvas == nullptr)
        return;
    mCurrActiveCanvas = uiCanvas;

    UI::UIManager *manager = UI::UIManager::GetInstance();
    manager->ActivateCanvas(mCurrActiveCanvas);
}

UI::UICanvasID Core::LogicalWindow::GetActiveCanvasID() const
{
    if (mCurrActiveCanvas)
    {
        return mCurrActiveCanvas->GetID();
    }
    else
    {
        return InvaildUICanvasID;
    }
}

UI::UICanvas *Core::LogicalWindow::GetActiveCanvas() const
{
    return mCurrActiveCanvas;
}

void Core::LogicalWindow::OnResizeWindow(uint32_t width, uint32_t height)
{
    mViewportController.UpdateWindowSize(width, height);

    Render::Viewport globalViewport = mViewportController.GetViewport();

    m3DWorldViewportController.UpdateWindowSize(globalViewport.Width, globalViewport.Height);

    if (mCurrActiveCanvas)
    {
        mCurrActiveCanvas->OnWindowResize(globalViewport.Width, globalViewport.Height);
    }

    // if (mCameraComponent != nullptr)
    //{
    //     //+ 댕글링 포인터도 확인할 방법이 필요하다.
    //     mCameraComponent->OnWindowResize(mViewportController.GetViewport().Width /
    //                                      mViewportController.GetViewport().Height);
    // }
}

Render::Viewport Core::LogicalWindow::GetGlobalSceneViewport() const
{

    return mUIGlobalFrameData.mSceneViewport;
}

Core::GlobalFrameData Core::LogicalWindow::GetGlobalFrameData() const
{
    return mGlobalFrameData;
}

Core::GlobalFrameData Core::LogicalWindow::GetUIGlobalFrameData() const
{
    return mUIGlobalFrameData;
}

void Core::LogicalWindow::UpdateFrameData()
{
    CameraComponent *cameraComponent = nullptr;
    if (mWorld)
        cameraComponent = mWorld->GetCurrentCameraCom();

    if (cameraComponent)
    {
        CoreMath::Matrix4X4 view = cameraComponent->GetViewMatrix();

        float nearZ = cameraComponent->GetNear();
        float farZ = cameraComponent->GetFar();
        // float aspect = mViewportController.GetAspect();
        float aspect = m3DWorldViewportController.GetAspect();

        // 카메라 모드를 알아야함.
        ECameraType cameraType = cameraComponent->GetType();
        CoreMath::Matrix4X4 proj;
        if (cameraType == ECameraType::ePerspective)
        {
            float fov = cameraComponent->GetFov();
            // proj = glm::perspectiveFovLH_ZO(glm::radians(fov), aspect, 1.0f, nearZ, farZ);
            proj = CoreMath::Matrix4X4::MakePerspectiveFovLH(fov, aspect, nearZ, farZ);
        }
        else
        {
            float height = cameraComponent->GetHeight();
            float width = height * aspect;

            // proj = glm::orthoLH_ZO(-width / 2, width / 2, -height / 2, height / 2, nearZ, farZ);
            proj = CoreMath::Matrix4X4::MakeOrthographicLH(-width / 2, width / 2, -height / 2, height / 2, nearZ, farZ);
        }

        mGlobalFrameData.mViewProj = proj * view;

        mGlobalFrameData.mViewProj.Transpose();

        mGlobalFrameData.mCameraPositionWorld = cameraComponent->GetPositionWorld();

        mGlobalFrameData.mAmbientLight = mWorld->GetAmbientLight();
    }

    mUIGlobalFrameData.mSceneViewport = mViewportController.GetViewport();
    // view 일단 (0,0,z= -10) 에서   z축을 방향으로 바라보는
    CoreMath::Matrix4X4 uiView(CoreMath::Matrix4X4::Identity);
    uiView.mat[3][2] = 10;

    mUIGlobalFrameData.mViewProj =
        CoreMath::Matrix4X4::MakeOrthographicLH(0.0f, mUIGlobalFrameData.mSceneViewport.Width,
                                                mUIGlobalFrameData.mSceneViewport.Height, 0.0f, 1.0f, 1000.0f) *
        uiView;

    mUIGlobalFrameData.mViewProj.Transpose();

    //  mGlobalFrameData.mSceneViewport = mViewportController.GetViewport();
    mGlobalFrameData.mSceneViewport = m3DWorldViewportController.GetViewport();
    //  mGlobalFrameData.mSceneViewport.TopLeftX += mUIGlobalFrameData.mSceneViewport.TopLeftX;
    //  mGlobalFrameData.mSceneViewport.TopLeftY += mUIGlobalFrameData.mSceneViewport.TopLeftY;

    // UIPass에서 , cpu에서 사용할때는 다시 전치해서 사용해야함.
    // mUIGlobalFrameData.mViewProj =
    //    glm::transpose(glm::orthoLH_ZO(0.0f, mUIGlobalFrameData.mSceneViewport.Width,
    //                                   mUIGlobalFrameData.mSceneViewport.Height, 0.0f, 1.0f, 1000.0f) *
    //                   uiView);

    // ui창에대해서 proj는 직교투영 (그래야 창의크기가 변해도 ui들의 크기가 동일)
    //
    // view는 일단 원점에서 z축 방향으로 바라보는 가상적인 카메라이니 일단 단위행렬
    // 그리고 update는 매 프레임 할이유가없다.
}

void Core::LogicalWindow::OnInputEvent(const Quad::RawInputData &rawInputData)
{

    if ((rawInputData.mInputState & EInputState::eMouseLButtonDown) ||
        (rawInputData.mInputState & EInputState::eMouseRButtonDown))
    {
        if (IsActive() == false)
        {
            // Notify ( 최상위 시스템에게 알린다.)
            Quad::Application *app = Quad::Application::GetInstance();
            app->NotifyLogicalWindowActive(this);
            // app은 등록된 logicalWIndow들에 모두 알린다.
            // 그렇다면 Logicalwindow들은 생성될때 app에 자신을 등록하자.
            // 파괴될때는 해제요청을하고
        }
    }

    // 일반적으로는 논리적윈도우마다 object컨트롤러가 독점한다. 다만 (마우스는 논리적윈도우까지 거쳐서)
    // 키보드는 바로 인풋시스템에 논리적윈도우가 활성화됬을때 설정

    // ObjectController를 활성화하고 끄고 하는 입력이 설정되어있거나 콜백이 있어야할듯
    // 그것을 호출해서 활성화되면 입력을 가져가고
    // 아니면 ui -> world순으로 처리되고

    // 입력을 받는데 일반적으로 소비하는 처리메서드가 아니라 활성화와 비활성화를 다루는 입력처리메서드
    // 활성화되었다면 - > 입력독점,
    // 비활성화되었다면 -> ui -> world순으로 입력이 전달되어 소비됨

    // bool bPreObjectControllerState = mObjectController.Get()->GetActiveState();

    // mObjectController.Get()->CheckToggleInput(inputData);

    // if (bPreObjectControllerState || mObjectController.Get()->GetActiveState())
    //{
    //  비활성화로 전환된 순간도 포함되어 active라고 판정한다
    // .

    Core::InputData inputDataWorld;

    inputDataWorld.mRawInputData = rawInputData;
    inputDataWorld.mNdcX = rawInputData.mouseMoveData.mClientX;
    inputDataWorld.mNdcY = rawInputData.mouseMoveData.mClientY;
    m3DWorldViewportController.ConvertToNdc(inputDataWorld.mNdcX, inputDataWorld.mNdcY,
                                            mUIGlobalFrameData.mSceneViewport.TopLeftX,
                                            mUIGlobalFrameData.mSceneViewport.TopLeftY);

    /*   CameraComponent *cameraComponent = nullptr;
       if (mWorld)
           cameraComponent = mWorld->GetCurrentCameraCom();*/

    inputDataWorld.mWorldRay = mWorldRay =
        CaculateWorldRay(rawInputData.mouseMoveData.mClientX, rawInputData.mouseMoveData.mClientY);

    // if (cameraComponent)
    //{
    //     // WorldRay계산 함수로 빼자 .

    //    CoreMath::Matrix4X4 viewProj = mGlobalFrameData.mViewProj.GetTransposed();
    //    CoreMath::Matrix4X4 invViewProj = viewProj.GetInversed();
    //    /*     CoreMath::Vector3 worldPos =
    //             invViewProj.TransformPoint(CoreMath::Vector3{inputDataWorld.mNdcX, inputDataWorld.mNdcY, 1.0f});*/

    //    CoreMath::Vector4 worldPosV4 =
    //        invViewProj * CoreMath::Vector4{inputDataWorld.mNdcX, inputDataWorld.mNdcY, 1.0f, 1.0f};

    //    // w를 1.0으로했기에사실 필요없는연산
    //    if (worldPosV4.W != 0.0f) // 0 나누기 방지
    //    {
    //        worldPosV4 /= worldPosV4.W;
    //    }
    //    CoreMath::Vector3 worldPos = CoreMath::Vector3{worldPosV4.X, worldPosV4.Y, worldPosV4.Z};
    //    inputDataWorld.mWorldRay.mOrigin = cameraComponent->GetPositionWorld();
    //    inputDataWorld.mWorldRay.mDirection = (worldPos - inputDataWorld.mWorldRay.mOrigin);
    //    inputDataWorld.mWorldRay.mDirection.Normalize();

    //    mWorldRay = inputDataWorld.mWorldRay;
    //}

    ObjectController *currentObjectController = nullptr;
    if (mWorld)
    {
        currentObjectController = mWorld->GetCurrentObjectController();
    }

    if (currentObjectController)
    {

        currentObjectController->CheckToggleInput(inputDataWorld);
    }
    if (currentObjectController && currentObjectController->GetActiveState())
    {

        currentObjectController->HandleInput(inputDataWorld);
    }
    else
    {
        // UI 먼저

        // UI에서 소비가되었다면 넘어가지않는다.
        auto uiManager = UI::UIManager::GetInstance();
        bool bConsume = false;
        if (uiManager)
        {

            Quad::RawInputData uiInputData = rawInputData;
            // ConverToWorldPos(uiInputData.)
            uiManager->HandleInput(uiInputData, *this, mCurrActiveCanvas, bConsume);
        }
        // 그후 Controller;

        if (currentObjectController && (bConsume == false))
        {
            bConsume = currentObjectController->HandleInput(inputDataWorld);
        }

        if (mWorld && (bConsume == false))
        {

            mWorld->OnInputEvent(inputDataWorld);
        }
    }
}

void Core::LogicalWindow::SetActiveState(bool state)
{

    mActiveState = state;
    if (mActiveState == false)
    {

        // world에 이벤트를 전달할수도
    }
}
bool Core::LogicalWindow::IsActive() const
{

    return mActiveState;
}

void Core::LogicalWindow::SetObjectController(ObjectController *objectController)
{
    mObjectController = objectController;
}

ObjectController *Core::LogicalWindow::GetObjectController() const
{
    return mObjectController.Get();
}

void Core::LogicalWindow::SetBackBufferClearColor(float r, float g, float b, float a)
{

    mBackBufferClearColor[0] = r;
    mBackBufferClearColor[1] = g;
    mBackBufferClearColor[2] = b;
    mBackBufferClearColor[3] = a;

    mRenderConfig.mBackBufferClearColor[0] = r;
    mRenderConfig.mBackBufferClearColor[1] = g;
    mRenderConfig.mBackBufferClearColor[2] = b;
    mRenderConfig.mBackBufferClearColor[3] = a;
}

const float *Core::LogicalWindow::GetBackBufferClearColor() const
{
    return mBackBufferClearColor;
}

void Core::LogicalWindow::OnNotifyReleasedKeyboardCapture()
{

    auto uiManager = UI::UIManager::GetInstance();
    uiManager->ReleaseKeyboardCaptureProgrammatically();

    ///    mWorld->ReleaseKeyboardCapture();
}

glm::vec2 Core::LogicalWindow::ConverToWorldPos(int clientPosX, int clientPosY) const
{

    // 마우스를 캡처한상태라면 해당 논리적윈도우채널로만 보낸다.
    float posX = clientPosX;
    float posY = clientPosY;

    Render::Viewport viewport = mViewportController.GetViewport();

    float localPosX = clientPosX - viewport.TopLeftX;
    float localPosY = clientPosY - viewport.TopLeftY;

    float worldPosX = localPosX + 0.0f; // + cameraX
    float worldPosY = localPosY + 0.0f; // + cameraY

    return {worldPosX, worldPosY};
}

Core::SuperController *Core::LogicalWindow::GetOwnerController()
{
    return mSuperController;
}

bool Core::LogicalWindow::HitTestUI(int clientPosX, int clientPosY)
{

    if (mCurrActiveCanvas)
    {
        glm::vec2 worldPos = ConverToWorldPos(clientPosX, clientPosY);
        return UI::UIManager::GetInstance()->HiTest(mCurrActiveCanvas, worldPos.r, worldPos.g);
    }

    return false;
}

const Core::WindowRenderConfig &Core::LogicalWindow::GetRenderConfig() const
{
    return mRenderConfig;
    // TODO: 여기에 return 문을 삽입합니다.
}

void Core::LogicalWindow::SetRenderConfig(const WindowRenderConfig &config)
{

    mRenderConfig = config;
}

void Core::LogicalWindow::SetDebugGridRender(bool state)
{

    mRenderConfig.bDebugGrid = state;
}

void Core::LogicalWindow::OnMouseEnter()
{

    mOnMouseEnterCallbackSystem.ExecuteCallbacks();
}

void Core::LogicalWindow::OnMouseLeave()
{
    if (mCurrActiveCanvas)
    {
        UI::UIManager::GetInstance()->OnMouseLeaveCanvas(mCurrActiveCanvas);
    }
    mOnMouseLeaveCallbackSystem.ExecuteCallbacks();
}

const CoreMath::Ray &Core::LogicalWindow::GetWorldRay() const
{
    return mWorldRay;
    // TODO: 여기에 return 문을 삽입합니다.
}
CoreMath::Ray Core::LogicalWindow::CaculateWorldRay(int clientPosX, int clientPosY) const
{
    CoreMath::Ray ray;

    float ndcX = clientPosX;
    float ndcY = clientPosY;

    m3DWorldViewportController.ConvertToNdc(ndcX, ndcY, mUIGlobalFrameData.mSceneViewport.TopLeftX,
                                            mUIGlobalFrameData.mSceneViewport.TopLeftY);

    CameraComponent *cameraComponent = nullptr;
    if (mWorld)
        cameraComponent = mWorld->GetCurrentCameraCom();
    else
        return ray;

    if (cameraComponent == nullptr)
        return ray;

    // WorldRay계산 함수로 빼자 .

    CoreMath::Matrix4X4 viewProj = mGlobalFrameData.mViewProj.GetTransposed();
    CoreMath::Matrix4X4 invViewProj = viewProj.GetInversed();
    /*     CoreMath::Vector3 worldPos =
             invViewProj.TransformPoint(CoreMath::Vector3{inputDataWorld.mNdcX, inputDataWorld.mNdcY, 1.0f});*/

    CoreMath::Vector4 worldPosV4 = invViewProj * CoreMath::Vector4{ndcX, ndcY, 1.0f, 1.0f};

    // w를 1.0으로했기에사실 필요없는연산
    if (worldPosV4.W != 0.0f) // 0 나누기 방지
    {
        worldPosV4 /= worldPosV4.W;
    }
    CoreMath::Vector3 worldPos = CoreMath::Vector3{worldPosV4.X, worldPosV4.Y, worldPosV4.Z};
    ray.mOrigin = cameraComponent->GetPositionWorld();
    ray.mDirection = (worldPos - ray.mOrigin);
    ray.mDirection.Normalize();

    //  mWorldRay = inputDataWorld.mWorldRay;

    return ray;
}