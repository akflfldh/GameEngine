#include "UIGridLayoutComponent.h"
#include <UiSystem/UIElement.h>
UIGridLayoutComponent::UIGridLayoutComponent() {}

UIGridLayoutComponent::~UIGridLayoutComponent() {}

void UIGridLayoutComponent::Update(float deltaTime) {}

void UIGridLayoutComponent::OnTransformChanged(UI::ETransformChangeType type)
{

    // 높이는 이  컴포넌트가 자동으로 강제 설정한다.
    if (type == UI::ETransformChangeType::eSize || type == UI::ETransformChangeType::eAll)
    {

        CalculateLayout();
    }
}

void UIGridLayoutComponent::CalculateLayout()
{
    if (mIsCalculating)
        return;

    mIsCalculating = true;

    auto ownerElement = GetOwnerUIElement();

    float maxWidth = ownerElement->mTransform.GetSize().r;

    float x = mSpacing;
    float y = mSpacing;

    for (auto child : ownerElement->GetChildVector())
    {
        if (child == nullptr || child->GetDeadState() || !child->GetActiveFlag())
            continue;

        child->SetSize(mCellWidth, mCellHeight);

        // 넘어가는경우 다음줄로
        if ((x + mCellWidth + mSpacing) > maxWidth)
        {
            // 첫번째 경우에는 통과
            if (x > mSpacing)
            {
                y += (mCellHeight + mSpacing);
                x = mSpacing;
            }
        }

        child->SetPositionLocal(x, y);
        x += (mCellWidth + mSpacing);
    }

    float ownerNewHeight = std::max(mMinHeight, y + mCellHeight + mSpacing);

    if (std::abs(ownerNewHeight - ownerElement->mTransform.GetSize().g) > 1.0f)
    {
        ownerElement->SetSize(maxWidth, ownerNewHeight);
    }

    mIsCalculating = false;
}

// void UIGridLayoutComponent::SetCellSize(float width, float height)
//{
//
//     mCellHeight = height;
//     mCellWidth = width;
//     CalculateLayout();
// }

void UIGridLayoutComponent::SetSpacing(float spacing)
{

    mSpacing = spacing;
    CalculateLayout();
}

void UIGridLayoutComponent::SetMinHeight(float height)
{

    mMinHeight = height;
    // CalculateLayout();
}

void UIGridLayoutComponent::SetItemSize(float w, float h)
{

    mCellHeight = h;
    mCellWidth = w;
    CalculateLayout();
}
