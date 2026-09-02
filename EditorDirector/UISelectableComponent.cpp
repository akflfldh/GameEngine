#include "UISelectableComponent.h"

UISelectableComponent::UISelectableComponent() : mHover(false), mPress(false) {}

UISelectableComponent::~UISelectableComponent() {}

void UISelectableComponent::Update(float deltaTime) {}

bool UISelectableComponent::IsHovered() const
{
    return mHover;
}

int UISelectableComponent::IsPointInside(float x, float y) const
{
    return IsPointInsideDefault(x, y);
}

void UISelectableComponent::OnHover(int x, int y)
{

    mHover = true;
}

void UISelectableComponent::OnReleaseHover()
{

    mHover = false;
}

void UISelectableComponent::OnMouseDown(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                        bool &bConsume)
{

    if (mHover)
    {
        mPress = true;
        SetSelect(true, true);
    }
}

void UISelectableComponent::OnMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                      bool &bConsume)
{

    if (mPress)
    {
        mPress = false;
    }
}

void UISelectableComponent::SetSelect(bool bSelected, bool nofity)
{

    if (mSelect != bSelected)
    {

        mSelect = bSelected;

        // 콜백시스템 호출
        mOnSelectedCallbackSystem.ExecuteCallbacks(mSelect);
    }
}
