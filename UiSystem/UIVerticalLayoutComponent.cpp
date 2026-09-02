#include "UIVerticalLayoutComponent.h"
#include "UIElement.h"
UI::UIVerticalLayoutComponent::UIVerticalLayoutComponent() {}

UI::UIVerticalLayoutComponent::~UIVerticalLayoutComponent() {}

void UI::UIVerticalLayoutComponent::Update(float deltaTime) {}

void UI::UIVerticalLayoutComponent::OnTransformChanged(ETransformChangeType type)
{

    /*   if (type == ETransformChangeType::eSize && !mIsCalculating)
           CalculateLayout();*/
}

void UI::UIVerticalLayoutComponent::OnRemoved()
{

    auto ownerElement = GetOwnerUIElement();
    if (ownerElement)
    {
        ownerElement->mOnAddedChildElementCallbackSystem.UnRegister(mOwnerElementOnAddedCallbackID);
        ownerElement->mOnRemovedChildElementCallbackSystem.UnRegister(mOwnerElementOnRemovedCallbackID);

        for (auto child : ownerElement->GetChildVector())
        {

            auto it = mSizeCallbackIDTable.find(child);
            if (it != mSizeCallbackIDTable.end())
            {
                child->mOnChangedSizeCallbackSystem.UnRegister(it->second);
                mSizeCallbackIDTable.erase(it);
            }

            auto it2 = mActiveCallbackIDTable.find(child);
            if (it2 != mActiveCallbackIDTable.end())
            {
                child->mOnActiveElementCallbackSystem.UnRegister(it2->second);
                mActiveCallbackIDTable.erase(it2);
            }
        }
    }
}

void UI::UIVerticalLayoutComponent::OnBegin()
{
    UI::UILayoutComponent::OnBegin();

    auto ownerElement = GetOwnerUIElement();

    if (ownerElement)
    {
        // 새 자식추가시
        mOwnerElementOnAddedCallbackID = ownerElement->mOnAddedChildElementCallbackSystem.Register(
            [this](UI::UIElement *child)
            {
                // 자식에게 사이즈 변화시 호출될 콜백 등록
                Core::CallbackID callbackID =
                    child->mOnChangedSizeCallbackSystem.Register([this](UI::UIElement *element) { CalculateLayout(); });

                // 콜백 id 저장
                mSizeCallbackIDTable[child] = callbackID;

                // 자식이 활성화되었을때 호출될 콜백 등록
                callbackID =
                    child->mOnActiveElementCallbackSystem.Register([this](bool bActive) { CalculateLayout(); });

                // 콜백 id 등록
                mActiveCallbackIDTable[child] = callbackID;

                // 업데이트
                CalculateLayout();
            });

        // 자식 제거시 콜백제거
        mOwnerElementOnRemovedCallbackID = ownerElement->mOnRemovedChildElementCallbackSystem.Register(
            [this](UI::UIElement *child)
            {
                auto it = mSizeCallbackIDTable.find(child);
                if (it != mSizeCallbackIDTable.end())
                {
                    child->mOnChangedSizeCallbackSystem.UnRegister(it->second);
                    mSizeCallbackIDTable.erase(it);
                }

                auto it2 = mActiveCallbackIDTable.find(child);
                if (it2 != mActiveCallbackIDTable.end())
                {
                    child->mOnActiveElementCallbackSystem.UnRegister(it2->second);
                    mActiveCallbackIDTable.erase(it2);
                }

                CalculateLayout();
            });

        // Component begin보다 먼저 자식으로 들어간 element에대해서 콜백등록
        for (auto child : ownerElement->GetChildVector())
        {
            Core::CallbackID callbackID =
                child->mOnChangedSizeCallbackSystem.Register([this](UI::UIElement *element) { CalculateLayout(); });

            mSizeCallbackIDTable[child] = callbackID;

            callbackID = child->mOnActiveElementCallbackSystem.Register([this](bool bActive) { CalculateLayout(); });

            mActiveCallbackIDTable[child] = callbackID;
        }
    }
}

void UI::UIVerticalLayoutComponent::CalculateLayout()
{

    mIsCalculating = true;

    UIElement *ownerElement = GetOwnerUIElement();

    float currentY = 0.0f;

    for (auto child : ownerElement->GetChildVector())
    {
        if (!child || child->GetDeadState() || !child->GetActiveFlag())
            continue;

        float h = child->mTransform.GetSize().g;

        child->SetPositionLocal(mPaddingX, currentY);

        currentY += h;
        currentY = glm::round(currentY);
    }

    ownerElement->SetHeight(currentY);

    mIsCalculating = false;
}

void UI::UIVerticalLayoutComponent::SetItemSize(float w, float h) {}

void UI::UIVerticalLayoutComponent::SetItemPaddingX(float x)
{

    mPaddingX = x;
    CalculateLayout();
}