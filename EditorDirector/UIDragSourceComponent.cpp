#include "UIDragSourceComponent.h"
#include <GlobalOverlayManager.h>
#include <glm/glm.hpp>
UIDragSourceComponent::UIDragSourceComponent()
    : mHover(false), mPress(false), mMouseDownStartX(0.0F), mMouseDownStartY(0.0f)
{
}

UIDragSourceComponent::~UIDragSourceComponent() {}

void UIDragSourceComponent::Update(float deltaTime) {}

int UIDragSourceComponent::IsPointInside(float x, float y) const
{
    return IsPointInsideDefault(x, y);
}

bool UIDragSourceComponent::IsHovered() const
{
    return mHover;
}

void UIDragSourceComponent::OnHover(int x, int y)
{

    mHover = true;
}

void UIDragSourceComponent::OnReleaseHover()
{

    mHover = false;
    // mPress = false;
}

void UIDragSourceComponent::OnMouseMove(const Quad::RawInputData &inputData, float worldPosX, float worldPosY)
{

    if (mPress && (mDragStart == false))
    {
        float dis = glm::distance(glm::vec2{worldPosX, worldPosY}, glm::vec2{mMouseDownStartX, mMouseDownStartY});

        if (dis > 5.0f)
        {
            DragPayload payload;

            GlobalOverlayManager::GetInstance()->StartDragDrop(mPayload);

            //  ReleaseMouseCaptureInput();

            mDragStart = true;
            //  mPress = false;
        }
    }
}

void UIDragSourceComponent::OnMouseDown(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                        bool &bConsume)
{
    // 우클릭 메뉴 버튼과 같은 요소에 붙으므로 드래그 캡처는 왼쪽 버튼으로만 시작한다.
    if (!(inputData.mInputState & EInputState::eMouseLButtonDown) || mPress)
        return;

    mPress = true;
    mMouseDownStartX = worldPosX;
    mMouseDownStartY = worldPosY;

    // bConsume = true;
    RequestMouseCaptureInput();
}

void UIDragSourceComponent::OnMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                      bool &bConsume)
{
    if (!(inputData.mInputState & EInputState::eMouseLButtonUp))
        return;

    if (mPress)
    {

        if (mDragStart)
        {
            GlobalOverlayManager *overlayManager = GlobalOverlayManager::GetInstance();
            if (overlayManager)
            {
                overlayManager->TryDropCurrentPayload();
            }
        }

        mPress = false;
        mDragStart = false;
        bConsume = true;
        ReleaseMouseCaptureInput();
    }
}

void UIDragSourceComponent::SetPayload(const DragPayload &payload)
{

    mPayload = payload;
}

void UIDragSourceComponent::OnPreviewMouseMove(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                               bool &bSteal)
{

    if (mPress)
    {
        bSteal = true;
    }

    OnMouseMove(inputData, worldPosX, worldPosY);
}

void UIDragSourceComponent::OnPreviewMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                             bool &bSteal)
{
    // preview 경로에서도 우클릭 해제가 진행 중인 왼쪽 드래그를 종료하지 않도록 한다.
    if (!mPress || !(inputData.mInputState & EInputState::eMouseLButtonUp))
        return;

    if (mDragStart)
    {
        GlobalOverlayManager *overlayManager = GlobalOverlayManager::GetInstance();
        if (overlayManager)
            overlayManager->TryDropCurrentPayload();

        mDragStart = false;
    }
    mPress = false;
    ReleaseMouseCaptureInput();
}
