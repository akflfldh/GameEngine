#include "UiSystem/UICanvas.h"
#include "UiSystem/UIElement.h"
#include "UiSystem/UIManager.h"
UI::UICanvas::UICanvas(UICanvasID id, const std::string &name, ECanvasSizeMode sizeMode)
    : mID(id), mName(name), mActiveFlag(true), mCanvasSizeMode(sizeMode), mDepthValue(1), mTopUIElementDepthValue(0)
{
}

UI::UICanvas::~UICanvas() {}

void UI::UICanvas::Update(float deltaTime)
{
    for (auto childUIElement : mChildUIElement)
    {
        childUIElement->Update(deltaTime);
    }
}

void UI::UICanvas::AddChild(UIElement *uiElement)
{
    auto manager = UIManager::GetInstance();

    manager->AddUIElement(this, uiElement);
}

UI::ECanvasSizeMode UI::UICanvas::GetSizeMode() const
{
    return mCanvasSizeMode;
}

void UI::UICanvas::SetSize(CoreMath::Vector2 size)
{
    if (mCanvasSizeMode == ECanvasSizeMode::eScreenSize)
    {
        mSize = size; // 항상 스크린영역의 크기와 동일

        // 자식들에게 영향을 준다
    }
}

bool UI::UICanvas::GetActiveFlag() const
{
    return mActiveFlag;
}

void UI::UICanvas::AddChildInternal(UIElement *uiElement)
{
    mChildUIElement.push_back(uiElement);
    uiElement->mDestCanvas = this;
}

UI::UIElement *UI::UICanvas::CreateUIElement(const char *className, const char *instanceName)
{
    UIManager *manager = UIManager::GetInstance();
    UIElement *uiElement = manager->CreateUIElement(className, instanceName);

    if (uiElement == nullptr)
        return nullptr;

    AddChild(uiElement);

    return uiElement;
}

const std::vector<UI::UIElement *> &UI::UICanvas::GetChildUIElementAll() const
{
    return mChildUIElement;
}

void UI::UICanvas::SetDepthValue(uint32_t value)
{

    mDepthValue = value;
}
uint32_t UI::UICanvas::GetDepthValue() const
{

    return mDepthValue;
}

uint32_t UI::UICanvas::GetTopUIElementDepthValue() const
{
    return mTopUIElementDepthValue;
}

void UI::UICanvas::SetUIElementTopDepth(UIElement *uiElement)
{

    if (uiElement == nullptr)
        return;
    mTopUIElementDepthValue++;
    uiElement->SetDepthValue(mTopUIElementDepthValue);
}
