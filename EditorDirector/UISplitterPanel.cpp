#include "UISplitterPanel.h"
#include <UISplitterComponent.h>
#include <UiSystem/UICanvas.h>
#include <UiSystem/UIImageComponent.h>
#include <algorithm>
UISplitterPanel::UISplitterPanel()
    : mOrientation(EUISplitterPanelOrientation::eHorizontal), mFirstChildElement(nullptr), mSecondChildElement(nullptr),
      mHandleElement(nullptr)
{
    SetUseScissorRect(true);
}

UISplitterPanel::~UISplitterPanel() {}

void UISplitterPanel::Update(float deltaTime) {}

void UISplitterPanel::SetFirstChildElement(UI::UIElement *child)
{

    child->SetParent(this);
    child->SetDepthValue(3);
    mFirstChildElement = child;
    child->SetUseScissorRect(true);
    UpdateLayout();
}

void UISplitterPanel::SetSecondChildElement(UI::UIElement *child)
{

    child->SetParent(this);
    child->SetDepthValue(1);
    mSecondChildElement = child;
    child->SetUseScissorRect(true);
    UpdateLayout();
}

void UISplitterPanel::UpdateLayout()
{
    if (mFirstChildElement == nullptr || mSecondChildElement == nullptr)
        return;

    float totalWidth = mTransform.GetSize().r;
    float splitterPosX = mSplitRatio * totalWidth;

    float maxHeight = 0.0f;

    if (mFirstChildElement)
    {
        //  totalWidth = mFirstChildElement->mTransform.GetSize().r;
        maxHeight = mFirstChildElement->mTransform.GetSize().g;
    }
    if (mSecondChildElement)
    {
        //  totalWidth += mHandleWidth;
        // totalWidth += mSecondChildElement->mTransform.GetSize().r;
        maxHeight = std::max(maxHeight, mSecondChildElement->mTransform.GetSize().g);
    }

    float offsetX = 0.0f;
    if (mFirstChildElement)
    {
        mFirstChildElement->SetPositionLocal(offsetX, 0);
    }

    if (mSecondChildElement)
    {
        offsetX += (splitterPosX + mHandleWidth);

        mSecondChildElement->SetPositionLocal(offsetX, 0);
    }

    if (mHandleElement == nullptr)
    {
        mHandleElement = GetDestCanvas()->CreateUIElement<UI::UIElement>("handle");
        auto splitterCom = mHandleElement->CreateUIComponent<UISplitterComponent>("SplitterCom");
        splitterCom->SetSplitterPanel(this);

        auto imageCom = mHandleElement->CreateUIComponent<UI::UIImageComponent>("ImageCom");

        imageCom->NotUseTexture();
        imageCom->SetColor(1.0f, 0.0f, 0.5f);

        mHandleElement->SetParent(this);
        mHandleElement->SetDepthValue(0);
        mHandleElement->SetSize(mHandleWidth, maxHeight);
    }
    else
    {
        mHandleElement->SetSize(mHandleWidth, maxHeight);
    }
    mHandleElement->SetPositionLocal(splitterPosX, 0);

    // SetSize(totalWidth, maxHeight);
}
void UISplitterPanel::OnSplitterDragged(float delta)
{

    float totalWidth = mTransform.GetSize().r;
    float changeRatio = delta / totalWidth;

    mSplitRatio = std::clamp(mSplitRatio + changeRatio, 0.0f, 1.0f);

    UpdateLayout();
}
void UISplitterPanel::SetHandleWidth(float width)
{

    mHandleWidth = width;
    UpdateLayout();
}
void UISplitterPanel::OnBegin()
{

    UpdateLayout();

    SetUseScissorRect(true);
}
