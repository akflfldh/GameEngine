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

void UI::UICanvas::EndUpdate(float deltaTime)
{

    for (auto childUIElement : mChildUIElement)
    {
        childUIElement->EndUpdate(deltaTime);
    }
}

void UI::UICanvas::AddChild(UIElement *uiElement)
{
    auto manager = UIManager::GetInstance();

    manager->AddUIElement(this, uiElement);
}

void UI::UICanvas::DestroyAllUIElements()
{

    for (auto element : mTopChildUIElementList)
    {
        element->Destroy();
    }

    for (auto element : mPendingAddList)
    {
        element->Destroy();
    }

    mTopChildUIElementList.clear();
    mPendingAddList.clear();


}

void UI::UICanvas::UpdateScissorRectRegions()
{
    // 마지막 Update 이후 생성된 요소도 이번 렌더에 포함될 수 있으므로 먼저 등록한다.
    ProcessPendingElements();
    for (UIElement *element : mTopChildUIElementList)
    {
        if (element != nullptr && !element->GetDeadState() && element->GetParent() == nullptr)
            element->UpdateScissorRectRegion();
    }
}

void UI::UICanvas::OnWindowResize(float w, float h)
{
    SetSize({w, h});

    for (auto element : mTopChildUIElementList)
    {
        element->OnWindowResize(w, h);
    }
}

void UI::UICanvas::OnInputActivationChanged(bool active)
{
    // 재연결이나 같은 상태의 통지가 UI의 닫기 동작을 중복 실행하지 않도록 실제 전환만 알린다.
    if (mInputActive == active)
        return;

    mInputActive = active;
    mOnInputActivationChangedCallbackSystem.ExecuteCallbacks(active);
}

void UI::UICanvas::OnPreviewMouseDown(UIElement *hitElement)
{
    // 빈 공간 클릭도 팝업을 닫는 입력이므로 nullptr를 걸러내지 않는다.
    mOnPreviewMouseDownCallbackSystem.ExecuteCallbacks(hitElement);
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

void UI::UICanvas::SetActiveFlag(bool flag)
{

    mActiveFlag = flag;
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

    mChildUIElement.erase(std::remove_if(mChildUIElement.begin(), mChildUIElement.end(),
                                         [](UI::UIElement *element) { return element->GetDeadState(); }),
                          mChildUIElement.end());

    mTopChildUIElementList.erase(std::remove_if(mTopChildUIElementList.begin(), mTopChildUIElementList.end(),
                                                [](UIElement *element) { return element->GetDeadState(); }),
                                 mTopChildUIElementList.end());

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

void UI::UICanvas::SetTheme(const UITheme &theme)
{

    mTheme = theme;
}

const UI::UITheme &UI::UICanvas::GetTheme() const
{
    return mTheme;

    // TODO: 여기에 return 문을 삽입합니다.
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
    return CreateUIElement(className, instanceName, {});
}

UI::UIElement *UI::UICanvas::CreateUIElement(const char *className, const char *instanceName,
                                             const std::function<void(UIElement *)> &initialize)
{
    UIManager *manager = UIManager::GetInstance();
    UIElement *uiElement = manager->CreateUIElement(className, instanceName);

    if (uiElement == nullptr)
        return nullptr;

    AddChild(uiElement);
    mPendingAddList.push_back(uiElement);
    uiElement->mDestCanvas = this;

    // 크기/폰트가 필요한 복합 UI의 OnBegin보다 먼저 호출한다. 콜백은 보관하지 않는다.
    if (initialize)
        initialize(uiElement);

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

    // 순서 - ui렌더프록시 등록 이후 - 다음  1. 일반자식 ui 2. popup 자식 ui
    // 그 ui가 popup스코프를 가지는지 확인, 현재 그 scope 렌더리스트에 집어넣어
    // 그러다가 더 자식 ui에 새로운 popup스코프가있다 그럼 처리할 최신스코프는 그 스코프 렌더리스트가 되고
    // 계속 넣다가 그 스코프오너의 모든 자식들을처리햇다면 그 이후에 그 스코프 popup들을 렌더처리

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

        // POPUP이 가장 마지막에 처리 (즉 맨위로 올라와야한다)
        const auto &children = uiElement->GetChildVector();
        for (auto it = children.rbegin(); it != children.rend(); ++it)
        {
            //  if ((*it)->GetRenderLayer() == EUIRenderLayer::ePopup)
            elementstack.push(*it);
        }

        /*    for (auto it = children.rbegin(); it != children.rend(); ++it)
            {
                if ((*it)->GetRenderLayer() == EUIRenderLayer::eNormal)
                    elementstack.push(*it);
            }*/
    }

    mIsRenderableListDirty = false;
}

void UI::UICanvas::RemoveUIElementFromList(UI::UIElement *uiElement)
{
    auto it = std::find(mChildUIElement.begin(), mChildUIElement.end(), uiElement);

    if (it != mChildUIElement.end())
    {

        mChildUIElement.erase(it);
    }

    auto itTop = std::find(mTopChildUIElementList.begin(), mTopChildUIElementList.end(), uiElement);

    if (itTop != mTopChildUIElementList.end())
    {
        mTopChildUIElementList.erase(itTop);
    }

    auto itPen = std::find(mPendingAddList.begin(), mPendingAddList.end(), uiElement);

    if (itPen != mPendingAddList.end())
    {
        mPendingAddList.erase(itPen);
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

const UI::UIControlStyle &UI::UITheme::GetStyle(EUIStyleRole role) const
{

    return mStyles[(uint8_t)role];

    // TODO: 여기에 return 문을 삽입합니다.
}

const UI::UIPalette &UI::UITheme::GetPalette() const
{
    // TODO: 여기에 return 문을 삽입합니다.
    return mPalette;
}

void UI::UITheme::SetStyle(EUIStyleRole role, const UIControlStyle &style)
{

    mStyles[(uint8_t)role] = style;
}
