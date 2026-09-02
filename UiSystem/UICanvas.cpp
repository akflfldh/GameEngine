#include "UiSystem/UICanvas.h"
#include "UiSystem/UIElement.h"
#include "UiSystem/UIManager.h"
#include <UiSystem/UIRenderableComponent.h>
#include <algorithm>
#include <stack>
UI::UICanvas::UICanvas(UICanvasID id, const std::string &name, ECanvasSizeMode sizeMode)
    : mID(id), mName(name), mActiveFlag(true), mCanvasSizeMode(sizeMode), mDepthValue(1), mTopUIElementDepthValue(0),
      mRefCount(0), mIsBegin(false)
{
}

UI::UICanvas::~UICanvas() {}

UI::UICanvasID UI::UICanvas::GetID() const
{

    return mID;
}

void UI::UICanvas::Begin()
{
    ProcessPendingElements();
    mIsBegin = true;

    for (auto childUIElement : mChildUIElement)
    {
        childUIElement->Begin();
    }
}

void UI::UICanvas::Update(float deltaTime)
{
    ProcessPendingElements();

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

void UI::UICanvas::OnWindowResize(float w, float h)
{
    SetSize({w, h});

    for (auto element : mTopChildUIElementList)
    {
        element->OnWindowResize(w, h);
    }
}
CoreMath::Vector2 UI::UICanvas::GetWindowSize() const
{

    return mWindowSize;
}
void UI::UICanvas::MarkDirty()
{
    mIsRenderableListDirty = true;
}

void UI::UICanvas::IncreaseRefCount()
{

    mRefCount++;
}

void UI::UICanvas::DecreaseRefCount()
{

    mRefCount--;
}
uint32_t UI::UICanvas::GetRefCount() const
{

    return mRefCount;
}

UI::ECanvasSizeMode UI::UICanvas::GetSizeMode() const
{
    return mCanvasSizeMode;
}

void UI::UICanvas::SetSize(CoreMath::Vector2 size)
{
    mWindowSize = size;
}

bool UI::UICanvas::GetActiveFlag() const
{
    return mActiveFlag;
}

void UI::UICanvas::DestroyUIElement(UIElement *uiElement)
{

    std::stack<UI::UIElement *> st;
    st.push(uiElement);

    MarkDirty();

    while (st.empty() == false)
    {

        UI::UIElement *element = st.top();
        st.pop();

        if (element->GetDeadState())
            continue;

        element->SetDeadState();

        // RemoveUIElementFromList(element);

        for (auto child : element->mChildVector)
        {
            st.push(child);
        }
        UIManager::GetInstance()->DestoryUIElement(element);
    }
}

const std::vector<UI::UIRenderProxy *> &UI::UICanvas::GetRenderProxyList()
{
    // TODO: 여기에 return 문을 삽입합니다.
    if (mIsRenderableListDirty)
    {
        RebuildRenderProxyList();
    }

    return mCachedRenderProxyList;
}

const std::vector<UI::UIRenderProxy *> &UI::UICanvas::GetRenderProxyLists()
{
    if (mIsRenderableListDirty)
    {
        RebuildRenderProxyList();
    }
    return mCachedRenderProxyList;
}

void UI::UICanvas::ProcessPendingElements()
{

    size_t preNum = mChildUIElement.size();

    std::remove_if(mChildUIElement.begin(), mChildUIElement.end(),
                   [](UI::UIElement *element) { return element->GetDeadState(); });

    std::remove_if(mTopChildUIElementList.begin(), mTopChildUIElementList.end(),
                   [](UIElement *element) { return element->GetDeadState(); });

    if (mPendingAddList.size() > 0 || preNum != mChildUIElement.size())
        MarkDirty();

    bool bAddedTopChildElement = false;
    for (auto element : mPendingAddList)
    {

        mChildUIElement.push_back(element);

        if (element->GetParent() == nullptr)
        {
            bAddedTopChildElement = true;
            mTopChildUIElementList.push_back(element);
        }
        // element->SetCanvasInternal(this);
    }

    if (bAddedTopChildElement)
        SortTopChildElement();

    mPendingAddList.clear();
}

void UI::UICanvas::SortTopChildElement()
{
    std::sort(mTopChildUIElementList.begin(), mTopChildUIElementList.end(),
              [](UI::UIElement *a, UI::UIElement *b) { return a->GetDepthValue() > b->GetDepthValue(); });
}

UI::UIElement *UI::UICanvas::GetHittedElement(float x, float y) const
{

    uint32_t topUIElementDepthValue = UINT_MAX;

    const std::vector<UIElement *> &childUIElementList = GetTopChildUIElement();

    UI::UIElement *targetElement = nullptr;

    for (auto it = childUIElementList.rbegin(); it != childUIElementList.rend(); ++it)
    {
        UIElement *element = *it;

        UIElement *hoverElement = FindHittedElementRecursive(element, x, y);

        if (hoverElement)
        {
            targetElement = hoverElement;
            break;
        }
    }
    return targetElement;
}

void UI::UICanvas::AddChildInternal(UIElement *uiElement)
{
    /*  if (uiElement->GetParent() == nullptr)
      {
          mTopChildUIElement.push_back(uiElement);
      }

      mChildUIElement.push_back(uiElement);
      uiElement->mDestCanvas = this;*/
}

UI::UIElement *UI::UICanvas::CreateUIElement(const char *className, const char *instanceName)
{
    UIManager *manager = UIManager::GetInstance();
    UIElement *uiElement = manager->CreateUIElement(className, instanceName);

    if (uiElement == nullptr)
        return nullptr;

    AddChild(uiElement);
    mPendingAddList.push_back(uiElement);
    uiElement->mDestCanvas = this;

    if (mIsBegin)
        uiElement->Begin();

    uiElement->SetCanvasInternal(this);

    MarkDirty();

    return uiElement;
}

void UI::UICanvas::RebuildRenderProxyList()
{
    mCachedRenderProxyList.clear();
    std::stack<UIElement *> elementstack;

    for (auto it = mTopChildUIElementList.rbegin(); it != mTopChildUIElementList.rend(); ++it)
        elementstack.push(*it);

    while (elementstack.empty() == false)
    {

        UIElement *uiElement = elementstack.top();
        elementstack.pop();

        if (uiElement->GetDeadState() || !uiElement->GetActiveFlag())
            continue;

        size_t size = 0;
        size = uiElement->GetComponentsNum<UI::UIRenderableComponent>();

        std::vector<UI::UIRenderableComponent *> renderableComVec(size, nullptr);
        uiElement->GetComponents<UI::UIRenderableComponent>(renderableComVec.data(), size);

        for (auto com : renderableComVec)
        {
            //    UI::UIRenderableComponent *renderableCom = static_cast<UI::UIRenderableComponent *>(com);

            if (com->GetActiveState())
                mCachedRenderProxyList.push_back(com->GetRenderProxy());
        }

        /* for (auto child : uiElement->GetChildVector())
         {
             elementstack.push(child);
         }*/
        const auto &children = uiElement->GetChildVector();
        for (auto it = children.rbegin(); it != children.rend(); ++it)
        {
            elementstack.push(*it);
        }
    }

    mIsRenderableListDirty = false;
}

void UI::UICanvas::RemoveUIElementFromList(UI::UIElement *uiElement)
{
    auto it = std::find(mChildUIElement.begin(), mChildUIElement.end(), uiElement);

    if (it == mChildUIElement.end())
        return;

    mChildUIElement.erase(it);

    auto itTop = std::find(mTopChildUIElementList.begin(), mTopChildUIElementList.end(), uiElement);

    if (itTop != mTopChildUIElementList.end())
    {
        mTopChildUIElementList.erase(itTop);
    }
}

UI::UIElement *UI::UICanvas::FindHittedElementRecursive(UI::UIElement *element, float x, float y) const
{
    if ((element == nullptr) || (element->GetActiveFlag() == false) || (element->GetOnlyVisible()))
        return nullptr;

    if (element->IsPointInside(x, y))
    {
        const std::vector<UIElement *> &childVec = element->GetChildVector();
        for (auto it = childVec.rbegin(); it != childVec.rend(); ++it)
        {
            UIElement *childElement = *it;

            UIElement *hoverElement = FindHittedElementRecursive(childElement, x, y);

            if (hoverElement != nullptr)
            {
                return hoverElement;
            }
        }

        return element;
    }

    return nullptr;
}

const std::vector<UI::UIElement *> &UI::UICanvas::GetChildUIElementAll() const
{

    return mChildUIElement;
}

const std::vector<UI::UIElement *> &UI::UICanvas::GetTopChildUIElement() const
{
    return mTopChildUIElementList;
    // TODO: 여기에 return 문을 삽입합니다.
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
