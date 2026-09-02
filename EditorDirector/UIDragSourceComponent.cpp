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
    mPress = true;
    mMouseDownStartX = worldPosX;
    mMouseDownStartY = worldPosY;

    // bConsume = true;
    RequestMouseCaptureInput();
}

void UIDragSourceComponent::OnMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                      bool &bConsume)
{

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

    if (!mPress)
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