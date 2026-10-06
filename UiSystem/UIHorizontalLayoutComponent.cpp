#include "UIHorizontalLayoutComponent.h"
#include "UIElement.h"

UI::UIHorizontalLayoutComponent::UIHorizontalLayoutComponent() : mPaddingTop(0.0F), mRightAlign(false) {}

UI::UIHorizontalLayoutComponent::~UIHorizontalLayoutComponent() {}

void UI::UIHorizontalLayoutComponent::Update(float deltaTime) {}

void UI::UIHorizontalLayoutComponent::OnTransformChanged(ETransformChangeType type)
{
    if (type == ETransformChangeType::eSize || type == ETransformChangeType::eAll)
        CalculateLayout();
}

void UI::UIHorizontalLayoutComponent::OnBegin()
{
    UI::UILayoutComponent::OnBegin();

    auto ownerElement = GetOwnerUIElement();
    if (ownerElement == nullptr || mOwnerElementOnAddedCallbackID != Core::CallbackIDNone)
        return;

    mOwnerElementOnAddedCallbackID = ownerElement->mOnAddedChildElementCallbackSystem.Register(
        [this](UIElement *child)
        {
            RegisterChildCallbacks(child);
            CalculateLayout();
        });

    mOwnerElementOnRemovedCallbackID = ownerElement->mOnRemovedChildElementCallbackSystem.Register(
        [this](UIElement *child)
        {
            UnRegisterChildCallbacks(child);
            CalculateLayout();
        });

    // Begin보다 먼저 추가된 자식도 동일한 갱신 경로를 사용한다. 이후 호출부가
    // 자식 크기를 바꾸면 크기 변경 알림에서 확정된 높이와 너비로 다시 배치한다.
    for (auto child : ownerElement->GetChildVector())
        RegisterChildCallbacks(child);

    CalculateLayout();
}

void UI::UIHorizontalLayoutComponent::RegisterChildCallbacks(UIElement *child)
{
    if (child == nullptr || child->GetDeadState() || mSizeCallbackIDTable.find(child) != mSizeCallbackIDTable.end())
        return;

    mSizeCallbackIDTable[child] =
        child->mOnChangedSizeCallbackSystem.Register([this](UIElement *element) { CalculateLayout(); });
    mActiveCallbackIDTable[child] =
        child->mOnActiveElementCallbackSystem.Register([this](bool active) { CalculateLayout(); });
}

void UI::UIHorizontalLayoutComponent::UnRegisterChildCallbacks(UIElement *child)
{
    if (child == nullptr)
        return;

    auto sizeIt = mSizeCallbackIDTable.find(child);
    if (sizeIt != mSizeCallbackIDTable.end())
    {
        child->mOnChangedSizeCallbackSystem.UnRegister(sizeIt->second);
        mSizeCallbackIDTable.erase(sizeIt);
    }

    auto activeIt = mActiveCallbackIDTable.find(child);
    if (activeIt != mActiveCallbackIDTable.end())
    {
        child->mOnActiveElementCallbackSystem.UnRegister(activeIt->second);
        mActiveCallbackIDTable.erase(activeIt);
    }
}

void UI::UIHorizontalLayoutComponent::OnRemoved()
{
    auto ownerElement = GetOwnerUIElement();
    if (ownerElement == nullptr)
        return;

    ownerElement->mOnAddedChildElementCallbackSystem.UnRegister(mOwnerElementOnAddedCallbackID);
    ownerElement->mOnRemovedChildElementCallbackSystem.UnRegister(mOwnerElementOnRemovedCallbackID);
    mOwnerElementOnAddedCallbackID = Core::CallbackIDNone;
    mOwnerElementOnRemovedCallbackID = Core::CallbackIDNone;

    for (auto child : ownerElement->GetChildVector())
        UnRegisterChildCallbacks(child);
}

void UI::UIHorizontalLayoutComponent::CalculateLayout()
{
    auto ownerElement = GetOwnerUIElement();
    if (ownerElement == nullptr || mIsCalculating)
        return;

    mIsCalculating = true;

    // 행의 높이를 자식의 최대 높이로 정해 바깥 VerticalLayout이 이 행을 배치할 수 있게 한다.
    // 부모 너비와 자식 크기는 유지하므로 공간이 부족해도 자동 축소나 줄바꿈은 하지 않는다.
    float rowHeight = 0.0f;
    for (auto child : ownerElement->GetChildVector())
    {
        if (child == nullptr || child->GetDeadState() || !child->GetActiveFlag())
            continue;

        rowHeight = glm::max(rowHeight, child->GetSize().Y);
    }
    rowHeight += mPaddingTop;

    // SetHeight의 크기 알림이 다시 이 컴포넌트를 호출할 수 있으므로 재진입을 막고,
    // 높이가 실제로 바뀔 때만 알림을 발생시킨다.
    if (ownerElement->GetSize().Y != rowHeight)
        ownerElement->SetHeight(rowHeight);

    float dir = mRightAlign ? -1 : 1;

    float currentX = dir * mGlboalPaddingX;
    if (mRightAlign)
    {
        currentX += ownerElement->GetWidth();
    }

    bool hasPreviousChild = false;

    for (auto child : ownerElement->GetChildVector())
    {
        if (child == nullptr || child->GetDeadState() || !child->GetActiveFlag())
            continue;

        const float y = mPaddingTop;
        const auto childSize = child->GetSize();
        if (mRightAlign)
        {
            currentX -= (mPaddingX + childSize.X);
            if (currentX < 0.0f)
                currentX = 0.0f;
        }
        else
        {
            if (hasPreviousChild)
                currentX += (dir * mPaddingX);
        }

        const auto position = child->mTransform.GetLocalPosition();
        if (position.x != currentX || position.y != y)
            child->SetPositionLocal(currentX, y);

        if (mRightAlign == false)
            currentX += (dir * childSize.X);

        currentX = glm::round(currentX);
        hasPreviousChild = true;
    }

    mIsCalculating = false;
}

void UI::UIHorizontalLayoutComponent::SetItemSize(float w, float h) {}

void UI::UIHorizontalLayoutComponent::SetItemPaddingX(float x)
{
    mPaddingX = std::max(0.0f, x);
    CalculateLayout();
}

void UI::UIHorizontalLayoutComponent::SetItemPaddingTop(float v)
{
    mPaddingTop = std::max(0.0f, v);
    CalculateLayout();
}

void UI::UIHorizontalLayoutComponent::SetGlobalPaddingX(float x)
{

    mGlboalPaddingX = std::max(0.0f, x);
    CalculateLayout();
}

void UI::UIHorizontalLayoutComponent::SetRightAlign(bool flag)
{

    mRightAlign = flag;
    CalculateLayout();
}